#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <map>
#include <string>

#include "../util.h"

int main()
{
    std::cout << "Sever started..." << std::endl;

    // проверяем текущий размер файла с запросами на подключение
    std::ifstream connect("con", std::ios::binary);
    connect.seekg(END_FILE);
    // переменная для запоминания размера файла с подключениями
    int con_size = connect.tellg();
    connect.close();

    std::map<int, int> base;

    Connection connection;

    // пременная для отслеживания наверстывания обновлений в файлах
    int sizer;
    while (true)
    {
        // проверяем новые подключения
        connect.open("con", std::ios::binary);
        connect.seekg(END_FILE);
        sizer = connect.tellg();
        // получили размер обновлений на текущий момент времени 
        // закрываем поток чтобы у клиентов была возможность в него писать
        connect.close();
        std::cout << con_size << " " << sizer << std::endl;
        while (con_size < sizer)
        {
            connect.open("con", std::ios::binary);
            // поэтапно отвечаем на запросы о подключении
            connect.seekg(con_size);
            connect.read((char*) &connection, sizeof(connection));
            std::cout << "request to " << connection.id << std::endl;
            connect.close();
            if (base[connection.id] == 0)
            {
                base[connection.id]++;
                connection.status = REG_OK;
                std::cout << "connecting to " << connection.id << std::endl;
            }
            else
            {
                connection.status = REG_FAIL;
            }
            std::cout << connection.id << " " << connection.type << " " << connection.status << std::endl;
            // создаем поток для ответа клиенту на запрос о подключении
            // т.к. нам нужно перезаписать часть файла используем флаг std::ios::out
            std::ofstream connect_answer("con", std::ios::out | std::ios::binary);
            connect_answer.seekp(con_size);
            connect_answer.write((char*) &connection, sizeof(connection));
            connect_answer.close();
            con_size += sizeof(connection);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}