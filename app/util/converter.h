#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#include "c_types.h"

/******************************************************************************
 * FunctionName : hex2bin.
 * Description  : convert. string to bin.
 * Parameters   : dst --
 * 				  hex_str --
 * Returns      : none.
*******************************************************************************/

void hex2bin(uint8 * dst, char * hex_str);

const char *u32_to_str(unsigned int val);
unsigned long HextoDec(const unsigned char *hex, int length);
unsigned int strbuf_to_u32(char str[],  int len);
unsigned char BCDtoHEX(unsigned char bcd);
int DectoBCD(int Dec, unsigned char *Bcd, int length);

#endif
