// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class UITest : ModuleRules
{
	public UITest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore"
		});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Slate", "SlateCore", "SlateRHIRenderer", "UMG", "ModelViewViewModel"
		});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}