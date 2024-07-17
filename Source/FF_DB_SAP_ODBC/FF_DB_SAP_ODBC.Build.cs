// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class FF_DB_SAP_ODBC : ModuleRules
{
	public FF_DB_SAP_ODBC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;

        if (UnrealTargetPlatform.Win64 == Target.Platform)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty", "sap_odbc", "Win64", "include"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "ThirdParty", "sap_odbc", "Win64", "lib", "odbccpp.lib"));
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "ThirdParty", "sap_odbc", "Win64", "lib", "odbccpp.dll"));
            PublicDelayLoadDLLs.Add("odbccpp.dll");
        }

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "Json",
                "JsonUtilities",
                "JsonBlueprintUtilities",
			}
			);

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects",	
			}
			);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
			);
	}
}
