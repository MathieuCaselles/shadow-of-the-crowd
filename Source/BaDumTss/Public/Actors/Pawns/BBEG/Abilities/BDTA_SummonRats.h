// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BDTGameplayAbility.h"
#include "BDTA_SummonRats.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTA_SummonRats : public UBDTGameplayAbility
{
	GENERATED_BODY()

public:
	UBDTA_SummonRats();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
protected:
	
	void SpawnRats() const;


	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	UPROPERTY()
	class ABDTGameState* GameStatePtr;
};
