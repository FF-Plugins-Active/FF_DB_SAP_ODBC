// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class FF_DB_SAPODBC : ModuleRules
{
	public FF_DB_SAPODBC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        if (UnrealTargetPlatform.Win64 == Target.Platform)
        {
			// SAP ODBC

            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty", "sap_odbc", "Win64", "include"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "ThirdParty", "sap_odbc", "Win64", "lib", "odbccpp.lib"));
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "ThirdParty", "sap_odbc", "Win64", "lib", "odbccpp.dll"));
            PublicDelayLoadDLLs.Add("odbccpp.dll");
        }

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
				 // ... add other public dependencies that you statically link with here ...
			}
			);

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
