// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputMappingContext.h"
#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "BDTPawnBase.generated.h"

class ABDTPawnBase;
DEFINE_LOG_CATEGORY_STATIC(LogEnhancedInputPawn, Log, All);

UCLASS(config=Game)
class BADUMTSS_API ABDTPawnBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	/** Ability System Component. Required to use Gameplay Attributes and Gameplay Abilities. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UBDTAbilitySystemComponent* AbilitySystemComponent;
	
	// Sets default values for this pawn's properties
	ABDTPawnBase();

	// Called mapping contexts to the player with EnhancedInputSystem
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual FInputActionValue GetInputActionValue(TObjectPtr<const UInputAction> ForAction);

	/** Ability System Interface getter. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Function to bind to AbilitySystemComponent->OnAbilityEnded. Calls OnGameplayAbilityEnded.
	void OnGameplayAbilityEndedInternal(const FAbilityEndedData& AbilityEndData);

	//Blueprint-implementable OnGameplayAbilityEnded.
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities")
	void OnGameplayAbilityEnded(bool bWasCancelled, FGameplayAbilitySpecHandle abilitySpecHandle, UGameplayAbility* AbilityThatEnded, bool bReplicateEndAbility);
	
	/** Grants an ability at the given level, with an input code used to pick and choose which ability should be triggered. */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode);
	
	/** Activates an ability with a matching input code */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivateAbility(int32 InputCode);

	/** Cancels abilities with specific Gameplay Tags applied to them.*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags);

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "BDTCharacter")
	virtual int32 GetAbilityLevel(int32 InputCode) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemovePawnAbilities();

	/**
	* Getters for attributes from BDTPrimaryAttributeSet
	**/
	UFUNCTION(BlueprintPure, Category = "BDTPawn|PawnAttributes")
	int32 GetEnergy() const;

	UFUNCTION(BlueprintPure, Category = "BDTPawn|PawnAttributes")
	int32 GetMaxEnergy() const;
	
	UFUNCTION(BlueprintPure, Category = "BDTPawn|PawnAttributes")
	int32 GetEnergyRegenRate() const;

	UFUNCTION(BlueprintPure, Category = "BDTPawn|PawnAttributes")
	int32 GetGoldus() const;
	
	UFUNCTION(BlueprintPure, Category = "BDTPawn|PawnAttributes")
	int32 GetCursorMoveSpeed() const;

	UFUNCTION(BlueprintPure, Category = "BDTPawn|PawnAttributes")
	int32 GetSpawnRate() const;
	
protected:
	/**********************************
 *         class overrides
 **********************************/
	
	/** Pawn Attribute Set. UPROPERTY macro required for reflection. */
	UPROPERTY()
	const class UBDTPawnAttributeSet* PawnAttributeSet;
	
	// Default abilities for this Pawn. These will be removed on Pawn death and re-given if Pawn respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UBDTGameplayAbility>> PawnAbilities;
	
	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
	
	virtual void AddStartupEffects();
};
