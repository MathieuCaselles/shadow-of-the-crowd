// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassProcessor.h"
#include "NavigationPath.h"
#include "UM_NavMeshMovementTrait.generated.h"

class UMassEntitySubsystem;

USTRUCT()
struct BADUMTSS_API FNavMeshPathFragment : public FMassFragment
{
	GENERATED_BODY()
	
	UPROPERTY()
	int8 PathIndex = 0;

	UPROPERTY(EditAnywhere)
	FVector TargetLocation;
	
	FPathFindingResult PathResult;

	float TimeBeforeRecalculatePath;
	
	void SetTargetLocation(const FVector& Target)
	{
		TargetLocation = Target;
		PathIndex = 0;
	}
};

USTRUCT()
struct BADUMTSS_API FNavTimeSharedFragment : public FMassSharedFragment
{
	GENERATED_BODY()
	float Time;
};

UCLASS()
class BADUMTSS_API UUM_NavMeshMovementTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

UCLASS()
class BADUMTSS_API UUM_NavMeshMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()

	UUM_NavMeshMovementProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntitySubsystem, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};

USTRUCT()
struct BADUMTSS_API FNavAgent : public FMassTag
{
	GENERATED_BODY();
};
