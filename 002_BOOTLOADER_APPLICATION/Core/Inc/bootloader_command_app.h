#ifndef __BOOTLOADER_COMMAND_APP_H
#define __BOOTLOADER_COMMAND_APP_H

#include "main.h"

#define BL_VER 						0x10  //1.0

#define CRC_FAIL					1
#define CRC_SUCCESS					0
#define BL_NACK_VALUE				0x7F
#define BL_ACK_VALUE				0xA5

void bootloader_get_version_cmd(uint8_t* bl_rx_data);
void bootloader_get_help_cmd(uint8_t* bl_rx_data);

uint8_t bootloader_verify_crc(uint8_t *buffer, uint32_t len, uint32_t crc);
void bootloader_send_ack(uint8_t command_code, uint8_t follow_length);
void bootloader_send_nack();
void bootloader_uart_write_data(uint8_t *buffer, uint32_t len);
uint8_t bootloader_get_version(void);





#endif /*__BOOTLOADER_COMMAND_APP_H*/
