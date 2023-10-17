// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "MassGameplayDebugTypes.h"
#include "UM_VisualisationTrait.generated.h"

/**
 * 
 */

USTRUCT()
struct FVisualData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Mass | Visualisation")
	TObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mass | Visualisation")
	TObjectPtr<UMaterialInterface> MaterialOverride = nullptr;

	/*
	// Near cull distance to override default value for that agent type 
	UPROPERTY(EditAnywhere, Category = "Mass|Debug")
	uint32 VisualNearCullDistance = 5000;

	// Far cull distance to override default value for that agent type 
	UPROPERTY(EditAnywhere, Category = "Mass|Debug")
	uint32 VisualFarCullDistance = 7500;

	// If Mesh is not set this WireShape will be used for debug drawing via GameplayDebugger 
	UPROPERTY(EditAnywhere, Category = "Mass|Debug")
	EMassEntityDebugShape WireShape = EMassEntityDebugShape::Box;
	*/
};

UCLASS()
class BADUMTSS_API UUM_VisualisationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere, Category = "Mass|Visualization")
	FAgentDebugVisualization Visualization;

	//UPROPERTY(EditAnywhere, Category= "Mass | Visualisation")
	//FVisualData Visualization;
};

USTRUCT()
struct FUM_VisualTag : public FMassTag
{
	GENERATED_BODY()
};