// Copyright Epic Games, Inc. All Rights Reserved.

#include "SAP_ODBC_BPLibrary.h"
#include "FF_DB_SAP_ODBC.h"

USAP_ODBC_BPLibrary::USAP_ODBC_BPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FString USAP_ODBC_BPLibrary::InsertInto(FString Target, TSet<FString> Placeholders)
{
	FString MainStatement = "INSERT INTO " + Target + " (";

	const int32 PlaceHoldersLenght = Placeholders.Num();

	FString ValueArea;
	for (int32 Index_PlaceHolder = 0; Index_PlaceHolder < PlaceHoldersLenght; Index_PlaceHolder++)
	{
		FString EachPlaceHolder = Placeholders.Array()[Index_PlaceHolder];
		MainStatement += EachPlaceHolder;

		ValueArea += "?";

		if (Index_PlaceHolder != PlaceHoldersLenght - 1)
		{
			MainStatement += ", ";

			ValueArea += ", ";
		}
	}

	ValueArea += ")";
	MainStatement += ") VALUES (" + ValueArea;

	return MainStatement;
}

FString USAP_ODBC_BPLibrary::SelectFrom(FString Target, FString Where, TSet<FString> Placeholders)
{
	return FString();
}

FJsonObjectWrapper USAP_ODBC_BPLibrary::PrintMetaData(FSAP_ODBC_MetaData In_MetaData)
{
	FJsonObjectWrapper JSonObject;

	JSonObject.JsonObject->SetStringField("Base Table Name", In_MetaData.BaseTableName);
	JSonObject.JsonObject->SetStringField("Table Name", In_MetaData.TableName);
	JSonObject.JsonObject->SetStringField("Catalog Name", In_MetaData.CatalogName);
	JSonObject.JsonObject->SetStringField("Schema Name", In_MetaData.SchemaName);

	JSonObject.JsonObject->SetStringField("Base Column Name", In_MetaData.BaseColumnName);
	JSonObject.JsonObject->SetStringField("Column Name", In_MetaData.ColumnName);
	JSonObject.JsonObject->SetStringField("Column Label", In_MetaData.ColumnLabel);
	JSonObject.JsonObject->SetStringField("Column Type Name", In_MetaData.ColumnTypeName);

	JSonObject.JsonObject->SetNumberField("Column Type", In_MetaData.ColumnType);
	JSonObject.JsonObject->SetNumberField("Column Lenght", In_MetaData.ColumnLenght);
	JSonObject.JsonObject->SetNumberField("Column Display Size", In_MetaData.ColumnDisplaySize);
	JSonObject.JsonObject->SetNumberField("Column Octet Lenght", In_MetaData.ColumnOctetLenght);
	JSonObject.JsonObject->SetNumberField("Column Precision", In_MetaData.ColumnPrecision);
	JSonObject.JsonObject->SetNumberField("Column Scale", In_MetaData.ColumnScale);

	JSonObject.JsonObject->SetBoolField("Is Auto Increment", In_MetaData.bIsAutoIncrement);
	JSonObject.JsonObject->SetBoolField("Is Case Sensitive", In_MetaData.bIsCaseSensitive);
	JSonObject.JsonObject->SetBoolField("Is Named", In_MetaData.bIsNamed);
	JSonObject.JsonObject->SetBoolField("Is Nullable", In_MetaData.bIsNullable);
	JSonObject.JsonObject->SetBoolField("Is Read Only", In_MetaData.bIsReadOnly);
	JSonObject.JsonObject->SetBoolField("Is Signed", In_MetaData.bIsSigned);
	JSonObject.JsonObject->SetBoolField("Is Searchable", In_MetaData.bIsSearchable);

	return JSonObject;
}