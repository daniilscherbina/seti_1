#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <map>
#include <string>

#include "../util.h"

// структура описывающая элемент из БД с пользователями
struct BaseEntity {
    int id;
    char pass[8];
};

// функция для сравнения паролей, встроенные функции работают 50/50 (по идее зависит от компилятора)
bool compare_pass (char a[], char b[])
{
    for (int i = 0; i < 8; i++)
    {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// функция подсчета ответа
int get_answer(int one, int two, int three, int four)
{
    if (one == 2 || two == 2 || three == 2 || four == 2) return DOLG;
    if (one == 3 || two == 3 || three == 3 || four == 3) return NO;
    if (one == 4 || two == 4 || three == 4 || four == 4) return DEFAULT;
    return POW;
}

int main()
{
    std::cout << "Sever started..." << std::endl;

    // проверяем текущий размер файла с запросами на подключение
    std::ifstream connect("con", std::ios::binary);
    connect.seekg(END_FILE);
    // переменная для запоминания размера файла с подключениями
    int con_size = connect.tellg();
    connect.close();

    // проверяем текущий размер файла с запросами
    std::ifstream req("req", std::ios::binary);
    req.seekg(END_FILE);
    // переменная для запоминания размера файла с запросами
    int req_size = req.tellg();
    req.close();

    // пременная для отслеживания наверстывания обновлений в файлах
    int sizer;

    // база данных и чтение ее из файла
    std::map<int, int> base;
    std::map<int, BaseEntity> acc_base;
    BaseEntity entity;
    std::ifstream db("db", std::ios::binary);
    db.seekg(END_FILE);
    sizer = db.tellg();
    db.seekg(0);
    for (int i = 0; i <= sizer / sizeof(entity); i++)
    {
        db.read((char*) &entity, sizeof(entity));
        acc_base[entity.id] = entity;
        db.seekg(i * sizeof(entity));
    }
    db.close();

    // вывод считанной бд
    //for (std::map<int, BaseEntity>::iterator it = acc_base.begin(); it != acc_base.end(); it++)
    //{
    //    std::cout << it->first << " " << it->second.pass << std::endl;
    //}

    Connection connection;
    Request request;
    int connecting = 0;
    while (true)
    {
        // выводим текущую статистику подключений
        std::cout << "connecting: " << connecting << std::endl;

        // проверяем новые подключения
        connect.open("con", std::ios::binary);
        connect.seekg(END_FILE);
        sizer = connect.tellg();
        // получили размер обновлений на текущий момент времени 
        // закрываем поток чтобы у клиентов была возможность в него писать
        connect.close();
        //std::cout << con_size << " " << sizer << std::endl;
        while (con_size < sizer)
        {
            connect.open("con", std::ios::binary);
            // поэтапно отвечаем на запросы о подключении
            connect.seekg(con_size);
            connect.read((char*) &connection, sizeof(connection));
            std::cout << "connecning request from " << connection.id << std::endl;
            connect.close();
            if (connection.type == CONNECT)
            {
                if (base[connection.id] == 0)
                {
                    if (compare_pass(connection.pass, acc_base[connection.id].pass))
                    {
                        base[connection.id]++;
                        connection.status = REG_OK;
                        connecting++;
                    }
                    else
                    {
                        connection.status = REG_FAIL_LOGiN;
                    }
                    std::cout << "connecting to " << connection.id << std::endl;
                }
                else
                {
                    connection.status = REG_FAIL;
                }
                //std::cout << connection.id << " " << connection.type << " " << connection.status << std::endl;
                // создаем поток для ответа клиенту на запрос о подключении
                // т.к. нам нужно перезаписать часть файла используем флаг std::ios::out
                std::ofstream connect_answer("con", std::ios::out | std::ios::binary);
                connect_answer.seekp(con_size);
                connect_answer.write((char*) &connection, sizeof(connection));
                connect_answer.close();
            }
            else
            {
                // удаляем текущую сессию по запросу отключения
                base[connection.id]--;
                connecting--;
            }
            con_size += sizeof(connection);
        }

        // читаем текущий размер файла с запросами
        req.open("req", std::ios::binary);
        req.seekg(END_FILE);
        sizer = req.tellg();
        req.close();
        //std::cout << sizer << " " << req_size << std::endl;
        // проверка и обработка запросов
        while (req_size < sizer) 
        {
            req.open("req", std::ios::binary);
            // поэтапно отвечаем на запросы
            req.seekg(req_size);
            req.read((char*) &request, sizeof(request));
            std::cout << "request from " << request.id << std::endl;
            req.close();

            // вывод информации в структуре Person
            //std::cout << "name: " << request.person.name << " one: " 
            //<< request.person.one << " two: " << request.person.two << " three: " 
            //<< request.person.three << " four: " << request.person.four << std::endl;

            request.answer.res = get_answer(request.person.one, request.person.two, 
                                                    request.person.three, request.person.four);
            
            // создаем поток для ответа клиенту на запрос
            // т.к. нам нужно перезаписать часть файла используем флаг std::ios::out
            std::ofstream request_answer("req", std::ios::out | std::ios::binary);
            request_answer.seekp(req_size);
            request_answer.write((char*) &request, sizeof(request));
            request_answer.close();
            req_size += sizeof(request);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    return 0;
}