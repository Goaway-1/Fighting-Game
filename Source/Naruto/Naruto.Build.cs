// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Naruto : ModuleRules
{
	public Naruto(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "PhysicsCore", "NavigationSystem", "AIModule", "GameplayTasks", "MediaAssets", "UMG" });
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
    }
}
