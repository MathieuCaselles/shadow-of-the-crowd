// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Pawns/BDTPawnBase.h"
#include "Actors/Pawns/Characters/Hero/BDTHeroCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BDTBBEGPawn.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API ABDTBBEGPawn : public ABDTPawnBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* CameraOriginLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SMC_Cursor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UMaterial* CursorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	float CursorDistanceFromBBEG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	double LerpValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hero, meta = (AllowPrivateAccess = "true"))
	AActor* HeroToken;


public:
	ABDTBBEGPawn();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	FORCEINLINE UStaticMeshComponent* GetCursor() const { return SMC_Cursor; }

	FTransform GetCursorTransform() const;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(const float DeltaTime) override;
	
	void Move(const FInputActionInstance& Instance);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetCursorLocationReplicated(const FVector& Location);

private:
	UPROPERTY()
	class ABDTGameState* GameStatePtr;


	//Functions to bind to Attribute Changed delegate. Calls On Energy Changed.
	virtual void OnEnergyChangedInternal(const FOnAttributeChangeData& Data);

	//Functions to bind to Attribute Changed delegate. Calls On EnergyRegenRate Changed.
	virtual void OnEnergyRegenRateChangedInternal(const FOnAttributeChangeData& Data);

	//Functions to bind to Attribute Changed delegate. Calls On Goldus Changed.
	virtual void OnGoldusChangedInternal(const FOnAttributeChangeData& Data);

public: 
	//Blueprint-implementable On Energy Changed event. Used to send information to the UI about current energy.
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities|Attributes")
	void OnEnergyChanged(float oldValue, float newValue);

	//Blueprint-implementable On EnergyRegenRate Changed event. Used to send information to the UI about current energy regen rate.
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities|Attributes")
	void OnEnergyRegenRateChanged(float oldValue, float newValue);

	//Blueprint-implementable On Goldus Changed event. Used to send information to the UI about current goldus.
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities|Attributes")
	void OnGoldusChanged(float oldValue, float newValue);
};
