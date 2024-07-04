#pragma once

#include "CoreMinimal.h"

// UE Includes.
#include "JsonObjectWrapper.h"
#include "JsonUtilities.h"

// Custom Includes.
#include "SAP_ODBC_Includes.h"

#include "SAP_ODBC_Connection.generated.h"

USTRUCT(BlueprintType)
struct FF_DB_SAPODBC_API FSAP_ODBC_DataValue
{
	GENERATED_BODY()

public:

	FString ValString;
	int32 ValInt32 = 0;
	double ValDouble = 0.f;
	bool ValBool = false;
	FDateTime ValDateTime;

	UPROPERTY(BlueprintReadOnly)
	int32 DataType = 0;

	UPROPERTY(BlueprintReadOnly)
	FString DataTypeName;

	UPROPERTY(BlueprintReadOnly)
	FString ColumnName;

	UPROPERTY(BlueprintReadOnly)
	FString ValueRepresentation;

};

USTRUCT(BlueprintType)
struct FF_DB_SAPODBC_API FSAP_ODBC_MetaData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	int32 ColumnScale = 0;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	int32 ColumnPrecision = 0;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	int32 ColumnDisplaySize = 0;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	int32 ColumnLenght = 0;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	int32 ColumnOctetLenght = 0;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	int32 ColumnType = 0;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString ColumnTypeName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString BaseColumnName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString ColumnName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString ColumnLabel;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString BaseTableName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString TableName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString CatalogName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	FString SchemaName;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsNullable = false;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsAutoIncrement = false;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsNamed = false;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsCaseSensitive = false;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsReadOnly = false;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsSearchable = false;

	UPROPERTY(BlueprintReadOnly, AdvancedDisplay)
	bool bIsSigned = false;
};


UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Connection : public UObject
{
	GENERATED_BODY()

private:

	FString ConnectionId;
	odbc::ConnectionRef Connection;

public:

	virtual odbc::ConnectionRef GetConenction();
	virtual bool SetConnection(odbc::ConnectionRef In_Ref);
	virtual bool SetConnectionId(FString In_Id);
	virtual bool Connection_Start(FString& Out_Code, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit);
	virtual bool Connection_Stop(FString& Out_Code);
	
	UFUNCTION(BlueprintPure)
	virtual bool IsConnectionValid();

	UFUNCTION(BlueprintPure)
	virtual FString GetConnectionId();

	UFUNCTION(BlueprintCallable)
	virtual bool PrepareStatement(FString& Out_Code, USAP_ODBC_Statement*& Out_Statement, FString SQL_Statement);

};

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Statement : public UObject
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

UCLASS(BlueprintType)
class FF_DB_SAPODBC_API USAP_ODBC_Result : public UObject
{
	GENERATED_BODY()

protected:

	int32 RowCount = 0;
	TSharedPtr<odbc::ResultSetRef> QueryResult;
	TMap<FVector2D, FSAP_ODBC_DataValue> All_Data;

public:

	virtual bool SetQueryResult(FString& Out_Code, odbc::ResultSetRef ResultReferance);

	UFUNCTION(BlueprintPure)
	virtual int32 GetColumnCount();

	UFUNCTION(BlueprintPure)
	virtual int32 GetRowCount(int32& LastIndex);

	UFUNCTION(BlueprintCallable)
	virtual bool GetMetaData(FString& Out_Code, FSAP_ODBC_MetaData& Out_MetaData, const int32 ColumnIndex = 1);

	UFUNCTION(BlueprintCallable)
	virtual bool GetDataFromRow(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const int32 RowIndex);

	UFUNCTION(BlueprintCallable)
	virtual bool GetDataFromColumnIndex(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const int32 ColumnIndex);

	UFUNCTION(BlueprintCallable)
	virtual bool GetDataFromColumnName(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const FString ColumnName);

	UFUNCTION(BlueprintCallable)
	virtual bool GetSingleData(FString& Out_Code, FSAP_ODBC_DataValue& Out_Value, const FVector2D Position);

};