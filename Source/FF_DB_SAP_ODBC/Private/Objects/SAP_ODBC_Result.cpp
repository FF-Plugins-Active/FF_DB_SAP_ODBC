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

				switch (DataType)
				{
					case -9:
					{
						// NVARCHAR & DATE & TIME

						odbc::String TempStringRaw = ResultReferance->getString(Index_Column);

						if (!TempStringRaw.isNull())
						{
							FString TempString = UTF8_TO_TCHAR(TempStringRaw->c_str());
							EachData.String = TempString;
							EachData.Preview = TempString;
						}

						break;
					}

					case -5:
					{
						// INT64 & BIGINT

						odbc::Long TempLong = ResultReferance->getLong(Index_Column);

						if (!TempLong.isNull())
						{
							EachData.Integer64 = *TempLong;
							EachData.Preview = FString::FromInt(EachData.Integer64);
						}

						break;
					}

					case -2:
					{
						// TIMESTAMP: odbc::timestamp is not SQL timestamp. We use it to check if rows changed since last retriving or not.
						
						odbc::String RawString = ResultReferance->getString(Index_Column);
						
						if (!RawString.isNull())
						{
							std::string TimeStampString = RawString->c_str();
							unsigned int TimeStampInt = std::stoul(TimeStampString, nullptr, 16);

							EachData.Integer64 = TimeStampInt;
							EachData.Preview = (FString)TimeStampString.c_str() + " - " + FString::FromInt(TimeStampInt);
						}
						
						break;
					}

					case -1:
					{
						// TEXT

						odbc::String TempStringRaw = ResultReferance->getString(Index_Column);

						if (!TempStringRaw.isNull())
						{
							FString TempString = UTF8_TO_TCHAR(TempStringRaw->c_str());
							EachData.String = TempString;
							EachData.Preview = TempString;
						}

						break;
					}

					case 4:
					{
						// INT32

						odbc::Int TempInt = ResultReferance->getInt(Index_Column);

						if (!TempInt.isNull())
						{
							EachData.Integer32 = *TempInt;
							EachData.Preview = FString::FromInt(*TempInt);
						}

						break;
					}

					case 6:
					{
						// FLOAT & DOUBLE

						odbc::Double TempDouble = ResultReferance->getDouble(Index_Column);

						if (!TempDouble.isNull())
						{
							EachData.Double = *TempDouble;
							EachData.Preview = FString::SanitizeFloat(*TempDouble);
						}

						break;
					}

					case 9:
					{
						// DATETIME : SAP ODBC gives "9" as datatype of DateTime.

						odbc::Timestamp TempDateTime = ResultReferance->getTimestamp(Index_Column);

						if (!TempDateTime.isNull())
						{
							int32 Year = TempDateTime->year();
							int32 Month = TempDateTime->month();
							int32 Day = TempDateTime->day();
							int32 Hours = TempDateTime->hour();
							int32 Minutes = TempDateTime->minute();
							int32 Seconds = TempDateTime->second();
							int32 Milliseconds = TempDateTime->milliseconds();

							EachData.DateTime = FDateTime(Year, Month, Day, Hours, Minutes, Seconds, Milliseconds);
							EachData.Preview = FString::Printf(TEXT("%d-%d-%d %d:%d:%d:%d"), Year, Month, Day, Hours, Minutes, Seconds, Milliseconds);
						}

						break;
					}

					default:
					{
						EachData.Note = "Currently there is no parser for this data type. Please convert it to another known type in your query !";
						break;
					}
				}

				Temp_Data.Add(FVector2D(Index_Row, Index_Column_Raw), EachData);
			}

			Index_Row += 1;
		}
	}

	catch (const odbc::Exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	this->QueryResult = ResultReferance;
	this->All_Data = Temp_Data;
	this->RowCount = Index_Row;

	return true;
}

int32 USAP_ODBC_Result::GetColumnCount()
{
	if (this->QueryResult.isNull())
	{
		return 0;
	}

	return this->QueryResult->getMetaData()->getColumnCount();
}

int32 USAP_ODBC_Result::GetRowCount(int32& LastIndex)
{
	if (this->QueryResult.isNull())
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
		Result = this->QueryResult->getMetaData().get();
	}

	catch (const odbc::Exception& Exception)
	{
		Out_Code = (FString)"FF SAP ODBC : " + Exception.what();
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
		const int32 ColumnCount = this->QueryResult->getMetaData()->getColumnCount();
		TArray<FSAP_ODBC_DataValue> Temp_Array;

		for (int32 Index_Column = 0; Index_Column < ColumnCount; Index_Column++)
		{
			Temp_Array.Add(*this->All_Data.Find(FVector2D(RowIndex, Index_Column)));
		}

		Out_Value = Temp_Array;
	}

	catch (const odbc::Exception& Exception)
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
		Out_Code = "FF SAP ODBC : Data pool is empty !";
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
		Out_Code = "FF SAP ODBC : Data pool is empty !";
		return false;
	}

	int32 TargetColumnIndex = 0;
	odbc::ResultSetMetaDataRef MetaDataRef = this->QueryResult->getMetaData();
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
		Out_Code = "FF SAP ODBC : Data pool is empty !";
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
		Out_Code = "FF SAP ODBC : No data found !";
		return false;
	}
}