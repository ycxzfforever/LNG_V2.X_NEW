#ifndef __E28F128FLASH_H
#define __E28F128FLASH_H
void OS_ENTER_CRITICAL(void);
void OS_EXIT_CRITICAL(void);

void JS28F128FlashInit(void);
bool JS28F128FlashCheckId(void);
bool JS28F128FlshEraseBlock(uint32_t ulAddr);
bool JS28F128FlashWriteChar(uint32_t ulAddr, uint8_t ucData);
uint8_t JS28F128FlashReadChar(uint32_t ulAddr);
void JS28F128FlashReadBuf(uint32_t rdAddr, uint32_t Cnt, uint8_t *buf);
bool JS28F128FlashWriteBuf(uint32_t wtAddr, uint32_t Cnt, uint8_t *buf);

void JS28F128FlashEraseAll(void);   //changed by ly 20180822

bool JS28F128FlashWriteBuffer(uint32_t ulAddr, uint32_t Cnt, uint8_t *buf);

uint8_t SaveFuelRecord(uint8_t select);   //保存流水/班流水到flash
uint8_t ReadFuelRecord(uint32_t id, uint8_t select);   //从flash中读出流水/班流水

#endif