#include "GASCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "DemoAttributes.h"
#include "GameFramework/CharacterMovementComponent.h"

AGASCharacterBase::AGASCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSet = CreateDefaultSubobject<UDemoAttributes>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGASCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AGASCharacterBase::BeginPlay()
{
    Super::BeginPlay();
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        if (AttributeSet)
        {
            AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMovementSpeedAttribute())
                .AddUObject(this, &AGASCharacterBase::OnMovementSpeedChanged);

            // Sync initial value immediately, don't wait for the first change
            GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMovementSpeed();
        }
    }
}

void AGASCharacterBase::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
    GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}