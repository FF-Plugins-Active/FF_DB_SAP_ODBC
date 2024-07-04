#include "SAP_ODBC_Connection.h"

// UE Includes.
#include "Kismet/KismetMathLibrary.h"

// CONNECTION.

bool USAP_ODBC_Connection::SetConnection(odbc::ConnectionRef In_Ref)
{
	if (In_Ref.isNull())
	{
		return false;
	}

	this->Connection = In_Ref;
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

bool USAP_ODBC_Connection::SetConnectionId(FString In_Id)
{
	if (In_Id.IsEmpty())
	{
		return false;
	}

	if (!this->ConnectionId.IsEmpty())
	{
		return false;
	}

	this->ConnectionId = In_Id;
	return true;
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
	this->Connection.reset();
	this->Connection = nullptr;
	
	Out_Code = "Connection successfull stopped.";
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

// STATEMENT.

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

// RESULT.

bool USAP_ODBC_Result::SetQueryResult(FString& Out_Code, odbc::ResultSetRef ResultReferance)
{
	if (ResultReferance.isNull())
	{
		return false;
	}
	
	int32 Index_Row = 0;
	TMap<FVector2D, FSAP_ODBC_DataValue> Temp_Data;

	try
	{
		const int32 ColumnCount = ResultReferance->getMetaData()->getColumnCount();

		while (ResultReferance->next())
		{
			for (int32 Index_Column_Raw = 0; Index_Column_Raw < ColumnCount; Index_Column_Raw++)
			{
				const int32 Index_Column = Index_Column_Raw + 1;

				const int32 DataType = ResultReferance->getMetaData()->getColumnType(Index_Column);
				const FString DataTypeName = UTF8_TO_TCHAR(ResultReferance->getMetaData()->getColumnTypeName(Index_Column).c_str());
				const FString ColumnName = UTF8_TO_TCHAR(ResultReferance->getMetaData()->getColumnName(Index_Column).c_str());

				FSAP_ODBC_DataValue EachData;
				EachData.DataType = DataType;
				EachData.DataTypeName = DataTypeName;
				EachData.ColumnName = ColumnName;

				if (DataType == 4)
				{
					odbc::Int TempInt = ResultReferance->getInt(Index_Column);

					if (!TempInt.isNull())
					{
						EachData.ValInt32 = *TempInt;
						EachData.ValueRepresentation = FString::FromInt(*TempInt);
					}
				}

				if (DataType == 6)
				{
					odbc::Double TempDouble = ResultReferance->getDouble(Index_Column);

					if (!TempDouble.isNull())
					{
						EachData.ValDouble = *TempDouble;
						EachData.ValueRepresentation = FString::SanitizeFloat(*TempDouble);
					}
				}

				// MSSQL's "time" dateType is an nvarchar, too.
				if (DataType == -9 || DataType == -1 || DataType == -2 || DataType == -9 || DataType == -93)
				{
					odbc::String TempStringRaw = ResultReferance->getString(Index_Column);

					if (!TempStringRaw.isNull())
					{
						FString TempString = UTF8_TO_TCHAR(TempStringRaw->c_str());
						EachData.ValString = TempString;
						EachData.ValueRepresentation = TempString;
					}
				}

				/*
				if (DataType == -9 || DataType == -93)
				{
					odbc::Date Section_Date = ResultReferance->getDate(Index_Column);
					odbc::Time Section_Time = ResultReferance->getTime(Index_Column);

					if (!Section_Date.isNull() && !Section_Time.isNull())
					{
						FDateTime DateTime = UKismetMathLibrary::MakeDateTime(Section_Date->year(), Section_Date->month(), Section_Date->day(), Section_Time->hour(), Section_Time->minute(), Section_Time->second());
						EachData.ValDateTime = DateTime;
						EachData.ValueRepresentation = DateTime.ToString();
					}
				}

				if (DataType == -2)
				{
					odbc::Timestamp TimeStamp = ResultReferance->getTimestamp(Index_Column);

					if (!TimeStamp.isNull())
					{
						FDateTime DateTime = UKismetMathLibrary::MakeDateTime(TimeStamp->year(), TimeStamp->month(), TimeStamp->day(), TimeStamp->hour(), TimeStamp->minute(), TimeStamp->second(), TimeStamp->milliseconds());
						EachData.ValDateTime = DateTime;
						EachData.ValueRepresentation = DateTime.ToString();
					}
				}
				*/
				Temp_Data.Add(FVector2D(Index_Row, Index_Column_Raw), EachData);
			}

			Index_Row += 1;
		}
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	this->QueryResult = MakeShared<odbc::ResultSetRef>(ResultReferance);
	this->All_Data = Temp_Data;
	this->RowCount = Index_Row;

	return true;
}

int32 USAP_ODBC_Result::GetColumnCount()
{
	if (this->QueryResult->isNull())
	{
		return 0;
	}

	return this->QueryResult->get()->getMetaData()->getColumnCount();
}

int32 USAP_ODBC_Result::GetRowCount(int32& LastIndex)
{
	if (this->QueryResult->isNull())
	{
		return 0;
	}

	if (this->RowCount != 0)
	{
		LastIndex = this->RowCount - 1;
	}

	return this->RowCount;
}

bool USAP_ODBC_Result::GetMetaData(FString& Out_Code, FSAP_ODBC_MetaData& Out_MetaData, const int32 ColumnIndex)
{
	odbc::ResultSetMetaData* Result = nullptr;

	try
	{
		Result = this->QueryResult->get()->getMetaData().get();
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

bool USAP_ODBC_Result::GetDataFromRow(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const int32 RowIndex)
{
	if (this->All_Data.IsEmpty())
	{
		Out_Code = "Data pool is empty !";
		return false;
	}

	if (RowIndex < 0 && this->RowCount >= RowIndex)
	{
		Out_Code = "Given row index is out of data pool's range !";
		return false;
	}

	try
	{
		const int32 ColumnCount = this->QueryResult->get()->getMetaData()->getColumnCount();
		TArray<FSAP_ODBC_DataValue> Temp_Array;

		for (int32 Index_Column = 0; Index_Column < ColumnCount; Index_Column++)
		{
			Temp_Array.Add(*this->All_Data.Find(FVector2D(RowIndex, Index_Column)));
		}

		Out_Value = Temp_Array;
	}

	catch (const std::exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	return true;
}

bool USAP_ODBC_Result::GetDataFromColumnIndex(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const int32 ColumnIndex)
{
	if (this->All_Data.IsEmpty())
	{
		Out_Code = "Data pool is empty !";
		return false;
	}

	TArray<FSAP_ODBC_DataValue> Temp_Array;

	for (int32 Index_Row = 0; Index_Row < this->RowCount; Index_Row++)
	{
		Temp_Array.Add(*this->All_Data.Find(FVector2D(Index_Row, ColumnIndex)));
	}

	Out_Value = Temp_Array;
	return true;
}

bool USAP_ODBC_Result::GetDataFromColumnName(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const FString ColumnName)
{
	if (this->All_Data.IsEmpty())
	{
		Out_Code = "Data pool is empty !";
		return false;
	}

	int32 TargetColumnIndex = 0;
	odbc::ResultSetMetaDataRef MetaDataRef = this->QueryResult->get()->getMetaData();
	const int32 ColumnCount = MetaDataRef->getColumnCount();

	for (int32 Index_Column = 1; Index_Column < ColumnCount + 1; Index_Column++)
	{
		FString EachColumnName = UTF8_TO_TCHAR(MetaDataRef->getColumnName(Index_Column).c_str());

		if (ColumnName == EachColumnName)
		{
			TargetColumnIndex = Index_Column;
			break;
		}
	}

	TArray<FSAP_ODBC_DataValue> Temp_Array;

	for (int32 Index_Row = 0; Index_Row < this->RowCount; Index_Row++)
	{
		Temp_Array.Add(*this->All_Data.Find(FVector2D(Index_Row, TargetColumnIndex)));
	}

	Out_Value = Temp_Array;
	return true;
}

bool USAP_ODBC_Result::GetSingleData(FString& Out_Code, FSAP_ODBC_DataValue& Out_Value, const FVector2D Position)
{
	if (this->All_Data.IsEmpty())
	{
		Out_Code = "Data pool is empty !";
		return false;
	}

	FSAP_ODBC_DataValue* TempValue = this->All_Data.Find(Position);

	if (TempValue)
	{
		Out_Value = *TempValue;
		return true;
	}

	else
	{
		Out_Code = "No data found !";
		return false;
	}
}