#include <iostream>
#include <cassert>
#include <fstream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define NUMBER_OF_PORT         5
#define RX_BUFFER_SIZE         256

#define CRC_POLYNOMIAL         0x04C11DB7


using namespace std;

string select_opened_port();
int open_serial_port(string);
void write_serial_port(int port, uint8_t *tx_buffer, int length);
uint32_t calculate_crc(uint8_t* tx_buffer, int length);

int main() {

    string opened_port = select_opened_port();
    cout << opened_port << endl;

    int serial_port = open_serial_port(opened_port);

    //uint8_t tx_buf[] = {0x22, 0xFF, 0x01, 0xA5};
    uint8_t tx_buf[] = {0x05, 0x51};

    write_serial_port(serial_port, tx_buf, sizeof(tx_buf));

    // HEX veri al
    unsigned char rx_buf[RX_BUFFER_SIZE];
    int n = read(serial_port, rx_buf, sizeof(rx_buf));
    if (n > 0) {
        std::cout << "Gelen: ";
        for (int i = 0; i < n; i++) {
            printf("0x%02X ", rx_buf[i]);
        }
        std::cout << std::endl;
    }

    close(serial_port);
    return 0;
}

string select_opened_port() {
    system("ls /dev/cu.* > port_list");
    int opened_port = 0;
    string ports[NUMBER_OF_PORT] = {};
    ifstream port_list;
    port_list.open("port_list");
    assert(port_list.is_open());
    for (int i = 0; i < NUMBER_OF_PORT; i++) {
        port_list >> ports[i];
        if (ports[i].empty()) {
            continue;
        }
        cout << i + 1 << ". " << ports[i] << endl;
    }
    cout << "Select the port will opened: " << endl;
    string port = {};
    cin >> opened_port;
    opened_port--;
    port = ports[opened_port];
    while (port.empty()) {
        port = select_opened_port();
    }
    port_list.close();
    return port;
}

int open_serial_port(string port_name) {
    int serial_port = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    cout << "open() çağrısı tamamlandı" << endl;

    if (serial_port < 0) {
        perror("Seri port açılamadı");
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    tcgetattr(serial_port, &tty);

    cfmakeraw(&tty);
    cfsetispeed(&tty, B38400);
    cfsetospeed(&tty, B38400);
    tty.c_cflag |= CREAD | CLOCAL;
    tcsetattr(serial_port, TCSANOW, &tty);

    return serial_port;

}
void write_serial_port(int port, uint8_t* tx_buffer, int length) {
    //crc işlemleri eklenecek burada ardından buffer yazılacak
    int crc_index = 0;
    uint32_t crc = 0;

    crc = calculate_crc(tx_buffer, length);

    crc_index = sizeof(tx_buffer) - 5;
    cout << crc_index << endl;

    //write(port, tx_buffer, length);
}

#include <cstdint>
#include <iostream>

#define CRC_POLYNOMIAL 0x04C11DB7

uint32_t calculate_crc(uint8_t* tx_buffer, int length) {
    uint32_t crc = 0xFFFFFFFF;

    for (int i = 0; i < length; i++) {
        crc ^= (uint32_t)(tx_buffer[i]) << 24;  // MSB-first

        for (int j = 0; j < 8; j++) {  // ✅ her byte için 8 bit shift
            if (crc & 0x80000000)
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            else
                crc <<= 1;
        }
        crc &= 0xFFFFFFFF;  // 32-bit sınırında tutmak için
    }

    std::cout << "Calculated CRC = 0x" << std::hex << crc << std::endl;

    return crc;
}


