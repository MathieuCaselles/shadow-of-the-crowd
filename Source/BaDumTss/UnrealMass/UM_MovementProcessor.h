// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "UM_MovementProcessor.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API UUM_MovementProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UUM_MovementProcessor();

	virtual void Initialize(UObject& Owner) override;

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	
private:
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	class ABDTHeroCharacter* HeroPtr;

	UPROPERTY()
	class URuntimeVirtualTexture* RVTMaskSwarm;
};
