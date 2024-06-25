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