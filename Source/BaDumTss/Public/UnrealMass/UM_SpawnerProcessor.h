// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "UM_SpawnerProcessor.generated.h"

/**
 * 
 */
UCLASS()
class BADUMTSS_API UUM_SpawnerProcessor : public UMassProcessor
{
	GENERATED_BODY()

	UUM_SpawnerProcessor();
	
	virtual void Initialize(UObject& Owner) override;

public:
	void SetSpawnRate(int NewSpawnRate);
	
protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntitySubsystem, FMassExecutionContext& Context) override;
	
private:
	UPROPERTY()
	class UUM_WorldSubsystem* UMSubsystem = nullptr;
	
	UPROPERTY()
	bool bHasInitUniqueID = false;
	
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	class ABDTGameState* GameStatePtr;
	
	float SpawnRate = 10.f;
	float SpawnTime = 0.0f;

	int Clock(const float TmpsSpawnRate);

	float TokenSpawnRate = 5.f;
	float TokenSpawnTime = 0.0f;
	
	int TokenClock(const float TmpsTokenSpawnRate);
};

USTRUCT()
struct BADUMTSS_API FEntityUniqueID : public FMassFragment
{
	GENERATED_BODY();

	int GetUniqueID();
	void SetUniqueID(int newUniqueID);
	
private:
	int UniqueID = 0;
};

USTRUCT()
struct BADUMTSS_API FCanBeInstantiatedTag : public FMassTag
{
	GENERATED_BODY();
};