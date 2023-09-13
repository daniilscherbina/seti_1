#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include "../util.h"

int main()
{
    std::cout << "your system id (int): ";
    int id;
    std::cin >> id;
    bool run = 1;

    Connection connection;
    connection.id = id;
    connection.type = CONNECT;
    connection.status = WAITING_REG;
    std::ofstream connect("con", std::ios::binary);
    connect.seekp(0, std::ios::end);
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
        std::cout << connection.status;
        if (connection.status != WAITING_REG)
        {
            if (connection.status == REG_FAIL)
            {
                run = 0;
            }
            wait = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::ifstream answer(std::to_string(id), std::ios::binary | std::ios::app);

    int answer_size = answer.tellg();
    answer.close();
    Request req;
    req.id = id;
    Answer ans;
    int n;
    while (run)
    {
        std::cout << "---------" << std::endl;
        std::cout << "0 - exit" << std::endl;
        std::cout << "1 - request" << std::endl;
        std::cout << "---------" << std::endl;
        std::cin >> n;
        if (n == 0) 
        {
            run = 0;
            connection.type = DISCONNECT;
            std::ofstream connect("con", std::ios::binary | std::ios::app);
            connect.write((char*) &connection, sizeof(connection));
            connect.close();
            break;
        }
        if (n == 1)
        {
            std::cout << "name: ";
            std::cin >> req.person.name;
            std::cout << "weight: ";
            std::cin >> req.person.weight;
            std::cout << "height: ";
            std::cin >> req.person.height;
            std::ofstream request("request", std::ios::binary | std::ios::app);
            request.write((char*) &req, sizeof(req));
            request.close();

            wait = true;
            while (wait)
            {
                answer.open(std::to_string(id), std::ios::binary);
                answer.seekg(0, std::ios::end);
                if (answer_size < answer.tellg())
                {
                    int temp = answer.tellg();
                    answer.seekg(answer_size);
                    answer.read((char*) &ans, sizeof(ans));
                    answer_size += sizeof(ans);
                    wait = false;
                    std::cout << "--------" << std::endl;
                    std::cout << "name: " << req.person.name << "answer: " << ans.res << std::endl;
                }
                answer.close();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

        }
    }

    return 0;
}