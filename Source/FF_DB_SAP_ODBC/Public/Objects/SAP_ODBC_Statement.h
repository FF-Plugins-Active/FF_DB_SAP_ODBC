#pragma once

#include "CoreMinimal.h"

#include "SAP_ODBC_Includes.h"
#include "SAP_ODBC_Result.h"

#include "SAP_ODBC_Statement.generated.h"

UCLASS(BlueprintType)
class FF_DB_SAP_ODBC_API USAP_ODBC_Statement : public UObject
{
	GENERATED_BODY()

public:

	odbc::ConnectionRef Connection;
	odbc::PreparedStatementRef Statement;

	UFUNCTION(BlueprintCallable)
	virtual void SetInt(int32 Value, int32 ParamIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual void SetString(FString Value, int32 ParamIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual void AddBatch();

	UFUNCTION(BlueprintCallable)
	virtual bool CommitStatement(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool ExecuteBatch(FString& Out_Code);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "This will return result."))
	virtual bool ExecuteQuery(FString& Out_Code, USAP_ODBC_Result*& Out_Result, bool bRecordResult);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "This will NOT return a result because it is just for update, insert and delete. It will return affected rows count."))
	virtual bool ExecuteUpdate(FString& Out_Code, int32& Affected_Rows);

};