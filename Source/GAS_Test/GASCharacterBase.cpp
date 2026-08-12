#include "GASCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "DemoAttributes.h"
#include "GameplayEffect.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

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