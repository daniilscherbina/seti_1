#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include "../util.h"

int main()
{
    Connection connection;

    std::cout << "your login (int): ";
    std::cin >> connection.id;
    std::cout << "your password (char[8]): ";
    std::cin >> connection.pass;

    connection.type = CONNECT;
    connection.status = WAITING_REG;

    std::ofstream connect("con", FILE_OPEN);
    connect.seekp(END_FILE);
    int c = connect.tellp();
    connect.write((char*) &connection, sizeof(connection));
    connect.close();
    
    bool wait = true;
    while (wait)
    {
        std::ifstream connectI("con", std::ios::binary);
        connectI.seekg(c);
        connectI.read((char*) &connection, sizeof(connection));
        connectI.close();

        if (connection.status != WAITING_REG)
        {
            std::cout << connection.status << std::endl;
            if (connection.status == REG_FAIL)
            {
                std::cerr << "the session of this account has not been completed yet";
            }
            if (connection.status == REG_FAIL_LOGiN)
            {
                std::cerr << "invalid username or password";
            }
            wait = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}