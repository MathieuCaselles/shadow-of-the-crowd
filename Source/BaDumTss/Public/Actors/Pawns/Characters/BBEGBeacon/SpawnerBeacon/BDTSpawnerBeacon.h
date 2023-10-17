// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Pawns/Characters/BBEGBeacon/BDTBeaconBase.h"
#include "BDTSpawnerBeacon.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API ABDTSpawnerBeacon : public ABDTBeaconBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	

	UPROPERTY()
	float IndexGameStateSpawner;

protected:
	UPROPERTY()
	class ABDTGameState* GameStatePtr;
};
