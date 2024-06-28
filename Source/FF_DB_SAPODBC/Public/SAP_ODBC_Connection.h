#pragma once

#include "CoreMinimal.h"

// UE Includes.
#include "JsonObjectWrapper.h"
#include "JsonUtilities.h"

// Custom Includes.
#include "SAP_ODBC_Includes.h"

#include "SAP_ODBC_Connection.generated.h"

USTRUCT(BlueprintType)
struct FF_DB_SAPODBC_API FSAP_ODBC_MetaData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	int32 ColumnScale = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ColumnPrecision = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ColumnDisplaySize = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ColumnLenght = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ColumnOctetLenght = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ColumnType = 0;

	UPROPERTY(BlueprintReadOnly)
	FString ColumnTypeName;

	UPROPERTY(BlueprintReadOnly)
	FString BaseColumnName;

	UPROPERTY(BlueprintReadOnly)
	FString ColumnName;

	UPROPERTY(BlueprintReadOnly)
	FString ColumnLabel;

	UPROPERTY(BlueprintReadOnly)
	FString BaseTableName;

	UPROPERTY(BlueprintReadOnly)
	FString TableName;

	UPROPERTY(BlueprintReadOnly)
	FString CatalogName;

	UPROPERTY(BlueprintReadOnly)
	FString SchemaName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsNullable = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAutoIncrement = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsNamed = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCaseSensitive = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsReadOnly = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSearchable = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSigned = false;
};


UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Connection : public UObject
{
	GENERATED_BODY()

public:

	odbc::ConnectionRef ConnectionRef = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FString ConnectionId;

	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Start(FString& Out_Code, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit);

	UFUNCTION(BlueprintCallable)
	virtual bool Connection_Stop(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool PrepareStatement(FString& Out_Code, USAP_ODBC_Statement*& Out_Statement, FString SQL_Statement);

};

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Statement : public UObject
{
	GENERATED_BODY()

public:

	TSharedPtr<odbc::ConnectionRef> ConnectionPtr;
	TSharedPtr<odbc::PreparedStatementRef> StatementPtr;

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

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Result : public UObject
{
	GENERATED_BODY()

protected:

	TSharedPtr<odbc::ResultSetMetaDataRef> MetaDataPtr;
	TSharedPtr<odbc::ResultSetRef> QueryResultPtr;

public:

	virtual bool SetQueryResultPtr(odbc::ResultSetRef ResultReferance);
	virtual bool GetMetaData(FString& Out_Code);

	UFUNCTION(BlueprintCallable)
	virtual bool GetColumnCount(int32& ColumnCount);

	UFUNCTION(BlueprintCallable)
	virtual bool GetMetaDataStruct(FString& Out_Code, FSAP_ODBC_MetaData& Out_MetaData, int32 ColumnIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual bool GetString(FString& Out_Code, TArray<FString>& Out_String, int32 ColumnIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual bool GetInt(FString& Out_Code, TArray<int32>& Out_String, int32 ColumnIndex = 1);

};