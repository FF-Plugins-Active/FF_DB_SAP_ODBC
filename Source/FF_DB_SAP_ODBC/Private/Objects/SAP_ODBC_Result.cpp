#include "Objects/SAP_ODBC_Result.h"

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
						EachData.Preview = FString::FromInt(*TempInt);
					}
				}

				if (DataType == 6)
				{
					odbc::Double TempDouble = ResultReferance->getDouble(Index_Column);

					if (!TempDouble.isNull())
					{
						EachData.ValDouble = *TempDouble;
						EachData.Preview = FString::SanitizeFloat(*TempDouble);
					}
				}

				// MSSQL's "time" dateType is an nvarchar, too.
				if (DataType == -9 || DataType == -1)
				{
					odbc::String TempStringRaw = ResultReferance->getString(Index_Column);

					if (!TempStringRaw.isNull())
					{
						FString TempString = UTF8_TO_TCHAR(TempStringRaw->c_str());
						EachData.ValString = TempString;
						EachData.Preview = TempString;
					}
				}

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