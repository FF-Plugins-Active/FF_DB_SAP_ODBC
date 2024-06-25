// Copyright Epic Games, Inc. All Rights Reserved.

#include "FF_DB_SAPODBC.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FFF_DB_SAPODBCModule"

#ifdef _WIN64

void FFF_DB_SAPODBCModule::Loader_SAP_ODBC()
{
	const FString DLL_SAP_ODBC = FPaths::Combine(*this->Dir_ThirdParty, TEXT("sap_odbc/Win64/lib/odbccpp.dll"));

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

void FFF_DB_SAPODBCModule::Release_SAP_ODBC()
{
	if (Handle_SAP_ODBC)
	{
		FPlatformProcess::FreeDllHandle(Handle_SAP_ODBC);
		Handle_SAP_ODBC = nullptr;
	}
}

#endif

void FFF_DB_SAPODBCModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
#ifdef _WIN64

	this->Dir_ThirdParty = FPaths::Combine(*IPluginManager::Get().FindPlugin("FF_DB_SAPODBC")->GetBaseDir(), TEXT("Source/FF_DB_SAPODBC/ThirdParty/"));

	this->Loader_SAP_ODBC();

#endif
}

void FFF_DB_SAPODBCModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
#ifdef _WIN64
	this->Release_SAP_ODBC();
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFF_DB_SAPODBCModule, FF_DB_SAPODBC)