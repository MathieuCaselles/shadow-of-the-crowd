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
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* CameraOriginLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SMC_Cursor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* SM_Cursor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UMaterial* CursorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	float cursorDistanceFromBBEG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	double LerpValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hero, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABDTHeroCharacter> Hero;

	

public:
	ABDTBBEGPawn();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(const float DeltaTime) override;

	void Move(const FInputActionInstance& Instance);

};
