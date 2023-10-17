// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BDTBBEGController.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API ABDTBBEGController : public APlayerController
{
	GENERATED_BODY()

	virtual void AcknowledgePossession(APawn* P) override;
};
