#pragma once

#include "CoreMinimal.h"

// Custom Includes.
#include "SAP_ODBC_Includes.h"
#include "Objects/SAP_ODBC_Statement.h"

#include "SAP_ODBC_Connection.generated.h"

UCLASS(BlueprintType)
class FF_DB_SAP_ODBC_API USAP_ODBC_Connection : public UObject
{
	GENERATED_BODY()

private:

	FString ConnectionId;
	odbc::ConnectionRef Connection;

public:

	virtual odbc::ConnectionRef GetConenction();
	virtual bool SetConnection(odbc::ConnectionRef In_Ref, const FString In_Id);

	UFUNCTION(BlueprintPure)
	virtual bool IsConnectionValid();

	UFUNCTION(BlueprintPure)
	virtual FString GetConnectionId();

	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Start(FString& Out_Code, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit);
	
	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Stop(FString& Out_Code);
	
	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Delete(FString& Out_Code);
	
	UFUNCTION(BlueprintCallable)
	virtual bool PrepareStatement(FString& Out_Code, USAP_ODBC_Statement*& Out_Statement, FString SQL_Statement);

};