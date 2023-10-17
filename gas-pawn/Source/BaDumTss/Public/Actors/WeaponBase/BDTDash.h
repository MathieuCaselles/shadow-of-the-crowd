// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Actor.h"
#include "BDTDash.generated.h"

UCLASS()
class BADUMTSS_API ABDTDash : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABDTDash();

	UPROPERTY()
	class ABDTHeroCharacter* CharacterPointer;

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
	float DelayToDestroyDashEffectAfterAbilityEnds;
	
	
	UPROPERTY()
	class UBDTCharacterMovementComponent* MovementComponent;
	
	FVector Start;

	FVector2D MoveDirection;

	


	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FVector GetCharacterFeetPosition() const;
	FVector GetGroundPosition() const;

	void SetCharacterFeetPosition(const FVector& NewPosition) const;

	FTimerHandle TimerHandle_RepeatSummon;
	
private:
	FTimerHandle TimerHandleToDestroyEffect;
};
