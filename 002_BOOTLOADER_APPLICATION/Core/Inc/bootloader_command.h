#ifndef __BOOTLOADER_COMMAND_H
#define __BOOTLOADER_COMMAND_H


#include "main.h"

#define BL_GET_VER   		    	0x51
#define BL_GET_HELP  		        0x52
#define BL_GET_CIP                  0x53
#define BL_GET_RDP_STATUS           0x54
#define BL_GO_TO_ADDR               0x55
#define BL_FLASH_ERASE              0x56
#define BL_MEM_WRITE                0x57
#define BL_EN_RW_PROTECT            0x58
#define BL_MEM_READ                 0x59
#define BL_READ_SECTOR_P_STATUS     0x5A
#define BL_OTP_READ                 0x5B
#define BL_DIS_R_W_PROTECT          0x5C

#define BL_VER 						0x10  //1.0

#define CRC_FAIL					1
#define CRC_SUCCESS					0
#define BL_NACK_VALUE				0x7F
#define BL_ACK_VALUE				0xA5





void bl_get_ver_cmd(uint8_t* bl_rx_data);
void bootloader_send_ack(uint8_t command_code, uint8_t follow_length);
void bootloader_send_nack();


uint8_t bootloader_verify_crc(uint8_t* buffer, uint32_t length, uint32_t crc_host);






#endif /*__BOOTLOADER_COMMAND_H*/
