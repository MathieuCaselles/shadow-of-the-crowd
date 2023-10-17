// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassMovementFragments.h"
#include "MassProcessor.h"
#include "UM_AdvancedRandomMovementTrait.generated.h"

USTRUCT()
struct BADUMTSS_API FNPC : public FMassTag
{
	GENERATED_BODY();
};

UCLASS()
class BADUMTSS_API UM_AdvancedRandomMovementTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

UCLASS()
class BADUMTSS_API UUM_AdvancedRandomMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()

	UUM_AdvancedRandomMovementProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntitySubsystem, FMassExecutionContext& Context) override;
	
private:
	FMassEntityQuery EntityQuery;
};