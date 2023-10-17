// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BaDumTss : ModuleRules
{
	public BaDumTss(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"StateTreeModule",
			"StructUtils",
			"EngineSettings", "EnhancedInput"
		});
		
		
		// Gameplay Ability System
		PublicDependencyModuleNames.AddRange(new string[] {"GameplayAbilities", "GameplayTags", "GameplayTasks"});

		PrivateDependencyModuleNames.AddRange(new string[] {
			// AI/MassAI Plugin Modules
			"MassAIBehavior",
			"MassAIDebug",
			"NavigationSystem",
			"AIModule",

			// Runtime/MassEntity Plugin Modules
			"MassEntity",

			// Runtime/MassGameplay Plugin Modules
			"MassActors",
			"MassCommon",
			"MassGameplayDebug",
			"MassLOD",
			"MassMovement",
			"MassNavigation",
			"MassRepresentation",
			"MassReplication",
			"MassSpawner",
			"MassSimulation",
			"MassSignals",
		});
		
		
		// Other
		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "Niagara",});
	}
}
