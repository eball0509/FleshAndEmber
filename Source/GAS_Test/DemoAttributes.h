#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DemoAttributes.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GAS_TEST_API UDemoAttributes : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDemoAttributes();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UDemoAttributes, MovementSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDemoAttributes, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDemoAttributes, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDemoAttributes, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UDemoAttributes, MaxStamina);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMovementSpeedValue() const { return GetMovementSpeed(); }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthValue() const { return GetHealth(); }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealthValue() const { return GetMaxHealth(); }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetStaminaValue() const { return GetStamina(); }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxStaminaValue() const { return GetMaxStamina(); }
};