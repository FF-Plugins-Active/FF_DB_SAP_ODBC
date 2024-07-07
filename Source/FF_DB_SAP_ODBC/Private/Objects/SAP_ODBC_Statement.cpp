#include "Objects/SAP_ODBC_Statement.h"

void USAP_ODBC_Statement::SetInt(int32 Value, int32 ParamIndex)
{
	if (!this->Statement.IsValid())
	{
		return;
	}

	if (this->Statement->isNull())
	{
		return;
	}

	this->Statement->get()->setInt(ParamIndex, Value);
}

void USAP_ODBC_Statement::SetString(FString Value, int32 ParamIndex)
{
	if (!this->Statement.IsValid())
	{
		return;
	}

	if (this->Statement->isNull())
	{
		return;
	}

	this->Statement->get()->setCString(ParamIndex, TCHAR_TO_UTF8(*Value));
}

void USAP_ODBC_Statement::AddBatch()
{
	if (!this->Statement.IsValid())
	{
		return;
	}

	if (this->Statement->isNull())
	{
		return;
	}

	this->Statement->get()->addBatch();
}

void USAP_ODBC_Statement::ExecuteBatch(FString& Out_Code)
{
	if (!this->Statement.IsValid())
	{
		return;
	}

	if (this->Statement->isNull())
	{
		return;
	}

	try
	{
		this->Statement->get()->executeBatch();
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
	}
}

bool USAP_ODBC_Statement::CommitStatement(FString& Out_Code)
{
	if (!this->Connection.IsValid())
	{
		Out_Code = "Connection pointer is not valid !";
		return false;
	}

	if (this->Connection->isNull())
	{
		Out_Code = "Connection reference is NULL !";
		return false;
	}

	if (!this->Connection->get()->isValid())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	if (!this->Connection->get()->connected())
	{
		Out_Code = "There is no active connection !";
		return false;
	}

	this->Connection->get()->commit();

	Out_Code = "Statement successfully commited !";
	return true;
}

bool USAP_ODBC_Statement::ExecuteQuery(FString& Out_Code, USAP_ODBC_Result*& Out_Result)
{
	if (!this->Statement.IsValid())
	{
		return false;
	}

	if (this->Statement->isNull())
	{
		return false;;
	}

	odbc::ResultSetRef QueryResult;

	try
	{
		QueryResult = this->Statement->get()->executeQuery();
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	USAP_ODBC_Result* ResultObject = NewObject<USAP_ODBC_Result>();

	if (!ResultObject->SetQueryResult(Out_Code, QueryResult))
	{
		return false;
	}

	Out_Result = ResultObject;
	return true;
}