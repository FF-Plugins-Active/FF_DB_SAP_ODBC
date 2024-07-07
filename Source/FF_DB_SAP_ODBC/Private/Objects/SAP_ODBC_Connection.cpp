#include "Objects/SAP_ODBC_Connection.h"

bool USAP_ODBC_Connection::SetConnection(odbc::ConnectionRef In_Ref, const FString In_Id)
{
	if (In_Ref.isNull())
	{
		return false;
	}

	if (In_Id.IsEmpty())
	{
		return false;
	}

	this->Connection = In_Ref;
	this->ConnectionId = In_Id;
	return true;
}

odbc::ConnectionRef USAP_ODBC_Connection::GetConenction()
{
	if (this->Connection.isNull())
	{
		return nullptr;
	}

	return nullptr;
}

bool USAP_ODBC_Connection::IsConnectionValid()
{
	return this->Connection.isNull();
}

FString USAP_ODBC_Connection::GetConnectionId()
{
	return this->ConnectionId;
}

bool USAP_ODBC_Connection::Connection_Start(FString& Out_Code, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit)
{
	if (this->Connection.isNull())
	{
		Out_Code = "Connection referance is null !";
		return false;
	}

	if (this->Connection->connected() && this->Connection->isValid())
	{
		Out_Code = "There is an already active connection !";
		return false;
	}

	bool bConnectionStatus = false;

	try
	{
		this->Connection->connect(TCHAR_TO_ANSI(*In_Server), TCHAR_TO_ANSI(*In_UserName), TCHAR_TO_ANSI(*In_Password));
		this->Connection->setAutoCommit(bUseAutoCommit);
		bConnectionStatus = this->Connection->connected();
	}
	
	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	Out_Code  = bConnectionStatus ? "Connection successfully started." : "Connection couldn't established !";
	return bConnectionStatus;
}

bool USAP_ODBC_Connection::Connection_Stop(FString& Out_Code)
{
	if (this->Connection.isNull())
	{
		Out_Code = "Connection reference is NULL !";
		return false;
	}

	if (!this->Connection->isValid())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!this->Connection->connected())
	{
		Out_Code = "There is no active connection !";
		return false;
	}

	this->Connection->disconnect();
	
	Out_Code = "Connection successfull stopped.";
	return true;
}

bool USAP_ODBC_Connection::Connection_Delete(FString& Out_Code)
{
	if (this->Connection.isNull())
	{
		Out_Code = "Connection reference is NULL !";
		return false;
	}

	this->Connection.reset();
	this->Connection = nullptr;

	return true;
}

bool USAP_ODBC_Connection::PrepareStatement(FString& Out_Code, USAP_ODBC_Statement*& Out_Statement, FString SQL_Statement)
{
	if (this->Connection.isNull())
	{
		Out_Code = "Connection reference is NULL !";
		return false;
	}

	if (!this->Connection->isValid())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!this->Connection->connected())
	{
		Out_Code = "There is no active connection !";
		return false;
	}

	odbc::PreparedStatementRef TempStatement = this->Connection->prepareStatement(reinterpret_cast<char16_t*>(TCHAR_TO_UTF16(*SQL_Statement)));
	
	if (TempStatement.isNull())
	{
		Out_Code = "There is a problem while preparing statement !";
		return false;
	}

	Out_Statement = NewObject<USAP_ODBC_Statement>();
	Out_Statement->Statement = MakeShared<odbc::PreparedStatementRef>(TempStatement);
	Out_Statement->Connection = MakeShared< odbc::ConnectionRef>(this->Connection);

	Out_Code = "Statement successfully created !";
	return true;
}