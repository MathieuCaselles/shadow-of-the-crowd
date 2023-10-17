// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BDTGameplayAbility.h"
#include "BDTA_SummonBeacon.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTA_SummonBeacon : public UBDTGameplayAbility
{
	GENERATED_BODY()

public:
	UBDTA_SummonBeacon();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Required")
	TSubclassOf<class ABDTBeaconBase> BeaconToSpawn;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SummonBeacon(const FTransform& CursorTransform, TSubclassOf<ABDTBeaconBase> Beacon);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	UPROPERTY()
	class ABDTGameState* GameStatePtr;

	UPROPERTY()
	class ABDTCharacterBase* BBEG;
};
