#include "GASCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "DemoAttributes.h"

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
	}
}