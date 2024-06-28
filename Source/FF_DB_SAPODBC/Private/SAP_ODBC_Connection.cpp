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
	
	if (!ResultObject->GetMetaData(Out_Code))
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

	this->QueryResultPtr = MakeShared<odbc::ResultSetRef>(ResultReferance);
	return true;
}

bool USAP_ODBC_Result::GetMetaData(FString& Out_Code)
{
	if (!this->QueryResultPtr.IsValid())
	{
		return false;
	}

	if (this->QueryResultPtr->isNull())
	{
		return false;
	}
	
	try
	{
		this->MetaDataPtr = MakeShared< odbc::ResultSetMetaDataRef>(this->QueryResultPtr->get()->getMetaData());
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	return true;
}

bool USAP_ODBC_Result::GetColumnCount(int32& ColumnCount)
{
	if (!this->MetaDataPtr.IsValid())
	{
		return false;
	}

	if (this->MetaDataPtr->isNull())
	{
		return false;
	}

	ColumnCount = this->MetaDataPtr->get()->getColumnCount();
	return true;
}

bool USAP_ODBC_Result::GetMetaDataStruct(FString& Out_Code, FSAP_ODBC_MetaData& Out_MetaData, int32 ColumnIndex)
{
	if (!this->MetaDataPtr.IsValid())
	{
		return false;
	}

	if (this->MetaDataPtr->isNull())
	{
		return false;
	}

	FSAP_ODBC_MetaData MetaData;
	
	try
	{
		MetaData.ColumnScale = this->MetaDataPtr->get()->getScale(ColumnIndex);
		MetaData.ColumnPrecision = this->MetaDataPtr->get()->getPrecision(ColumnIndex);

		MetaData.ColumnDisplaySize = this->MetaDataPtr->get()->getColumnDisplaySize(ColumnIndex);
		MetaData.ColumnLenght = this->MetaDataPtr->get()->getColumnLength(ColumnIndex);
		MetaData.ColumnOctetLenght = this->MetaDataPtr->get()->getColumnOctetLength(ColumnIndex);

		MetaData.ColumnType = this->MetaDataPtr->get()->getColumnType(ColumnIndex);
		MetaData.ColumnTypeName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getColumnTypeName(ColumnIndex).c_str());

		MetaData.BaseColumnName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getBaseColumnName(ColumnIndex).c_str());
		MetaData.ColumnName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getColumnName(ColumnIndex).c_str());
		MetaData.ColumnLabel = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getColumnLabel(ColumnIndex).c_str());

		MetaData.BaseTableName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getBaseTableName(ColumnIndex).c_str());
		MetaData.TableName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getTableName(ColumnIndex).c_str());

		MetaData.CatalogName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getCatalogName(ColumnIndex).c_str());

		MetaData.SchemaName = UTF8_TO_TCHAR(this->MetaDataPtr->get()->getSchemaName(ColumnIndex).c_str());

		MetaData.bIsNullable = this->MetaDataPtr->get()->isNullable(ColumnIndex);
		MetaData.bIsAutoIncrement = this->MetaDataPtr->get()->isAutoIncrement(ColumnIndex);
		MetaData.bIsNamed = this->MetaDataPtr->get()->isNamed(ColumnIndex);
		MetaData.bIsCaseSensitive = this->MetaDataPtr->get()->isCaseSensitive(ColumnIndex);
		MetaData.bIsReadOnly = this->MetaDataPtr->get()->isReadOnly(ColumnIndex);
		MetaData.bIsSearchable = this->MetaDataPtr->get()->isSearchable(ColumnIndex);
		MetaData.bIsSigned = this->MetaDataPtr->get()->isSigned(ColumnIndex);
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	Out_MetaData = MetaData;

	return true;
}

bool USAP_ODBC_Result::GetString(FString& Out_Code, TArray<FString>& Out_String, int32 ColumnIndex)
{
	if (!this->QueryResultPtr.IsValid())
	{
		return false;
	}

	if (this->QueryResultPtr->isNull())
	{
		return false;
	}

	TArray<FString> TempArray;

	try
	{
		while (this->QueryResultPtr->get()->next())
		{
			FString EachString = this->QueryResultPtr->get()->getString(ColumnIndex)->c_str();
			TempArray.Add(EachString);
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
	if (!this->QueryResultPtr.IsValid())
	{
		return false;
	}

	if (this->QueryResultPtr->isNull())
	{
		return false;
	}

	TArray<int32> TempArray;
	 
	try
	{
		while (this->QueryResultPtr->get()->next())
		{
			int32 EachInt = this->QueryResultPtr->get()->getInt(ColumnIndex);
			TempArray.Add(EachInt);
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