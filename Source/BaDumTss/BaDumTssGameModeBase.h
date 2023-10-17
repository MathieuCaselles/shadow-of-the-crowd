// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaDumTssGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API ABaDumTssGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaDumTssGameModeBase();

	void HeroDied(AController* Controller);

	UFUNCTION(BlueprintImplementableEvent, Category="Abilities|Attributes")
	void OnHeroDied(AController* Controller);

protected:
	
	virtual void BeginPlay() override;

	void RespawnHero(AController* Controller);
};
