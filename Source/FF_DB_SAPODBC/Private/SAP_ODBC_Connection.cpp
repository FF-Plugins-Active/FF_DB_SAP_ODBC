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
	Out_Statement->ConnectionPtr = MakeShared< odbc::ConnectionRef>(this->ConnectionRef);

	Out_Code = "Statement successfully created !";
	return true;
}

// STATEMENT.

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

bool USAP_ODBC_Statement::CommitStatement(FString& Out_Code)
{
	if (!this->ConnectionPtr.IsValid())
	{
		Out_Code = "Connection pointer is not valid !";
		return false;
	}

	if (this->ConnectionPtr->isNull())
	{
		Out_Code = "Connection referance is not valid !";
		return false;
	}

	if (!this->ConnectionPtr->get()->connected() && !this->ConnectionPtr->get()->isValid())
	{
		Out_Code = "There is no active connection to commit target statement !";
		return false;
	}

	this->ConnectionPtr->get()->commit();

	Out_Code = "Statement successfully commited !";
	return true;
}

bool USAP_ODBC_Statement::ExecuteQuery(FString& Out_Code, USAP_ODBC_Result*& Out_Result)
{
	if (!this->StatementPtr.IsValid())
	{
		return false;
	}

	if (this->StatementPtr->isNull())
	{
		return false;;
	}

	odbc::ResultSetRef QueryResult;

	try
	{
		QueryResult = this->StatementPtr->get()->executeQuery();
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	USAP_ODBC_Result* ResultObject = NewObject<USAP_ODBC_Result>();

	if (!ResultObject->SetQueryResultPtr(QueryResult))
	{
		return false;
	}
	
	Out_Result = ResultObject;
	return true;
}

// RESULT.

bool USAP_ODBC_Result::SetQueryResultPtr(odbc::ResultSetRef ResultReferance)
{
	if (ResultReferance.isNull())
	{
		return false;
	}
	
	this->QueryResult = ResultReferance;
	return true;
}

bool USAP_ODBC_Result::GetColumnCount(int32& ColumnCount)
{
	if (this->QueryResult.isNull())
	{
		return false;
	}

	ColumnCount = this->QueryResult->getMetaData()->getColumnCount();

	return true;
}

bool USAP_ODBC_Result::GetMetaDataStruct(FString& Out_Code, FSAP_ODBC_MetaData& Out_MetaData, int32 ColumnIndex)
{
	odbc::ResultSetMetaData* Result = nullptr;

	try
	{
		Result = this->QueryResult->getMetaData().get();
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	FSAP_ODBC_MetaData MetaData;
	MetaData.ColumnScale = Result->getScale(ColumnIndex);
	MetaData.ColumnPrecision = Result->getPrecision(ColumnIndex);

	MetaData.ColumnDisplaySize = Result->getColumnDisplaySize(ColumnIndex);
	MetaData.ColumnLenght = Result->getColumnLength(ColumnIndex);
	MetaData.ColumnOctetLenght = Result->getColumnOctetLength(ColumnIndex);

	MetaData.ColumnType = Result->getColumnType(ColumnIndex);
	MetaData.ColumnTypeName = UTF8_TO_TCHAR(Result->getColumnTypeName(ColumnIndex).c_str());

	MetaData.BaseColumnName = UTF8_TO_TCHAR(Result->getBaseColumnName(ColumnIndex).c_str());
	MetaData.ColumnName = UTF8_TO_TCHAR(Result->getColumnName(ColumnIndex).c_str());
	MetaData.ColumnLabel = UTF8_TO_TCHAR(Result->getColumnLabel(ColumnIndex).c_str());

	MetaData.BaseTableName = UTF8_TO_TCHAR(Result->getBaseTableName(ColumnIndex).c_str());
	MetaData.TableName = UTF8_TO_TCHAR(Result->getTableName(ColumnIndex).c_str());

	MetaData.CatalogName = UTF8_TO_TCHAR(Result->getCatalogName(ColumnIndex).c_str());

	MetaData.SchemaName = UTF8_TO_TCHAR(Result->getSchemaName(ColumnIndex).c_str());

	MetaData.bIsNullable = Result->isNullable(ColumnIndex);
	MetaData.bIsAutoIncrement = Result->isAutoIncrement(ColumnIndex);
	MetaData.bIsNamed = Result->isNamed(ColumnIndex);
	MetaData.bIsCaseSensitive = Result->isCaseSensitive(ColumnIndex);
	MetaData.bIsReadOnly = Result->isReadOnly(ColumnIndex);
	MetaData.bIsSearchable = Result->isSearchable(ColumnIndex);
	MetaData.bIsSigned = Result->isSigned(ColumnIndex);

	Out_MetaData = MetaData;

	return true;
}

bool USAP_ODBC_Result::GetString(FString& Out_Code, TArray<FString>& Out_String, int32 ColumnIndex)
{
	if (this->QueryResult.isNull())
	{
		Out_Code = "Query result referance is null !";
		return false;
	}

	TArray<FString> TempArray;

	try
	{
		while (this->QueryResult->next())
		{
			odbc::String ValueString = this->QueryResult->getString(ColumnIndex);

			if (!ValueString.isNull())
			{
				TempArray.Add(UTF8_TO_TCHAR(ValueString->c_str()));
			}
		}	
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	Out_String = TempArray;

	return true;
}

bool USAP_ODBC_Result::GetInt(FString& Out_Code, TArray<int32>& Out_String, int32 ColumnIndex)
{
	if (this->QueryResult.isNull())
	{
		Out_Code = "Query result referance is null !";
		return false;
	}

	TArray<int32> TempArray;

	try
	{
		while (this->QueryResult->next())
		{
			odbc::Int ValueInt = this->QueryResult->getInt(ColumnIndex);
			if (!ValueInt.isNull())
			{
				TempArray.Add(*ValueInt);
			}
		}
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	Out_String = TempArray;

	return true;
}