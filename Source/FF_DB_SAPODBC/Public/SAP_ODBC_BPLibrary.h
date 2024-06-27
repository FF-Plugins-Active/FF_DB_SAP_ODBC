// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "SAP_ODBC_BPLibrary.generated.h"

UCLASS()
class USAP_ODBC_BPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, meta = (DisplayName = "SAP ODBC - INSERT INTO STATEMENT TEMPLATE", Keywords = "odbc, sap, statement, template, insert, into"), Category = "Frozen Forest|Database|SAP ODBC")
	static FString InsertInto(FString Target, TSet<FString> Placeholders);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "SAP ODBC - SELECT FROM", Keywords = "odbc, sap, statement, template, insert, into"), Category = "Frozen Forest|Database|SAP ODBC")
	static FString SelectFrom(FString Target, FString Where, TSet<FString> Placeholders);

};