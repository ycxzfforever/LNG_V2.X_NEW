#ifndef __FYD12864_H
#define __FYD12864_H

void FYD12864Init(void);
void FYD12864ClrScreen(void);
void FYD12864ClearLine(uint8_t line);
void FYD12864DispPrintf(uint8_t posx, unsigned posy, char * fmt, ...);   //живЊ
void FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...);


void FYD12864Locate(uint8_t row, uint8_t col);
void FYD12864WrtStr(uint8_t row, uint8_t col, char *str, char bClrLeft);
void FYD12864DispString(uint8_t row, uint8_t col, char __farflash* str, bool bClrLeft);
void FYD12864Inputchar(uint8_t row, uint8_t  col, char tmp);


void FYD12864NarrowUp(uint8_t px, uint8_t py);
void FYD12864NarrowDown(uint8_t px, uint8_t py);
void FYD12864SelectFlag(uint8_t px, uint8_t py);
void FYD12864ClearSelectFlag(uint8_t px, uint8_t py);
void ZHYEpsErrorDisplay(uint8_t row, uint8_t col, char *str);
void FYD12864WrtData(char Data, char bCmd);
void FYD12864ShowGDRAM(uint8_t* address);


#endif
