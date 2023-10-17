// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Pawns/Characters/BDTCharacterBase.h"
#include "BDTHeroCharacter.generated.h"

class UBoxComponent;
class ABDTSwordSwipe;
class ABDTProjectile;
class USpringArmComponent;


/**
 * 
 */
UCLASS()
class BADUMTSS_API ABDTHeroCharacter : public ABDTCharacterBase
{
	friend ABDTSwordSwipe;
	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	
public:
	ABDTHeroCharacter(const FObjectInitializer& ObjectInitializer);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	virtual void FinishDying() override;
	
	void TakeDamageFromRat() const;

	TArray<ABDTProjectile*>& GetArrayProjectile();
	
	UBoxComponent* GetSwordBoxCollision() const;

	UPROPERTY()
	bool bDashDamage = false;

	UPROPERTY()
	bool bSwordDamage = false;
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ABILITY")
	const TSubclassOf<UGameplayEffect> RatDamage;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;
	
	bool bAscInputBound = false;

	FGameplayTag DeadTag;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	void Look(const FInputActionInstance& Instance);

	void Move(const FInputActionInstance& Instance);

private:
	UPROPERTY()
	TArray<ABDTProjectile*> ArrayOfProjectiles;

	UPROPERTY()
	UBoxComponent* SwordBoxCollision;

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	
};
