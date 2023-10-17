// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BDTGameplayAbility.h"
#include "BDTA_LightningDash.generated.h"

class ABDTCharacterBase;
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

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	FTimerHandle TimerHandle_TickDash;

	UPROPERTY()
	ABDTCharacterBase* Hero;

	UPROPERTY()
	class UBDTCharacterMovementComponent* MovementComponent;
	
	FVector Start;

	FVector2D MoveDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dash|Sound")
	USoundWave* TrailSoundToPlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dash|Sound")
	USoundWave* ZoneSoundToPlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dash|Sound")
	USoundWave* DelayedSoundToPlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dash|Effect")
	class UNiagaraSystem* TrailEffectToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dash|Effect")
	class UNiagaraSystem* ZoneEffectToSpawn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dash|Effect")
	TSubclassOf<class AActor> ZoneDecalsToSpawn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Dash|Variables")
	float DashSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Dash|Variables")
	float DashTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Dash|Variables")
	float DashTick;


	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="Dash|Variables")
	float DelayToDestroyDashEffectAfterAbilityEnds;
	


	UFUNCTION()
	void InitDash();

	UFUNCTION(Server, Unreliable, WithValidation)
	void DashingTick();

		UFUNCTION()
	void LightningDash();
	
	UFUNCTION()
	void EndDash();

	FVector GetCharacterFeetPosition() const;
	FVector GetGroundPosition() const;

	void SetCharacterFeetPosition(const FVector& NewPosition) const;

	FTimerHandle TimerHandle_RepeatSummon;
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	FTimerHandle TimerHandleToDestroyEffect;
};
