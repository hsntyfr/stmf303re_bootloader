#include <iostream>
#include <cassert>
#include <fstream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define NUMBER_OF_PORT         5
#define RX_BUFFER_SIZE         256


using namespace std;

string select_opened_port();
int open_serial_port(string);
void write_serial_port(int port, unsigned char *tx_buffer, int length);

int main() {

    string opened_port = select_opened_port();
    cout << opened_port << endl;

    int serial_port = open_serial_port(opened_port);

    unsigned char tx_buf[] = { 0xA5, 0x01, 0xFF };
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
    system("ls /dev/tty.* > port_list");
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
void write_serial_port(int port, unsigned char *tx_buffer, int length) {
    //crc işlemleri eklenecek burada ardından buffer yazılacak
    write(port, &tx_buffer, length);

}


