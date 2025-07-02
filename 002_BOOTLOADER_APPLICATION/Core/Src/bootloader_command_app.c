#include "bootloader_command_app.h"

extern uint8_t supported_commands[];



void bootloader_get_version_cmd(uint8_t* bl_rx_data){

	uint8_t bl_version = 0;
	print_message("BL_DEBUG_MSG: Running bootloader_get_version_cmd\n");

	uint32_t command_packet_length = bl_rx_data[0] + 1;
	uint32_t host_crc = *((uint32_t*) (bl_rx_data + command_packet_length - 4));

	for (uint32_t i = 0; i < command_packet_length; i++) {
		//HAL_UART_Transmit(&huart3, &bl_rx_data[i], 1, HAL_MAX_DELAY);
	}

	//HAL_UART_Transmit(&huart3, &host_crc, 4, HAL_MAX_DELAY);

	//crc kontrolu yapilacak
	if(!bootloader_verify_crc(&bl_rx_data[0], command_packet_length - 4, host_crc)){ //-4 eklenmeden önce de çalışıyordu
		print_message("BL_DEBUG_MSG: Checksum success\n");
		bootloader_send_ack(bl_rx_data[0], 1);
		bl_version = bootloader_get_version();
		print_message("BL_DEBUG_MSG: BL_VER: %d\n", bl_version);
		//bootloader_uart_write_data(&bl_version, 1);

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


uint8_t bootloader_verify_crc(uint8_t *Buffer, uint32_t len, uint32_t crcHost)
{
    uint32_t crcValue = 0xFF;
    uint32_t data = 0;

    __HAL_CRC_DR_RESET(&hcrc);  // Reset başa alındı, daha doğru

    for(uint32_t i = 0; i < len; i++)
    {
        data = Buffer[i];
        //HAL_UART_Transmit(&huart3, &Buffer[i], 1, HAL_MAX_DELAY);
        crcValue = HAL_CRC_Accumulate(&hcrc, &data, 1);
    }



    // Hesaplanan CRC'yi UART üzerinden gönder (4 byte olarak)
    HAL_UART_Transmit(&huart3, (uint8_t*)&crcValue, 4, HAL_MAX_DELAY);

    // EOL karakteri istersen okunabilirlik için ekle
    uint8_t newline[] = "\r\n";
    //HAL_UART_Transmit(&huart3, newline, 2, HAL_MAX_DELAY);

    // Son olarak CRC karşılaştırması
    if(crcValue == crcHost)
    {
        uint8_t de = 'a';
        HAL_UART_Transmit(&huart3, &de, 1, HAL_MAX_DELAY);
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

