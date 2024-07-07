#pragma once

#include "CoreMinimal.h"

#include "SAP_ODBC_Includes.h"

#include "SAP_ODBC_Result.generated.h"

USTRUCT(BlueprintType)
struct FF_DB_SAP_ODBC_API FSAP_ODBC_DataValue
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = "Use it for \"datetime, nvarchar, text\""))
	FString String;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = "Use it for \"int32\""))
	int32 Integer32 = 0;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = "Use it for \"int32\""))
	int64 Integer64 = 0;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	double Double = (double)0.f;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	bool Bool = false;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	FDateTime DateTime;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	int32 DataType = 0;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	FString DataTypeName;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	FString ColumnName;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	FString Preview;

	UPROPERTY(BlueprintReadOnly, meta = (ToolTip = ""))
	FString Note;

};

USTRUCT(BlueprintType)
struct FF_DB_SAP_ODBC_API FSAP_ODBC_MetaData
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
class FF_DB_SAP_ODBC_API USAP_ODBC_Result : public UObject
{
	GENERATED_BODY()

protected:

	int32 RowCount = 0;
	odbc::ResultSetRef QueryResult;
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