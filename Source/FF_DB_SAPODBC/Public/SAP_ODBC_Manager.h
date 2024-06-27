// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SAP_ODBC_Thread.h"
#include "SAP_ODBC_Connection.h"

#include "SAP_ODBC_Manager.generated.h"

UCLASS()
class FF_DB_SAPODBC_API ASAP_ODBC_Manager : public AActor
{
	GENERATED_BODY()
	
protected:
	
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Called when the game end or when destroyed.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	odbc::EnvironmentRef SAP_ODBC_Environment;

public:	
	
	// Sets default values for this actor's properties.
	ASAP_ODBC_Manager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual bool SAP_ODBC_Init_Environment();

	UFUNCTION(BlueprintCallable)
	virtual bool SAP_ODBC_Connection_Create(FString& Out_Code, USAP_ODBC_Connection*& CreatedConnection, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit);

	UFUNCTION(BlueprintCallable)
	virtual bool SAP_ODBC_Connection_Delete_Id(FString& Out_Code, FString ConnectionId);

	UFUNCTION(BlueprintCallable)
	virtual bool SAP_ODBC_Connection_Delete_Object(FString& Out_Code, UPARAM(ref) USAP_ODBC_Connection*& TargetConnection);

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, USAP_ODBC_Connection*> MAP_Connections;

};