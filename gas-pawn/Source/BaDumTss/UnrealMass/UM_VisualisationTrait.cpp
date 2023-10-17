// Fill out your copyright notice in the Description page of Project Settings.


#include "UM_VisualisationTrait.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
//
#include "MassDebuggerSubsystem.h"
#include "MassDebugVisualizationComponent.h"

void UUM_VisualisationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	FSimDebugVisFragment& DebugVisFragment = BuildContext.AddFragment_GetRef<FSimDebugVisFragment>();
	
	UMassDebuggerSubsystem* Debugger = World.GetSubsystem<UMassDebuggerSubsystem>();
	/*if (ensure(Debugger))
	{
		UMassDebugVisualizationComponent* DebugVisComponent = Debugger->GetVisualizationComponent();
		if (ensure(DebugVisComponent))
		{
			DebugVisFragment.VisualType = DebugVisComponent->AddDebugVisType(Visualization);
		}
		// @todo this path requires a fragment destructor that will remove the mesh from the debugger.
	}*/
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	FSimDebugVisFragment& DebugVisFragment = BuildContext.AddFragment_GetRef<FSimDebugVisFragment>();
	
	UMassDebuggerSubsystem* Debugger = World.GetSubsystem<UMassDebuggerSubsystem>();
	if (ensure(Debugger))
	{
		UMassDebugVisualizationComponent* DebugVisComponent = Debugger->GetVisualizationComponent();
		if (ensure(DebugVisComponent))
		{
			//DebugVisFragment.VisualType = DebugVisComponent->AddDebugVisType(Visualization);
		}
		// @todo this path requires a fragment destructor that will remove the mesh from the debugger.
	}
	*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	BuildContext.AddTag<FMassDebuggableTag>();
	
	BuildContext.AddFragment<FAgentRadiusFragment>();
	BuildContext.AddFragment<FTransformFragment>();

	/*
	#if WITH_EDITORONLY_DATA
	const UStaticMesh* const DebugMesh = Visualization.Mesh;
#else
	const UStaticMesh* const DebugMesh = nullptr;
#endif
	
	if (DebugMesh)
	{
#if WITH_EDITORONLY_DATA
		FSimDebugVisFragment& DebugVisFragment = BuildContext.AddFragment_GetRef<FSimDebugVisFragment>();
		UMassDebuggerSubsystem* Debugger = World.GetSubsystem<UMassDebuggerSubsystem>();
		if (ensure(Debugger))
		{
			UMassDebugVisualizationComponent* DebugVisComponent = Debugger->GetVisualizationComponent();
			if (ensure(DebugVisComponent))
			{
				DebugVisFragment.VisualType = DebugVisComponent->AddDebugVisType(Visualization);
			}
			// @todo this path requires a fragment destructor that will remove the mesh from the debugger.
		}
#endif // WITH_EDITORONLY_DATA
	}
	// add fragments needed whenever we have debugging capabilities
	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	BuildContext.AddTag<FMassDebuggableTag>();
#if WITH_EDITORONLY_DATA
	BuildContext.AddFragment_GetRef<FDataFragment_DebugVis>().Shape = Visualization.WireShape;
#else
	// DebugShape unavailable, will used default instead
	BuildContext.AddFragment<FDataFragment_DebugVis>();
#endif // WITH_EDITORONLY_DATA
	BuildContext.AddFragment<FAgentRadiusFragment>();

	BuildContext.AddFragment<FTransformFragment>();
#endif // if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	*/
}