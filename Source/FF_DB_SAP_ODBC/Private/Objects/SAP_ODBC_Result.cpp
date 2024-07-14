#include "Objects/SAP_ODBC_Result.h"

bool USAP_ODBC_Result::SetQueryResult(FString& Out_Code, odbc::ResultSetRef In_Result)
{
	if (In_Result.isNull())
	{
		return false;
	}

	this->QueryResult = In_Result;
	this->Count_Column = In_Result->getMetaData()->getColumnCount();

	return true;
}

bool USAP_ODBC_Result::Result_Record(FString& Out_Code)
{
	int32 Index_Row = 0;
	TMap<FVector2D, FSAP_ODBC_DataValue> Temp_Data;

	try
	{
		const int32 ColumnCount = this->QueryResult->getMetaData()->getColumnCount();

		while (this->QueryResult->next())
		{
			for (int32 Index_Column_Raw = 0; Index_Column_Raw < ColumnCount; Index_Column_Raw++)
			{
				const FVector2D Position = FVector2D(Index_Column_Raw, Index_Row);
				const int32 Index_Column = Index_Column_Raw + 1;

				const int32 DataType = this->QueryResult->getMetaData()->getColumnType(Index_Column);
				const FString DataTypeName = UTF8_TO_TCHAR(this->QueryResult->getMetaData()->getColumnTypeName(Index_Column).c_str());
				const FString ColumnName = UTF8_TO_TCHAR(this->QueryResult->getMetaData()->getColumnName(Index_Column).c_str());

				FSAP_ODBC_DataValue EachData;
				EachData.DataType = DataType;
				EachData.DataTypeName = DataTypeName;
				EachData.ColumnName = ColumnName;

				switch (DataType)
				{
				case -9:
				{
					// NVARCHAR & DATE & TIME

					odbc::String TempStringRaw = this->QueryResult->getString(Index_Column);

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

					odbc::Long TempLong = this->QueryResult->getLong(Index_Column);

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

					odbc::String RawString = this->QueryResult->getString(Index_Column);

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

					odbc::String TempStringRaw = this->QueryResult->getString(Index_Column);

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

					odbc::Int TempInt = this->QueryResult->getInt(Index_Column);

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

					odbc::Double TempDouble = this->QueryResult->getDouble(Index_Column);

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

					odbc::Timestamp TempDateTime = this->QueryResult->getTimestamp(Index_Column);

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

				Temp_Data.Add(Position, EachData);
			}

			Index_Row += 1;
		}
	}

	catch (const odbc::Exception& Exception)
	{
		Out_Code = Exception.what();
		return false;
	}

	this->Data_Pool = Temp_Data;
	this->Count_Row = Index_Row;

	return true;
}

bool USAP_ODBC_Result::Result_Fetch(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Values, int32 Index_Column)
{
	return false;
}

int32 USAP_ODBC_Result::GetColumnCount()
{
	return this->Count_Column;
}

int32 USAP_ODBC_Result::GetRowCount()
{
	return this->Count_Row;
}

bool USAP_ODBC_Result::GetMetaData(FString& Out_Code, FSAP_ODBC_MetaData& Out_MetaData, const int32 Index_Column)
{
	odbc::ResultSetMetaData* Result = nullptr;

	try
	{
		Result = this->QueryResult->getMetaData().get();
	}

	catch (const odbc::Exception& Exception)
	{
		Out_Code = (FString)"FF DB SAP ODBC : " + Exception.what();
		return false;
	}

	FSAP_ODBC_MetaData MetaData;
	MetaData.ColumnScale = Result->getScale(Index_Column);
	MetaData.ColumnPrecision = Result->getPrecision(Index_Column);

	MetaData.ColumnDisplaySize = Result->getColumnDisplaySize(Index_Column);
	MetaData.ColumnLenght = Result->getColumnLength(Index_Column);
	MetaData.ColumnOctetLenght = Result->getColumnOctetLength(Index_Column);

	MetaData.ColumnType = Result->getColumnType(Index_Column);
	MetaData.ColumnTypeName = UTF8_TO_TCHAR(Result->getColumnTypeName(Index_Column).c_str());

	MetaData.BaseColumnName = UTF8_TO_TCHAR(Result->getBaseColumnName(Index_Column).c_str());
	MetaData.ColumnName = UTF8_TO_TCHAR(Result->getColumnName(Index_Column).c_str());
	MetaData.ColumnLabel = UTF8_TO_TCHAR(Result->getColumnLabel(Index_Column).c_str());

	MetaData.BaseTableName = UTF8_TO_TCHAR(Result->getBaseTableName(Index_Column).c_str());
	MetaData.TableName = UTF8_TO_TCHAR(Result->getTableName(Index_Column).c_str());

	MetaData.CatalogName = UTF8_TO_TCHAR(Result->getCatalogName(Index_Column).c_str());

	MetaData.SchemaName = UTF8_TO_TCHAR(Result->getSchemaName(Index_Column).c_str());

	MetaData.bIsNullable = Result->isNullable(Index_Column);
	MetaData.bIsAutoIncrement = Result->isAutoIncrement(Index_Column);
	MetaData.bIsNamed = Result->isNamed(Index_Column);
	MetaData.bIsCaseSensitive = Result->isCaseSensitive(Index_Column);
	MetaData.bIsReadOnly = Result->isReadOnly(Index_Column);
	MetaData.bIsSearchable = Result->isSearchable(Index_Column);
	MetaData.bIsSigned = Result->isSigned(Index_Column);

	Out_MetaData = MetaData;

	return true;
}

bool USAP_ODBC_Result::GetRow(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const int32 Index_Row)
{
	if (this->Data_Pool.IsEmpty())
	{
		Out_Code = "Data pool is empty !";
		return false;
	}

	if (Index_Row < 0 || Index_Row >= this->Count_Row)
	{
		Out_Code = "Given row index is out of data pool's range !";
		return false;
	}

	try
	{
		TArray<FSAP_ODBC_DataValue> Temp_Array;

		for (int32 Index_Column = 0; Index_Column < this->Count_Column; Index_Column++)
		{
			const FVector2D Position = FVector2D(Index_Column, Index_Row);

			if (!this->Data_Pool.Contains(Position))
			{
				return false;
			}

			FSAP_ODBC_DataValue* EachData = this->Data_Pool.Find(Position);

			if (!EachData)
			{
				return false;
			}

			Temp_Array.Add(*EachData);
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

bool USAP_ODBC_Result::GetColumnFromIndex(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const int32 Index_Column)
{
	if (this->Data_Pool.IsEmpty())
	{
		Out_Code = "FF DB SAP ODBC : Data pool is empty !";
		return false;
	}

	if (Index_Column < 0 || Index_Column >= this->Count_Column)
	{
		Out_Code = "Given column index is out of data pool's range !";
		return false;
	}

	TArray<FSAP_ODBC_DataValue> Temp_Array;

	for (int32 Index_Row = 0; Index_Row < this->Count_Row; Index_Row++)
	{
		const FVector2D Position = FVector2D(Index_Column, Index_Row);

		if (!this->Data_Pool.Contains(Position))
		{
			return false;
		}

		FSAP_ODBC_DataValue* EachData = this->Data_Pool.Find(Position);

		if (!EachData)
		{
			return false;
		}

		Temp_Array.Add(*EachData);
	}

	Out_Value = Temp_Array;
	return true;
}

bool USAP_ODBC_Result::GetColumnFromName(FString& Out_Code, TArray<FSAP_ODBC_DataValue>& Out_Value, const FString ColumnName)
{
	if (this->Data_Pool.IsEmpty())
	{
		Out_Code = "FF DB SAP ODBC : Data pool is empty !";
		return false;
	}

	int32 TargetColumnIndex = 0;
	odbc::ResultSetMetaDataRef MetaDataRef = this->QueryResult->getMetaData();

	for (int32 Index_Column = 1; Index_Column < this->Count_Column + 1; Index_Column++)
	{
		FString EachColumnName = UTF8_TO_TCHAR(MetaDataRef->getColumnName(Index_Column).c_str());

		if (ColumnName == EachColumnName)
		{
			TargetColumnIndex = Index_Column;
			break;
		}
	}

	TArray<FSAP_ODBC_DataValue> Temp_Array;

	for (int32 Index_Row = 0; Index_Row < this->Count_Row; Index_Row++)
	{
		const FVector2D Position = FVector2D(TargetColumnIndex, Index_Row);

		if (!this->Data_Pool.Contains(Position))
		{
			return false;
		}

		FSAP_ODBC_DataValue* EachData = this->Data_Pool.Find(Position);

		if (!EachData)
		{
			return false;
		}

		Temp_Array.Add(*EachData);
	}

	Out_Value = Temp_Array;
	return true;
}

bool USAP_ODBC_Result::GetSingleData(FString& Out_Code, FSAP_ODBC_DataValue& Out_Value, const FVector2D Position)
{
	if (this->Data_Pool.IsEmpty())
	{
		Out_Code = "FF DB SAP ODBC : Data pool is empty !";
		return false;
	}

	if (Position.X < 0 || Position.Y < 0 || Position.X >= this->Count_Column || Position.Y >= this->Count_Row)
	{
		Out_Code = "FF DB SAP ODBC : Given position is out of data pool's range !";
		return false;
	}

	FSAP_ODBC_DataValue* TempValue = this->Data_Pool.Find(Position);

	if (!TempValue)
	{
		Out_Code = "FF DB SAP ODBC : Found data is invalid !";
		return false;
	}

	Out_Value = *TempValue;
	return true;
}