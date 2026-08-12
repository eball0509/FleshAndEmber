#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GASCharacterBase.generated.h"

class UAbilitySystemComponent;
class UDemoAttributes;
class UGameplayEffect;
class UUserWidget;

UCLASS()
class GAS_TEST_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	FORCEINLINE UDemoAttributes* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Regen")
	TSubclassOf<UGameplayEffect> RegenEffectClass;

	UFUNCTION(BlueprintCallable, Category = "GAS|Regen")
	void ApplyPassiveRegen();

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS|Melee")
	void BroadcastMeleeEnded();

	UPROPERTY(EditDefaultsOnly, Category = "GAS|UI")
	TSubclassOf<UUserWidget> DeathScreenWidgetClass;

	virtual void Die();
	bool bIsDead = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UDemoAttributes> AttributeSet;

private:
	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
};