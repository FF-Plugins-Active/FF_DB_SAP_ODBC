#pragma once

#include "CoreMinimal.h"

THIRD_PARTY_INCLUDES_START
#include "odbc/Connection.h"
#include "odbc/Environment.h"
#include "odbc/Exception.h"
#include "odbc/PreparedStatement.h"
#include "odbc/ResultSet.h"
THIRD_PARTY_INCLUDES_END

#include "SAP_ODBC_Connection.generated.h"

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Connection : public UObject
{
	GENERATED_BODY()

public:

	odbc::ConnectionRef ConnectionRef = nullptr;

	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Start(FString& Out_Code, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit);

	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Stop(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Delete(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool PrepareStatement(FString& Out_Code, USAP_ODBC_Statement*& Out_Statement, FString SQL_Statement);

	UFUNCTION(BlueprintCallable)
	virtual bool CommitStatement(FString& Out_Code);

};

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Statement : public UObject
{
	GENERATED_BODY()

public:

	TSharedPtr<odbc::PreparedStatementRef> StatementPtr;

	UFUNCTION(BlueprintCallable)
	virtual void SetInt(int32 Value, int32 ParamIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual void SetString(FString Value, int32 ParamIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual void AddBatch();

	UFUNCTION(BlueprintCallable)
	virtual void ExecuteBatch(FString& Out_Code);

};

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Result : public UObject
{
	GENERATED_BODY()

public:

	odbc::ResultSetRef QueryResult = nullptr;

	UFUNCTION(BlueprintCallable)
	virtual bool GetString(FString& Out_Code, FString& Out_String, int32 ColumnIndex);

};