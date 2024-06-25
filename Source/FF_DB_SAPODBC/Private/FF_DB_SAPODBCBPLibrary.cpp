// Copyright Epic Games, Inc. All Rights Reserved.

#include "FF_DB_SAPODBCBPLibrary.h"
#include "FF_DB_SAPODBC.h"

UFF_DB_SAPODBCBPLibrary::UFF_DB_SAPODBCBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UFF_DB_SAPODBCBPLibrary::SAP_ODBC_Init_Environment(USAP_ODBC_Environment*& Out_Environment)
{
#ifdef _WIN64
	USAP_ODBC_Environment* Object_Environment = NewObject<USAP_ODBC_Environment>();
	if (!IsValid(Object_Environment))
	{
		Out_Environment = nullptr;
		return false;
	}

	Object_Environment->SAP_ODBC_Environment = odbc::Environment::create();

	if (Object_Environment->SAP_ODBC_Environment.isNull())
	{
		Out_Environment = nullptr;
		return false;
	}

	else
	{
		Out_Environment = Object_Environment;
		return true;
	}

#else

	Out_Code = "Only Windows OS supported !";
	return false;

#endif
}

bool UFF_DB_SAPODBCBPLibrary::SAP_ODBC_Connection_Create(FString& Out_Code, UPARAM(ref)USAP_ODBC_Environment*& In_Environment, USAP_ODBC_Connection*& Out_Connection)
{
#ifdef _WIN64
	if (!IsValid(In_Environment))
	{
		Out_Code = "Environment object is not valid !";
		return false;
	}

	if (In_Environment->SAP_ODBC_Environment.isNull())
	{
		Out_Code = "Environment reference is not valid !";
		return false;
	}

	USAP_ODBC_Connection* Object_Connection = NewObject<USAP_ODBC_Connection>();

	if (!IsValid(Object_Connection))
	{
		Out_Code = "There is a problem with connection object creation !";
		return false;
	}

	Object_Connection->SAP_ODBC_Connection = In_Environment->SAP_ODBC_Environment->createConnection();

	if (Object_Connection->SAP_ODBC_Connection.isNull())
	{
		Out_Code = "There is a problem with connection reference creation !";
		Out_Connection = nullptr;

		return false;
	}

	else
	{
		Out_Code = "Connection successfully created.";
		Out_Connection = Object_Connection;

		return true;
	}

#else

	Out_Code = "Only Windows OS supported !";
	return false;

#endif
}

bool UFF_DB_SAPODBCBPLibrary::SAP_ODBC_Connection_Delete(FString& Out_Code, UPARAM(ref)USAP_ODBC_Connection*& In_Connection)
{

#ifdef _WIN64
	if (!IsValid(In_Connection))
	{
		Out_Code = "Connection object is not valid !";
		return false;
	}

	if (In_Connection->SAP_ODBC_Connection.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (In_Connection->SAP_ODBC_Connection->connected() || In_Connection->SAP_ODBC_Connection->isValid())
	{
		In_Connection->SAP_ODBC_Connection->disconnect();
	}

	In_Connection->SAP_ODBC_Connection.reset();
	In_Connection->SAP_ODBC_Connection = nullptr;
	In_Connection = nullptr;

	Out_Code = "Connection successfully deleted.";
	return true;

#else

	Out_Code = "Only Windows OS supported !";
	return false;

#endif
}

bool UFF_DB_SAPODBCBPLibrary::SAP_ODBC_Connection_Start(FString& Out_Code, UPARAM(ref)USAP_ODBC_Connection*& In_Connection, FString Driver, FString Username, FString Password, bool bUseAutoCommit)
{
#ifdef _WIN64
	
	if (!IsValid(In_Connection))
	{
		Out_Code = "Connection object is not valid !";
		return false;
	}

	if (In_Connection->SAP_ODBC_Connection.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (In_Connection->SAP_ODBC_Connection->connected() && In_Connection->SAP_ODBC_Connection->isValid())
	{
		Out_Code = "There is already an active connection !";
		return false;
	}

	In_Connection->SAP_ODBC_Connection->connect(TCHAR_TO_ANSI(*Driver), TCHAR_TO_ANSI(*Username), TCHAR_TO_ANSI(*Password));
	In_Connection->SAP_ODBC_Connection->setAutoCommit(bUseAutoCommit);

	if (In_Connection->SAP_ODBC_Connection->connected())
	{
		Out_Code = "Connection successfully started.";
		return true;
	}

	else
	{
		Out_Code = "There is a problem with connection start !";
		return false;
	}

#else

	Out_Code = "Only Windows OS supported !";
	return false;

#endif
}

bool UFF_DB_SAPODBCBPLibrary::SAP_ODBC_Connection_Stop(FString& Out_Code, UPARAM(ref)USAP_ODBC_Connection*& In_Connection)
{
#ifdef _WIN64
	if (!IsValid(In_Connection))
	{
		Out_Code = "Connection object is not valid !";
		return false;
	}

	if (In_Connection->SAP_ODBC_Connection.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!In_Connection->SAP_ODBC_Connection->connected())
	{
		Out_Code = "This connection reference hasn't been used for making connection, yet !";
		return false;
	}

	In_Connection->SAP_ODBC_Connection->disconnect();

	Out_Code = "Connection successfull stopped.";
	return true;
#else

	Out_Code = "Only Windows OS supported !";
	return false;

#endif
}

bool UFF_DB_SAPODBCBPLibrary::SAP_ODBC_Statement_Prepare(USAP_ODBC_Statement_Prepared*& Out_Statement_Prepared, FString& Out_Code, USAP_ODBC_Connection* In_Connection, FString SQL_Statement)
{
#ifdef _WIN64

	if (!IsValid(In_Connection))
	{
		Out_Code = "Connection object is not valid !";
		return false;
	}

	if (In_Connection->SAP_ODBC_Connection.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!In_Connection->SAP_ODBC_Connection->connected() && !In_Connection->SAP_ODBC_Connection->isValid())
	{
		Out_Code = "There is no active connection to prepare a statement !";
		return false;
	}
	
	/*
	std::string SQL_Statement_UTF8 = TCHAR_TO_UTF8(*SQL_Statement);
	std::u16string SQL_Statement_UTF16;
	SQL_Statement_UTF16.reserve(SQL_Statement_UTF8.size());
	std::copy(SQL_Statement_UTF8.begin(), SQL_Statement_UTF8.end(), std::back_inserter(SQL_Statement_UTF16));
	*/

	USAP_ODBC_Statement_Prepared* Object_Prepared_Statement = NewObject<USAP_ODBC_Statement_Prepared>();
	Object_Prepared_Statement->SAP_ODBC_Statement = In_Connection->SAP_ODBC_Connection->prepareStatement(reinterpret_cast<char16_t*>(TCHAR_TO_UTF16(*SQL_Statement)));

	Out_Statement_Prepared = Object_Prepared_Statement;
	Out_Code = "Statement created.";

	return true;

#else

	Out_Code = "Only Windows OS supported !";
	return false;

#endif
}