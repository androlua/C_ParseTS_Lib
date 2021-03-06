#include <stdio.h>
#include <string.h>

#include "Parse_ST.h"
#include "TsParser.h"
#include "Get_Section.h"

#define ST_PID 0x0013
#define ST_TABLE_ID 0x71
#define INITIAL_VERSION 0xff
#define SECTION_COUNT_256 256
#define SECTION_MAX_LENGTH_4092 1024 * 4

/******************************************
 *
 *解析ST段信息
 *
 ******************************************/

void ParseST_Section(TS_ST_T *pstTS_ST, unsigned char *pucSectionBuffer)
{
	pstTS_ST->uiTable_id = pucSectionBuffer[0];
	pstTS_ST->uiSection_syntax_indicator = pucSectionBuffer[1] >> 7;
	pstTS_ST->uiReserved_future_use = (pucSectionBuffer[1] >> 6) & 0x01;
	pstTS_ST->uiReserved = (pucSectionBuffer[1] >> 4) & 0x03;
	pstTS_ST->uiSection_length = ((pucSectionBuffer[1] & 0x0f) << 8) | pucSectionBuffer[2];
	if (pstTS_ST->uiSection_length > 0)
	{
		memcpy(pstTS_ST->aucST_data, pucSectionBuffer + 3, pstTS_ST->uiSection_length);
	}
}

/******************************************
 *
 *输出ST信息
 *
 ******************************************/
void PrintST(TS_ST_T *pstTS_ST)
{
	DUBUGPRINTF("\n-------------ST info start-------------\n");
	DUBUGPRINTF("ST->table_id: %02x\n", pstTS_ST->uiTable_id);
	DUBUGPRINTF("ST->uiSection_syntax_indicator: %02x\n", pstTS_ST->uiSection_syntax_indicator);
	DUBUGPRINTF("ST->uiReserved_future_use_fiST: %02x\n", pstTS_ST->uiReserved_future_use);
	DUBUGPRINTF("ST->uiReserved: %02x\n", pstTS_ST->uiReserved);
	DUBUGPRINTF("ST->uiSection_length: %02x\n", pstTS_ST->uiSection_length);
	if (pstTS_ST->uiSection_length > 0)
	{
		DUBUGPRINTF("ST->ST_data: %s\n", pstTS_ST->aucST_data);
	}
	DUBUGPRINTF("\n-------------ST info end-------------\n");
}

/******************************************
 *
 *从流中解析ST信息
 *
 ******************************************/
int ParseST_Table(FILE *pfTsFile, int iTsPosition, int iTsLength, TS_ST_T *pstTS_ST)
{
	DUBUGPRINTF("\n\n=================================ParseST_Table Start================================= \n");
	int iTemp = 0;
	unsigned int uiVersion = INITIAL_VERSION;
	unsigned char ucSectionBuffer[SECTION_MAX_LENGTH_4092] = { 0 };
	unsigned int uiRecordGetSection[SECTION_COUNT_256] = { 0 };

	if (-1 == fseek(pfTsFile, iTsPosition, SEEK_SET))
	{
		DUBUGPRINTF("Parse ST error\n");
		return -1;
	}
	while (!feof(pfTsFile))
	{
		iTemp = GetOneSection(pfTsFile, iTsLength, ucSectionBuffer, ST_PID, ST_TABLE_ID, &uiVersion);
		switch (iTemp)
		{
			case 0:
				uiVersion = INITIAL_VERSION;
				memset(uiRecordGetSection, 0, sizeof(char) * SECTION_COUNT_256);
				fseek(pfTsFile, 0 - iTsLength, SEEK_CUR);
				break;
			case 1:
				if (0 == IsSectionGetBefore(ucSectionBuffer, uiRecordGetSection))
				{
					ParseST_Section(pstTS_ST, ucSectionBuffer);
					if (1 == PRINTFST_INFO)
					{
						PrintST(pstTS_ST);
					}
					return 1;
				}
				if (1 == IsAllSectionOver(ucSectionBuffer, uiRecordGetSection))
				{
					DUBUGPRINTF("\n=================================ParseST_Table END=================================== \n\n");
					return 1;
				}
				break;
			case -1:
				DUBUGPRINTF("\n\n=================================ParseST_Table End================================= \n");
				return -1;
				break;
			default:
				LOG("ParseST_Table switch (iTemp) default\n");
				break;
		}
	}

	DUBUGPRINTF("\n\n=================================ParseST_Table End================================= \n");
	return -1;
}

