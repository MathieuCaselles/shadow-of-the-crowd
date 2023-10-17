// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "BDTSwordSwipe.generated.h"

UCLASS()
class BADUMTSS_API ABDTSwordSwipe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABDTSwordSwipe();
	
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY()
	class ABDTHeroCharacter* CharacterPointer;

	UPROPERTY(BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="SwordSwipe|Scene")
	USceneComponent* Pivot; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="SwordSwipe|Box|Collision")
	class UBoxComponent* BoxCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="SwordSwipe|Sound")
	USoundWave* SoundToPlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="SwordSwipe|Effect")
	class UNiagaraSystem* EffectToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,  Category="SwordSwipe")
	float SpeedRotationPivot; 

	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
    void ReplicateEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
