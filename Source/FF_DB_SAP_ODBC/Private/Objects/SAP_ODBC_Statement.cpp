#include "Objects/SAP_ODBC_Statement.h"

void USAP_ODBC_Statement::SetInt(int32 Value, int32 ParamIndex)
{
	if (this->Statement.isNull())
	{
		return;
	}

	this->Statement->setInt(ParamIndex, Value);
}

void USAP_ODBC_Statement::SetString(FString Value, int32 ParamIndex)
{
	if (this->Statement.isNull())
	{
		return;
	}

	this->Statement->setCString(ParamIndex, TCHAR_TO_UTF8(*Value));
}

void USAP_ODBC_Statement::AddBatch()
{
	if (this->Statement.isNull())
	{
		return;
	}

	this->Statement->addBatch();
}

void USAP_ODBC_Statement::ExecuteBatch(FString& Out_Code)
{
	if (this->Statement.isNull())
	{
		return;
	}

	try
	{
		this->Statement->executeBatch();
	}

	catch (const odbc::Exception& Exception)
	{
		Out_Code = Exception.what();
	}
}

bool USAP_ODBC_Statement::CommitStatement(FString& Out_Code)
{
	if (this->Connection.isNull())
	{
		Out_Code = "FF SAP ODBC :  Connection reference is NULL !";
		return false;
	}

	if (!this->Connection->isValid())
	{
		Out_Code = "FF SAP ODBC : Connection reference is not valid !";
		return false;
	}

	if (!this->Connection->connected())
	{
		Out_Code = "FF SAP ODBC : There is no active connection !";
		return false;
	}

	this->Connection->commit();

	Out_Code = "FF SAP ODBC : Statement successfully commited !";
	return true;
}

bool USAP_ODBC_Statement::ExecuteQuery(FString& Out_Code, USAP_ODBC_Result*& Out_Result)
{
	if (this->Statement.isNull())
	{
		Out_Code = "FF SAP ODBC : Statement object is null !";
		return false;;
	}

	odbc::ResultSetRef QueryResult;

	try
	{
		QueryResult = this->Statement->executeQuery();
	}

	catch (const odbc::Exception& Exception)
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