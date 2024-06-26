#include "SAP_ODBC_Connection.h"

// CONNECTION.

bool USAP_ODBC_Connection::Connection_Start(FString& Out_Code, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit)
{
	if (this->ConnectionRef.isNull())
	{
		Out_Code = "Connection referance is null !";
		return false;
	}

	if (this->ConnectionRef->connected() && this->ConnectionRef->isValid())
	{
		Out_Code = "There is an already active connection !";
		return false;
	}

	FString ErrorLog;

	try
	{
		this->ConnectionRef->connect(TCHAR_TO_ANSI(*In_Server), TCHAR_TO_ANSI(*In_UserName), TCHAR_TO_ANSI(*In_Password));
		this->ConnectionRef->setAutoCommit(bUseAutoCommit);
	}
	
	catch (const std::exception& Exception)
	{
		ErrorLog = Exception.what();
	}
	
	if (this->ConnectionRef->connected())
	{
		Out_Code = "Connection successfully started.";
		return true;
	}

	else
	{
		Out_Code = "There is a problem with connection start ! = " + ErrorLog;
		return false;
	}
}

bool USAP_ODBC_Connection::Connection_Stop(FString& Out_Code)
{
	if (this->ConnectionRef.isNull())
	{
		Out_Code = "Connection reference is NULL !";
		return false;
	}

	if (!this->ConnectionRef->isValid())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!this->ConnectionRef->connected())
	{
		Out_Code = "There is no active connection !";
		return false;
	}

	this->ConnectionRef->disconnect();
	
	Out_Code = "Connection successfull stopped.";
	return true;
}

bool USAP_ODBC_Connection::Connection_Delete(FString& Out_Code)
{
	if (this->ConnectionRef.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	FString Out_Code_Stop;
	this->Connection_Stop(Out_Code_Stop);

	this->ConnectionRef.reset();
	this->ConnectionRef = nullptr;

	Out_Code = "Connection successfully deleted. Please destroy this UObject !";
	return true;
}

bool USAP_ODBC_Connection::PrepareStatement(FString& Out_Code, USAP_ODBC_Statement*& Out_Statement, FString SQL_Statement)
{
	if (this->ConnectionRef.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!this->ConnectionRef->connected() && !this->ConnectionRef->isValid())
	{
		Out_Code = "There is no active connection to prepare a statement !";
		return false;
	}

	odbc::PreparedStatementRef Statement = this->ConnectionRef->prepareStatement(reinterpret_cast<char16_t*>(TCHAR_TO_UTF16(*SQL_Statement)));
	
	if (Statement.isNull())
	{
		Out_Code = "There is a problem while preparing statement !";
		return false;
	}

	Out_Statement = NewObject<USAP_ODBC_Statement>();
	Out_Statement->StatementPtr = MakeShared<odbc::PreparedStatementRef>(Statement);

	Out_Code = "Statement successfully created !";
	return true;
}

// STATEMENT.

bool USAP_ODBC_Connection::CommitStatement(FString& Out_Code)
{
	if (this->ConnectionRef.isNull())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!this->ConnectionRef->connected() && !this->ConnectionRef->isValid())
	{
		Out_Code = "There is no active connection to commit target statement !";
		return false;
	}

	this->ConnectionRef->commit();

	Out_Code = "Statement successfully commited !";
	return true;
}

void USAP_ODBC_Statement::SetInt(int32 Value, int32 ParamIndex)
{
	if (!this->StatementPtr.IsValid())
	{
		return;
	}

	if (this->StatementPtr->isNull())
	{
		return;
	}

	this->StatementPtr->get()->setInt(ParamIndex, Value);
}

void USAP_ODBC_Statement::SetString(FString Value, int32 ParamIndex)
{
	if (!this->StatementPtr.IsValid())
	{
		return;
	}

	if (this->StatementPtr->isNull())
	{
		return;
	}

	this->StatementPtr->get()->setCString(ParamIndex, TCHAR_TO_UTF8(*Value));
}

void USAP_ODBC_Statement::AddBatch()
{
	if (!this->StatementPtr.IsValid())
	{
		return;
	}

	if (this->StatementPtr->isNull())
	{
		return;
	}

	this->StatementPtr->get()->addBatch();
}

void USAP_ODBC_Statement::ExecuteBatch(FString& Out_Code)
{
	if (!this->StatementPtr.IsValid())
	{
		return;
	}

	if (this->StatementPtr->isNull())
	{
		return;
	}

	try
	{
		this->StatementPtr->get()->executeBatch();
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
	}
}

// RESULT.

bool USAP_ODBC_Result::GetString(FString& Out_Code, FString& Out_String, int32 ColumnIndex)
{
	if (this->QueryResult.isNull())
	{
		Out_Code = "Query is NULL !";
		return false;
	}

	//this->QueryResult->

	//Out_String =  ? "" : this->QueryResult->getString(1)->c_str();
	return true;
}