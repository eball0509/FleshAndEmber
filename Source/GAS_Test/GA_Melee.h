#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Melee.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAnimMontage;
class UGameplayEffect;

UCLASS()
class GAS_TEST_API UGA_Melee : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Melee();

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> MeleeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<UGameplayEffect> BlockRegenEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<AActor> FloatingTextClass;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float HitSphereRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float HitSphereForwardOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee|Debug")
	bool bDrawDebugSphere = false;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

	void ResolveHit();

	void FinishMeleeAbility();
};