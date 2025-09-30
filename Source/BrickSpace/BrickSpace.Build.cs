// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BrickSpace : ModuleRules
{
	public BrickSpace(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
           {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Json",
				"JsonUtilities",
                "AndroidPermission",
                "Sockets",
                "UMG",
				"Niagara",
				"OculusXRScene",
                "OculusXRHMD",
                "OculusXRAnchors",
                "OculusXRColocation",
                "OculusXRInput"
           });


        PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Slate",
                "SlateCore",
                "OnlineSubsystem"
            });

        if (Target.Platform != UnrealTargetPlatform.Android)
        {
            PrecompileForTargets = PrecompileTargetsType.None;
        }

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
