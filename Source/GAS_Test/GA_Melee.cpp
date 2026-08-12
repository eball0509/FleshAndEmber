#include "GA_Melee.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GASCharacterBase.h"
#include "DemoAttributes.h"

UGA_Melee::UGA_Melee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Melee::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!MeleeMontage)
	{
		ResolveHit();
		FinishMeleeAbility();
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		MeleeMontage,
		1.f,
		NAME_None,
		true
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UGA_Melee::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_Melee::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_Melee::OnMontageCancelled);
		MontageTask->ReadyForActivation();
		UE_LOG(LogTemp, Warning, TEXT("GA_Melee: montage task created and activated"));
	}
	else
	{
		ResolveHit();
		FinishMeleeAbility();
	}
}

void UGA_Melee::OnMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("GA_Melee: OnCompleted fired"));
	ResolveHit();
	FinishMeleeAbility();
}

void UGA_Melee::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("GA_Melee: OnInterrupted fired"));
	ResolveHit();
	FinishMeleeAbility();
}

void UGA_Melee::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("GA_Melee: OnCancelled fired"));
	ResolveHit();
	FinishMeleeAbility();
}

void UGA_Melee::ResolveHit()
{
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	AActor* AvatarActor = Info ? Info->AvatarActor.Get() : nullptr;
	AGASCharacterBase* Attacker = Cast<AGASCharacterBase>(AvatarActor);
	if (!Attacker)
	{
		return;
	}

	const FVector Origin = Attacker->GetActorLocation() + Attacker->GetActorForwardVector() * HitSphereForwardOffset;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Attacker);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		Attacker,
		Origin,
		HitSphereRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_Pawn) },
		AGASCharacterBase::StaticClass(),
		IgnoreActors,
		OutActors
	);

	UE_LOG(LogTemp, Warning, TEXT("GA_Melee: overlap found %d actors"), OutActors.Num());

#if !UE_BUILD_SHIPPING
	if (bDrawDebugSphere)
	{
		UKismetSystemLibrary::DrawDebugSphere(Attacker, Origin, HitSphereRadius, 12, FLinearColor::Red, 2.f, 2.f);
	}
#endif

	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Melee: DamageEffectClass is not set, skipping damage"));
		return;
	}

	UAbilitySystemComponent* AttackerASC = Attacker->GetAbilitySystemComponent();
	if (!AttackerASC)
	{
		return;
	}

	const bool bAttackerIsPlayer = Attacker->IsPlayerControlled();

	for (AActor* HitActor : OutActors)
	{
		if (!HitActor || HitActor == Attacker)
		{
			continue;
		}

		AGASCharacterBase* HitCharacter = Cast<AGASCharacterBase>(HitActor);
		if (!HitCharacter)
		{
			continue;
		}

		const bool bTargetIsPlayer = HitCharacter->IsPlayerControlled();
		if (bAttackerIsPlayer == bTargetIsPlayer)
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitCharacter);
		if (!TargetASC)
		{
			UE_LOG(LogTemp, Warning, TEXT("GA_Melee: %s has no ASC, skipping"), *HitCharacter->GetName());
			continue;
		}

		const UDemoAttributes* TargetAttributesBefore = Cast<UDemoAttributes>(TargetASC->GetAttributeSet(UDemoAttributes::StaticClass()));
		if (TargetAttributesBefore)
		{
			UE_LOG(LogTemp, Warning, TEXT("GA_Melee: %s Health BEFORE = %f"), *HitCharacter->GetName(), TargetAttributesBefore->GetHealth());
		}

		FGameplayEffectContextHandle ContextHandle = AttackerASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		// 1. Apply Damage Effect
		FGameplayEffectSpecHandle SpecHandle = AttackerASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveHandle = AttackerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			UE_LOG(LogTemp, Warning, TEXT("GA_Melee: applied damage to %s, valid handle: %s"),
				*HitCharacter->GetName(), ActiveHandle.IsValid() ? TEXT("true") : TEXT("false"));

			// 2. Apply Block Regen Effect with Diagnostics
			if (!BlockRegenEffectClass)
			{
				UE_LOG(LogTemp, Error, TEXT("GA_Melee: BlockRegenEffectClass is NULL! Assign GE_BlockRegen in your Melee Ability Blueprint details."));
			}
			else
			{
				FGameplayEffectSpecHandle BlockSpecHandle = AttackerASC->MakeOutgoingSpec(BlockRegenEffectClass, GetAbilityLevel(), ContextHandle);
				if (BlockSpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle BlockActiveHandle = AttackerASC->ApplyGameplayEffectSpecToTarget(*BlockSpecHandle.Data.Get(), TargetASC);
					UE_LOG(LogTemp, Warning, TEXT("GA_Melee: BlockRegen applied to %s. Success: %s"),
						*HitCharacter->GetName(), BlockActiveHandle.IsValid() ? TEXT("TRUE") : TEXT("FALSE"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("GA_Melee: MakeOutgoingSpec FAILED for BlockRegenEffectClass!"));
				}
			}

			const UDemoAttributes* TargetAttributesAfter = Cast<UDemoAttributes>(TargetASC->GetAttributeSet(UDemoAttributes::StaticClass()));
			if (TargetAttributesAfter)
			{
				UE_LOG(LogTemp, Warning, TEXT("GA_Melee: %s Health AFTER = %f"), *HitCharacter->GetName(), TargetAttributesAfter->GetHealth());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GA_Melee: MakeOutgoingSpec failed for %s"), *HitCharacter->GetName());
		}
	}
}

void UGA_Melee::FinishMeleeAbility()
{
	const FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	AActor* AvatarActor = Info ? Info->AvatarActor.Get() : nullptr;

	if (AGASCharacterBase* Attacker = Cast<AGASCharacterBase>(AvatarActor))
	{
		Attacker->BroadcastMeleeEnded();

		if (UAbilitySystemComponent* ASC = Attacker->GetAbilitySystemComponent())
		{
			ASC->CurrentMontageStop(0.25f);
		}
	}

	if (Info)
	{
		EndAbility(Handle, Info, GetCurrentActivationInfo(), true, false);
	}
}