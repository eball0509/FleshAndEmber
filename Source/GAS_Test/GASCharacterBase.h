#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GASCharacterBase.generated.h"

class UAbilitySystemComponent;
class UDemoAttributes;

UCLASS()
class GAS_TEST_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UDemoAttributes> AttributeSet;
};