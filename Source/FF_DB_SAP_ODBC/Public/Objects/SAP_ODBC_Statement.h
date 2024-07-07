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

	TSharedPtr<odbc::ConnectionRef> Connection;
	TSharedPtr<odbc::PreparedStatementRef> Statement;

	UFUNCTION(BlueprintCallable)
	virtual void SetInt(int32 Value, int32 ParamIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual void SetString(FString Value, int32 ParamIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual void AddBatch();

	UFUNCTION(BlueprintCallable)
	virtual void ExecuteBatch(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool CommitStatement(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool ExecuteQuery(FString& Out_Code, USAP_ODBC_Result*& Out_Result);

};