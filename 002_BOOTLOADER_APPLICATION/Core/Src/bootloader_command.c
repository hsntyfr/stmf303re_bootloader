#include "bootloader_command.h"

void bl_get_ver_cmd(uint8_t* bl_rx_data){

	uint8_t bl_ver = 0;
	print_message("BL_DEBUG_MSG: Bootloader get ver running\n");
	uint32_t cmd_package_length = bl_rx_data[0] + 1;
	uint32_t host_crc = *((uint32_t*)bl_rx_data + cmd_package_length - 4);

	if(!bootloader_verify_crc(&bl_rx_data[0], cmd_package_length - 4, host_crc)){		//0 ise crc doğru
		print_message("BL DEBUG MSG: Checksum success\n");
		bootloader_send_ack(bl_rx_data[0], 1);


	}

}



uint8_t bootloader_verify_crc(uint8_t* buffer, uint32_t length, uint32_t crc_host){
	uint32_t crc_value = 0xff;
	uint32_t data = 0;
	for(uint32_t i = 0; i < length; i++){
		data = buffer[i];
		crc_value = HAL_CRC_Accumulate(&hcrc, &data, 1);
	}

	__HAL_CRC_DR_RESET(&hcrc);
	if(crc_value == crc_host){
		return CRC_SUCCESS;
	}
	else{
		return CRC_FAIL;
	}


	return 0;
}


void bootloader_send_ack(uint8_t command_code, uint8_t follow_length){

}


void bootloader_send_nack(){
	uint8_t nack_value = BL_NACK_VALUE;

}

