#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include "../util.h"

int main()
{
    Connection connection;

    // формируем запрос на подключение
    std::cout << "your login (int): ";
    std::cin >> connection.id;
    std::cout << "your password (char[8]): ";
    std::cin >> connection.pass;
    connection.type = CONNECT;
    connection.status = WAITING_REG;

    // отправляем запрос на подключение
    std::ofstream connect("con", FILE_OPEN);
    connect.seekp(END_FILE);
    // запоминаем позицию куда записали данные чтобы потом с этого места читать
    int c = connect.tellp();
    connect.write((char*) &connection, sizeof(connection));
    connect.close();
    
    // ожидаем ответа на запрос подключения
    bool wait = true;
    while (wait)
    {
        std::ifstream connectI("con", std::ios::binary);
        connectI.seekg(c);
        connectI.read((char*) &connection, sizeof(connection));
        connectI.close();

        // проверяем наличие ответа
        if (connection.status != WAITING_REG)
        {
            //std::cout << connection.status << std::endl;
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

    int n;
    while (true)
    {
        std::cout << "-----------" << std::endl;
        std::cout << "0 - exit" << std::endl;
        std::cout << "1 - request" << std::endl;
        std::cout << "-----------" << std::endl;
        std::cout << "num command: ";
        std::cin >> n;
        std::cout << "-----------" << std::endl;
        if (n == 0)
        {
            connection.type = DISCONNECT;
            connection.status = WAITING_REG;
            connect.open("con", FILE_OPEN);
            connect.seekp(END_FILE);
            connect.write((char*) &connection, sizeof(connection));
            connect.close();
            break;
        }
        if (n == 1)
        {
            // формируем запрос для сервера
            Request request;
            request.id = connection.id;
            std::cout << "name: ";
            std::cin >> request.person.name;
            std::cout << "one: ";
            std::cin >> request.person.one;
            std::cout << "two: ";
            std::cin >> request.person.two;
            std::cout << "three: ";
            std::cin >> request.person.three;
            std::cout << "four: ";
            std::cin >> request.person.four;
            // дабы при повторном запросе клиент не игнорировал ответ сервера
            request.answer.res = 0;
            std::ofstream req("req", FILE_OPEN);
            req.seekp(END_FILE);
            // запоминаем позицию куда записали данные чтобы потом с этого места читать
            c = req.tellp();
            req.write((char*) &request, sizeof(request));
            req.close();
            
            // ожидаем ответ
            wait = true;
            while(wait)
            {
                std::ifstream reqI("req", std::ios::binary);
                reqI.seekg(c);
                reqI.read((char*) &request, sizeof(request));
                reqI.close();
                //std::cout << request.id << " " << request.person.name << " " << request.person.one << " " << request.answer.res << std::endl;
                
                // проверяем наличие ответа
                if (request.answer.res != 0)
                {
                    std::cout << "-----------" << std::endl;
                    if (request.answer.res == DOLG)
                        std::cout << "u vas dolg" << std::endl;
                    if (request.answer.res == NO)
                        std::cout << "stependia ne predusmotrena" << std::endl;
                    if (request.answer.res == DEFAULT)
                        std::cout << "poluchaet obichuiu stependiu" << std::endl;
                    if (request.answer.res == POW)
                        std::cout << "poluchaet povichenuiu stependiu" << std::endl;
                    std::cout << "-----------" << std::endl;
                    wait = false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            } 
        }
    }

    return 0;
}