#ifndef _PARSE_TDT_H_
#define _PARSE_TDT_H_

typedef struct TS_TDT_T
{
	unsigned int uiTable_id :8;
	unsigned int uiSection_syntax_indicator :1;
	unsigned int uiReserved_future_use :1;
	unsigned int uiReserved :2;
	unsigned int uiSection_length :12;
	unsigned int auiUTC_time[5];
} TS_TDT_T;

int ParseTDT_Table(FILE *pfTsFile, int iTsPosition, int iTsLength, TS_TDT_T *pstTS_TDT);
#endif
