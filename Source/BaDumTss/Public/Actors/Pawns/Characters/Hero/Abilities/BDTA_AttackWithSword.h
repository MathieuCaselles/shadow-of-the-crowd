// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BDTGameplayAbility.h"
#include "BDTA_AttackWithSword.generated.h"

class ABDTHeroCharacter;

/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTA_AttackWithSword : public UBDTGameplayAbility
{
	GENERATED_BODY()

public:
	UBDTA_AttackWithSword();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Required")
	UAnimMontage* SwipeMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Required")
	TSubclassOf<class ABDTSwordSwipe> SwordSwipe;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void AttackWithSword(ABDTHeroCharacter* Hero);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
