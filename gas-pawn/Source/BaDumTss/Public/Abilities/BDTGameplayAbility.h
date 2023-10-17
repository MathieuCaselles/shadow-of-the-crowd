// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaDumTss/BaDumTss.h"
#include "BDTGameplayAbility.generated.h"


DEFINE_LOG_CATEGORY_STATIC(LogGameplayAbility, Log, All);


/**
 * 
 */
UCLASS()
class BADUMTSS_API UBDTGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBDTGameplayAbility();
	
	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
