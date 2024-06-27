// Fill out your copyright notice in the Description page of Project Settings.

#include "SAP_ODBC_Manager.h"

// Sets default values
ASAP_ODBC_Manager::ASAP_ODBC_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASAP_ODBC_Manager::BeginPlay()
{
	Super::BeginPlay();
}

void ASAP_ODBC_Manager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ASAP_ODBC_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASAP_ODBC_Manager::SAP_ODBC_Init_Environment()
{
	odbc::EnvironmentRef Environment = odbc::Environment::create();

	if (Environment.isNull())
	{
		return false;
	}

	this->SAP_ODBC_Environment = Environment;

	return true;
}

bool ASAP_ODBC_Manager::SAP_ODBC_Connection_Create(FString& Out_Code, USAP_ODBC_Connection*& CreatedConnection, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit)
{
	if (In_Server.IsEmpty())
	{
		Out_Code = "Server address should not be empty !";
		return false;
	}

	if (In_UserName.IsEmpty())
	{
		Out_Code = "User name should not be empty !";
		return false;
	}

	if (this->SAP_ODBC_Environment.isNull())
	{
		Out_Code = "You have to create ODBC environment first !";
		return false;
	}

	odbc::ConnectionRef ConnectionRef = this->SAP_ODBC_Environment->createConnection();
	if (ConnectionRef.isNull())
	{
		Out_Code = "There is a problem while creating ODBC Connection referance !";
		return false;
	}

	USAP_ODBC_Connection* Connection_Object = NewObject<USAP_ODBC_Connection>();
	Connection_Object->ConnectionRef = ConnectionRef;

	FString Out_Code_Start;
	if (Connection_Object->Connection_Start(Out_Code_Start, In_Server, In_UserName, In_Password, bUseAutoCommit))
	{
		FString ConnectionId = In_Server + "&&" + In_UserName;
		Connection_Object->ConnectionId = ConnectionId;

		CreatedConnection = Connection_Object;

		this->MAP_Connections.Add(ConnectionId, CreatedConnection);

		Out_Code = Out_Code_Start;
		return true;
	}

	else
	{
		Out_Code = Out_Code_Start;
		return false;
	}
}

bool ASAP_ODBC_Manager::SAP_ODBC_Connection_Delete_Id(FString& Out_Code, FString ConnectionId)
{
	if (ConnectionId.IsEmpty())
	{
		return false;
	}

	USAP_ODBC_Connection* TargetConnection = *this->MAP_Connections.Find(ConnectionId);

	if (!IsValid(TargetConnection))
	{
		return false;
	}
	
	if (TargetConnection->ConnectionRef.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	FString Out_Code_Stop;
	TargetConnection->Connection_Stop(Out_Code_Stop);

	TargetConnection->ConnectionRef.reset();
	TargetConnection->ConnectionRef = nullptr;

	MAP_Connections.Remove(ConnectionId);
	TargetConnection = nullptr;

	Out_Code = "Connection successfully deleted !";
	return true;
}

bool ASAP_ODBC_Manager::SAP_ODBC_Connection_Delete_Object(FString& Out_Code, UPARAM(ref) USAP_ODBC_Connection*& TargetConnection)
{
	if (!IsValid(TargetConnection))
	{
		return false;
	}

	if (TargetConnection->ConnectionRef.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	FString Out_Code_Stop;
	TargetConnection->Connection_Stop(Out_Code_Stop);

	TargetConnection->ConnectionRef.reset();
	TargetConnection->ConnectionRef = nullptr;

	this->MAP_Connections.Remove(TargetConnection->ConnectionId);
	TargetConnection = nullptr;

	Out_Code = "Connection successfully deleted !";
	return true;
}