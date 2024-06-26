// Copyright Epic Games, Inc. All Rights Reserved.

#include "SAP_ODBC_BPLibrary.h"
#include "FF_DB_SAPODBC.h"

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