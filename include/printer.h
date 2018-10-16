#include "kernel.h"

#ifndef __PRINTER_H
#define __PRINTER_H

void    WHPrinterInit(void);
void    WHPrinterNormalCmd(void);
void    WHPrinterFreeRow(uint8_t rows);
void    WHPrinterHead(void);
uint8_t WHPrintReady(void);
void    WHPrintf(char *fmt, ...);
void 	WHPrintfFlash(char __farflash*fmt, ...);
void    WHPrintfString(char __farflash* str);

void    WHFuelRecord(void);
void    uart0PutString(char* string);


//´òÓ¡Á÷Ë®¡£
void Printid(void);
void StopPrint(void);

void WHFuelShifRecord(void);
void PrintShifid(void);

void PrintSum(void);
void WHFuelSumRecord(void);
void WHFuelSysInfo(void);
void PrintSysInfo(void);
void PrintContInfo(void);


extern  uint8_t stopreasonbuf[][16];

#endif

