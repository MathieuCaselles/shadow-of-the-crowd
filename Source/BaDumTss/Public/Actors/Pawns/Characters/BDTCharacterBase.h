// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Abilities/Components/BDTAbilitySystemComponent.h"
#include "BDTCharacterBase.generated.h"

class ABDTCharacterBase;
DEFINE_LOG_CATEGORY_STATIC(LogEnhancedInputCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ABDTCharacterBase*, Character);


//Implements IAbilitySystemInterface so that the ability system component can be accessed without needing to know the specific class.
UCLASS(config=Game)
class BADUMTSS_API ABDTCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;
	
	
	/** Ability System Component. Required to use Gameplay Attributes and Gameplay Abilities. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UBDTAbilitySystemComponent* AbilitySystemComponent;
	

	UPROPERTY(BlueprintAssignable, Category = "BDTCharacter")
	FCharacterDiedDelegate OnCharacterDied;
	
	// Sets default values for this character's properties
	ABDTCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Called mapping contexts to the player with EnhancedInputSystem
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual FInputActionValue GetInputActionValue(TObjectPtr<const UInputAction> ForAction);

	/** Ability System Interface getter. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

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


	
	UFUNCTION(BlueprintCallable, Category = "BDTCharacter")
	virtual bool IsAlive() const;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "BDTCharacter")
	virtual int32 GetAbilityLevel(int32 InputCode) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemoveCharacterAbilities();

	/**
	* Getters for attributes from BDTPrimaryAttributeSet
	**/
	UFUNCTION(BlueprintPure, Category = "BDTCharacter|PrimaryAttributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintPure, Category = "BDTCharacter|PrimaryAttributes")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "BDTCharacter|PrimaryAttributes")
	float GetMaxHealth() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintPure, Category = "BDTCharacter|PrimaryAttributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "BDTCharacter")
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "BDTCharacter")
	virtual void FinishDying();

	//Blueprint-implementable On Health Changed event. Used to send information to the UI about current health.
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities|Attributes")
	void OnHealthChanged(float oldValue, float newValue);


	
protected:
	/**********************************
 *         class overrides
 **********************************/
	
	/** Character Attribute Set. UPROPERTY macro required for reflection. */
	UPROPERTY()
	const class UBDTPrimaryAttributeSet* PrimaryAttributeSet;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;
	FGameplayTag InvulnerableToDamageTag;


	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	UAnimMontage* DeathMontage;

	// Default abilities for this Character. These will be removed on Character death and re-given if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UBDTGameplayAbility>> CharacterAbilities;


	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;


	
	virtual void AddStartupEffects();

private:
	//Functions to bind to Attribute Changed delegate. Calls On Health Changed.
	virtual void OnHealthChangedInternal(const FOnAttributeChangeData& Data);
};