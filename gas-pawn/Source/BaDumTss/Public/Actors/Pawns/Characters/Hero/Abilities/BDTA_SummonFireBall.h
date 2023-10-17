// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BDTGameplayAbility.h"
#include "Actors/WeaponBase/BDTProjectile.h"
#include "BDTA_SummonFireBall.generated.h"


class ABDTHeroCharacter;


/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTA_SummonFireBall : public UBDTGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBDTA_SummonFireBall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEnhancedInputComponent* EnhancedInputComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireHipMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ABDTProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	
	UFUNCTION()
	void SpawnFireBall(ABDTHeroCharacter* Hero);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	
};
