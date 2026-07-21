// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS_Test : ModuleRules
{
	public GAS_Test(ReadOnlyTargetRules Target) : base(Target)
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
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"GAS_Test",
			"GAS_Test/Variant_Platforming",
			"GAS_Test/Variant_Platforming/Animation",
			"GAS_Test/Variant_Combat",
			"GAS_Test/Variant_Combat/AI",
			"GAS_Test/Variant_Combat/Animation",
			"GAS_Test/Variant_Combat/Gameplay",
			"GAS_Test/Variant_Combat/Interfaces",
			"GAS_Test/Variant_Combat/UI",
			"GAS_Test/Variant_SideScrolling",
			"GAS_Test/Variant_SideScrolling/AI",
			"GAS_Test/Variant_SideScrolling/Gameplay",
			"GAS_Test/Variant_SideScrolling/Interfaces",
			"GAS_Test/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
