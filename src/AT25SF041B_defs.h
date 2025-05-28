#ifndef AT25SF041B_DEFS_H
#define AT25SF041B_DEFS_H

#define FLASH_MAX_ADDRESS 0x7FFFF 

// https://www.mouser.com/datasheet/2/590/at25sf041b-1888127.pdf

#define GET_FLASH_ID 0x9F

#define MANUFACTURER_ID 0x1F    //  ADESTO
#define DEVICE_ID1 0x84         //  AT25SFxxx series,  4 Mbit
#define DEVICE_ID2 0x01         //  

#define READ_STATUS_REG1            0x05
// SRP0 | BP4 | BP3 | BP2 | BP1 | BP0 | WEL (Write Enabled) | BUSY
#define BUSY_STATUS_FLAG            0x01
#define WRITE_ENABLE_STATUS_FLAG    0x02 // WEL (Write Enabled) bit in status register


#define WRITE_ENABLE        0x06
#define BYTE_PAGE_PROGRAM   0x02
#define READ_ARRAY          0x03

#define ERASE_4KB           0x20
#define ERASE_32KB          0x52
#define ERASE_64KB          0xD8
#define CHIP_ERASE          0xC7

#endif // AT25SF041B_DEFS_H