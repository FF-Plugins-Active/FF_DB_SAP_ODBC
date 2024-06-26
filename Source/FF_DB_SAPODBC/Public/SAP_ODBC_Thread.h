// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// UE Includes.
#include "HAL/Runnable.h"

// Custom Includes.
#include "SAP_ODBC_Connection.h"

// Fordward Declerations.
class FRunnableThread;
class ASAP_ODBC_Manager;

class FDB_Thread_SAPODBC : public FRunnable
{

public:

	// Sets default values for this actor's properties
	FDB_Thread_SAPODBC(ASAP_ODBC_Manager* In_Parent_Actor);

	// Destructor.
	virtual ~FDB_Thread_SAPODBC() override;

	virtual bool Init() override;

	virtual uint32 Run() override;

	virtual void Stop() override;

	virtual bool Toggle(bool bIsPause);

	ASAP_ODBC_Manager* Parent_Actor = nullptr;

private:

	bool bStartThread = false;
	FRunnableThread* RunnableThread = nullptr;

};