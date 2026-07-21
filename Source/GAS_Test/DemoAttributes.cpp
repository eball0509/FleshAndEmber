#include "DemoAttributes.h"
#include "GameplayEffectExtension.h"

UDemoAttributes::UDemoAttributes()
{
	InitMovementSpeed(600.0f);
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
}

void UDemoAttributes::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMovementSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UDemoAttributes::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
}