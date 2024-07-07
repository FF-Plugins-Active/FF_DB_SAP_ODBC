// Copyright Epic Games, Inc. All Rights Reserved.

#include "FF_DB_SAP_ODBC.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FFF_DB_SAP_ODBCModule"

void FFF_DB_SAP_ODBCModule::StartupModule()
{
	const FString DLL_SAP_ODBC = FPaths::Combine(*IPluginManager::Get().FindPlugin("FF_DB_SAP_ODBC")->GetBaseDir(), TEXT("Source/FF_DB_SAP_ODBC/ThirdParty/sap_odbc/Win64/lib/odbccpp.dll"));

	if (!FPaths::FileExists(DLL_SAP_ODBC))
	{
		UE_LOG(LogTemp, Fatal, TEXT("odbccpp.dll couldn't be found."));
	}

	Handle_SAP_ODBC = FPlatformProcess::GetDllHandle(*DLL_SAP_ODBC);

	if (Handle_SAP_ODBC != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("odbccpp.dll loaded successfully!"));
	}

	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("odbccpp.dll failed to load!"));
	}
}

void FFF_DB_SAP_ODBCModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	if (Handle_SAP_ODBC)
	{
		FPlatformProcess::FreeDllHandle(Handle_SAP_ODBC);
		Handle_SAP_ODBC = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFF_DB_SAP_ODBCModule, FF_DB_SAP_ODBC)