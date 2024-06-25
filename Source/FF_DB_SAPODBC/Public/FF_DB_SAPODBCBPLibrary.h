// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

THIRD_PARTY_INCLUDES_START
#ifdef _WIN64
// SAP ODBC.
#include "odbc/Connection.h"
#include "odbc/Environment.h"
#include "odbc/Exception.h"
#include "odbc/PreparedStatement.h"
#include "odbc/ResultSet.h"
#endif
THIRD_PARTY_INCLUDES_END

#include "FF_DB_SAPODBCBPLibrary.generated.h"

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Environment : public UObject
{
	GENERATED_BODY()

public:

	odbc::EnvironmentRef SAP_ODBC_Environment;

};

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Connection : public UObject
{
	GENERATED_BODY()

public:

	odbc::ConnectionRef SAP_ODBC_Connection;

};

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Statement_Prepared : public UObject
{
	GENERATED_BODY()

public:

	odbc::PreparedStatementRef SAP_ODBC_Statement;

};

UCLASS()
class UFF_DB_SAPODBCBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SAP ODBC - Init Environment", Keywords = "odbc, init, environment"), Category = "Frozen Forest|Database|SAP ODBC")
	static bool SAP_ODBC_Init_Environment(USAP_ODBC_Environment*& Out_Environment);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SAP ODBC - Create Connection", Keywords = "odbc, connection, create"), Category = "Frozen Forest|Database|SAP ODBC")
	static bool SAP_ODBC_Connection_Create(FString& Out_Code, UPARAM(ref)USAP_ODBC_Environment*& In_Environment, USAP_ODBC_Connection*& Out_Connection);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SAP ODBC - Delete Connection", Keywords = "odbc, connection, delete"), Category = "Frozen Forest|Database|SAP ODBC")
	static bool SAP_ODBC_Connection_Delete(FString& Out_Code, UPARAM(ref)USAP_ODBC_Connection*& In_Connection);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SAP ODBC - Start Connection", Keywords = "odbc, connection, start"), Category = "Frozen Forest|Database|SAP ODBC")
	static bool SAP_ODBC_Connection_Start(FString& Out_Code, UPARAM(ref)USAP_ODBC_Connection*& In_Connection, FString Driver, FString Username, FString Password, bool bUseAutoCommit);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SAP ODBC - Close Connection", Keywords = "odbc, connection, stop"), Category = "Frozen Forest|Database|SAP ODBC")
	static bool SAP_ODBC_Connection_Stop(FString& Out_Code, UPARAM(ref)USAP_ODBC_Connection*& In_Connection);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SAP ODBC - Close Connection", Keywords = "odbc, connection, stop"), Category = "Frozen Forest|Database|SAP ODBC")
	static bool SAP_ODBC_Statement_Prepare(USAP_ODBC_Statement_Prepared*& Out_Statement, FString& Out_Code, USAP_ODBC_Connection* In_Connection, FString SQL_Statement);

};
