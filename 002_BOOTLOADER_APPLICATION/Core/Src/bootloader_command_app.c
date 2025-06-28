#include "bootloader_command_app.h"

extern uint8_t supported_commands[];



void bootloader_get_version_cmd(uint8_t* bl_rx_data){

	uint8_t bl_version = 0;
	print_message("BL_DEBUG_MSG: Running bootloader_get_version_cmd\n");

	uint32_t command_packet_length = bl_rx_data[0] + 1;
	uint32_t host_crc = *((uint32_t*) (bl_rx_data + command_packet_length - 4));

	for (uint32_t i = 0; i < command_packet_length; i++) {
		HAL_UART_Transmit(&huart3, &bl_rx_data[i], 1, HAL_MAX_DELAY);
	}

	//HAL_UART_Transmit(&huart3, &host_crc, 4, HAL_MAX_DELAY);

	//crc kontrolu yapilacak
	if(!bootloader_verify_crc(&bl_rx_data[0], command_packet_length - 4, host_crc)){ //-4 eklenmeden önce de çalışıyordu
		print_message("BL_DEBUG_MSG: Checksum success\n");
		bootloader_send_ack(bl_rx_data[0], 1);
		bl_version = bootloader_get_version();
		print_message("BL_DEBUG_MSG: BL_VER: %d\n", bl_version);
		bootloader_uart_write_data(&bl_version, 1);

	}
	else{
		print_message("BL_DEBUG_MSG: Checksum fail\n");
		bootloader_send_nack();
	}

}

void bootloader_get_help_cmd(uint8_t* bl_rx_data){
	print_message("BL_DEBUG_MSG: Running bootloader_get_help_cmd\n");
	uint32_t command_packet_len = bl_rx_data[0] + 1;
	uint32_t host_crc = *((uint32_t*) bl_rx_data + command_packet_len - 4);
	if(!bootloader_verify_crc(&bl_rx_data[0], command_packet_len - 4, host_crc)){
		print_message("BL_DEBUG_MSG: Checksum success\n");
		bootloader_send_ack(bl_rx_data[0], strlen(supported_commands));
		bootloader_uart_write_data(supported_commands, strlen(supported_commands));

	}
	else{
		print_message("BL_DEBUG_MSG: Checksum fail\n");
		bootloader_send_nack();


	}


}


uint8_t bootloader_verify_crc(uint8_t *buffer, uint32_t len, uint32_t crc){

	uint32_t crc_value = 0xFF;
	//uint32_t data = 0;
	//HAL_UART_Transmit(&huart3, &crc, 4, HAL_MAX_DELAY);
	__HAL_CRC_DR_RESET(&hcrc);

    uint8_t data[2] = {0x05, 0x51};
    uint32_t data32;

    // Byte dizisini 32-bit hizalamak için padding yapılır:
    data32 =  (uint32_t)data[0] |
             ((uint32_t)data[1] << 8);  // 0x00005105 olur (Little Endian)

    __HAL_CRC_DR_RESET(&hcrc);  // CRC birimini sıfırla
    crc_value = HAL_CRC_Accumulate(&hcrc, &data32, 1);  // 1 x 32-bit word

    // Sonucu UART üzerinden gönderelim:
    HAL_UART_Transmit(&huart3, (uint8_t*)&crc_value, 4, HAL_MAX_DELAY);
    uint8_t ch = 'H';
    HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY);



	if(crc_value == crc){
		  uint8_t ch = 'P';
		    HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY);
		return CRC_SUCCESS;
	}

	return CRC_FAIL;

}


void bootloader_send_ack(uint8_t command_code, uint8_t follow_length){
	uint8_t ack_buffer[2];
	ack_buffer[0] = BL_ACK_VALUE;
	ack_buffer[1] = follow_length;

	HAL_UART_Transmit(&huart3, ack_buffer, 2, HAL_MAX_DELAY);
}


void bootloader_send_nack(){
	uint8_t nack_value = BL_NACK_VALUE;
	HAL_UART_Transmit(&huart3, &nack_value, 1, HAL_MAX_DELAY);

}

void bootloader_uart_write_data(uint8_t *buffer, uint32_t len){
	HAL_UART_Transmit(&huart3, buffer, len, HAL_MAX_DELAY);
}

uint8_t bootloader_get_version(void){
	return BL_VER;
}

