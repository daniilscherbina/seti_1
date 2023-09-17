#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <map>
#include <string>

#include "../util.h"

int main()
{
    int n;
    while (true)
    {
        std::cout << "-----------" << std::endl;
        std::cout << "0 - exit" << std::endl;
        std::cout << "1 - connections list" << std::endl;
        std::cout << "-----------" << std::endl;
        std::cin >> n;
        if (n == 0)
        {
            break;
        }
        if (n == 1)
        {
            std::ifstream con("con", std::ios::binary);
            con.seekg(END_FILE);
            int pos = 0;
            int size = con.tellg();
            
            std::string type = "NULL";
            std::string status = "NULL";
            Connection connecting;

            while(pos < size)
            {
                con.seekg(pos);
                con.read((char*) &connecting, sizeof(connecting));
                if (connecting.type == CONNECT) type = "CONNECT";
                else type = "DISCONNECT";
                if (connecting.status == WAITING_REG) status = "WAITING";
                else if (connecting.status == REG_OK) status = "LOGIN";
                else if (connecting.status == REG_FAIL_LOGiN) status = "FAIL_LOGIN";
                else if (connecting.status == REG_FAIL) status = "FAIL";
                std::cout << connecting.id << " " << connecting.pass << " " << type << " " << status << " " << std::endl;
                pos += sizeof(connecting);
            }
        }
    }
}