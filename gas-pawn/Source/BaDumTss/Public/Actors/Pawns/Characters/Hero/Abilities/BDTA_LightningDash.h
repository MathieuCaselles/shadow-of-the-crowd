// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BDTGameplayAbility.h"
#include "BDTA_LightningDash.generated.h"

class ABDTHeroCharacter;
/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTA_LightningDash : public UBDTGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBDTA_LightningDash();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Required")
	UAnimMontage* DashMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Required")
	TSubclassOf<class ABDTDash> Dash;


	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	FTimerHandle TimerHandle_Cooldown;

	UFUNCTION()
	void LightningDash(ABDTHeroCharacter* Hero);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
