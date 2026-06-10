// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_GemCoop : ModuleRules
{
	public Project_GemCoop(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"NavigationSystem",
			"GameplayTasks",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Project_GemCoop",
			"Project_GemCoop/Variant_Platforming",
			"Project_GemCoop/Variant_Platforming/Animation",
			"Project_GemCoop/Variant_Combat",
			"Project_GemCoop/Variant_Combat/AI",
			"Project_GemCoop/Variant_Combat/Animation",
			"Project_GemCoop/Variant_Combat/Gameplay",
			"Project_GemCoop/Variant_Combat/Interfaces",
			"Project_GemCoop/Variant_Combat/UI",
			"Project_GemCoop/Variant_SideScrolling",
			"Project_GemCoop/Variant_SideScrolling/AI",
			"Project_GemCoop/Variant_SideScrolling/Gameplay",
			"Project_GemCoop/Variant_SideScrolling/Interfaces",
			"Project_GemCoop/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
