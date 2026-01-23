// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenWorldFramework : ModuleRules
{
	public OpenWorldFramework(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"OpenWorldFramework",
			"OpenWorldFramework/Variant_Platforming",
			"OpenWorldFramework/Variant_Platforming/Animation",
			"OpenWorldFramework/Variant_Combat",
			"OpenWorldFramework/Variant_Combat/AI",
			"OpenWorldFramework/Variant_Combat/Animation",
			"OpenWorldFramework/Variant_Combat/Gameplay",
			"OpenWorldFramework/Variant_Combat/Interfaces",
			"OpenWorldFramework/Variant_Combat/UI",
			"OpenWorldFramework/Variant_SideScrolling",
			"OpenWorldFramework/Variant_SideScrolling/AI",
			"OpenWorldFramework/Variant_SideScrolling/Gameplay",
			"OpenWorldFramework/Variant_SideScrolling/Interfaces",
			"OpenWorldFramework/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
