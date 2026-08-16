#include "GASCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "DemoAttributes.h"
#include "GameplayEffect.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

AGASCharacterBase::AGASCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UDemoAttributes>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGASCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Reset input mode to Game Only and hide mouse cursor on spawn
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}

	// Initialize Ability System Component actor info
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// Listen for changes to the Health attribute
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UDemoAttributes::GetHealthAttribute()
		).AddUObject(this, &AGASCharacterBase::OnHealthAttributeChanged);
	}

	// Automatically apply passive regen on server start
	if (HasAuthority())
	{
		ApplyPassiveRegen();
	}
}

void AGASCharacterBase::ApplyPassiveRegen()
{
	if (!AbilitySystemComponent || !RegenEffectClass)
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(RegenEffectClass, 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AGASCharacterBase::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f && !bIsDead)
	{
		Die();
	}
}

void AGASCharacterBase::Die()
{
	bIsDead = true;

	// If this character is controlled by AI (an enemy), handle enemy death cleanup & wave notification
	if (Cast<APlayerController>(Controller) == nullptr)
	{
		// Notify the Wave Manager that this enemy was killed
		if (UWorld* World = GetWorld())
		{
			// Find the Wave Manager actor in the level
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (Actor && Actor->GetClass()->GetName().Contains(TEXT("BP_WaveManager")))
				{
					// Use a function call or reflection, or even better: 
					// Call a Blueprint Interface or use UFunction to trigger NotifyEnemyKilled
					UFunction* Func = Actor->FindFunction(FName("NotifyEnemyKilled"));
					if (Func)
					{
						Actor->ProcessEvent(Func, nullptr);
					}
					break;
				}
			}
		}

		// Disable capsule collision so the player doesn't trip over the corpse
		if (GetCapsuleComponent())
		{
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Stop movement
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->DisableMovement();
		}

		// Destroy the enemy actor after a short delay (e.g., 3 seconds)
		SetLifeSpan(3.0f);
		return;
	}

	// --- Existing Player Death Logic ---
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	if (IsLocallyControlled() && DeathScreenWidgetClass)
	{
		UUserWidget* DeathScreen = CreateWidget<UUserWidget>(GetWorld(), DeathScreenWidgetClass);
		if (DeathScreen)
		{
			DeathScreen->AddToViewport();

			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

void AGASCharacterBase::Jump()
{
	Super::Jump();
	bJumpedViaInput = true;
}

void AGASCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bJumpedViaInput && AbilitySystemComponent)
	{
		FGameplayEventData EventData;
		EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Landed"));
		AbilitySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);
	}

	bJumpedViaInput = false;
}