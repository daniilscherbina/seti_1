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
    
    std::ifstream config(".config");
    std::ifstream connect("con", std::ios::binary);
    std::ifstream request("request", std::ios::binary);
    int status = 0;

    bool run = true;
    int count_connections = 0;
    connect.seekg(0, std::ios::end);
    request.seekg(0, std::ios::end);
    long con_size = connect.tellg();
    long request_size = request.tellg();
    connect.close();
    request.close();

    Connection connection;
    Request req;
    Answer ans;
    
    std::map<int, int> id_base;

    while (run) 
    {
        config >> status;
        config.seekg(0);
        run = status;
        std::cout << "connections: " << count_connections << std::endl;
        std::cout << "listen..." << std::endl;
        
        connect.open("con", std::ios::binary);
        connect.seekg(0, std::ios::end);
        
        if (con_size < connect.tellg())
        {
            int temp = connect.tellg();
            connect.seekg(con_size);
            while (con_size != temp)
            {
                connect.open("con", std::ios::binary);
                connect.seekg(0, std::ios::end);
                connect.read((char*) &connection, sizeof(connection));
                connect.close();
                if (connection.type == DISCONNECT)
                {
                    count_connections--;
                    id_base[connection.id]--;
                } 
                else 
                {
                    if (connection.type == CONNECT)
                    {
                        count_connections++;
                        if (id_base[connection.id] != 1)
                        {
                            id_base[connection.id]++;
                            connection.status = REG_OK;
                        }
                        else
                        {
                            connection.status = REG_FAIL;
                        }
                        std::ofstream out("con", std::ios::binary);
                        out.seekp(con_size);
                        out.write((char*)&connection, sizeof(connection));
                        out.close();
                    } 
                    else
                    {
                        std::cout << "error: unknown connection code " << (int) connection.type << std::endl;
                    }
                }
                con_size += sizeof(connection);
            }
        }
        connect.close();

        request.open("request", std::ios::binary);
        request.seekg(0, std::ios::end);
        if (request_size < request.tellg())
        {
            int temp = request.tellg();
            request.seekg(request_size);
            while (request_size != temp)
            {

                request.read((char*) &req, sizeof(req));
                std::cout << "request: " << req.id << " " << req.person.name << "  " << req.person.weight << " " << req.person.height << " " << std::endl;
                int answer;
                double IMT = req.person.weight / (0.01 * req.person.height) / (0.01 * req.person.height);
		        if (18.5 <= IMT && IMT < 25) answer = 1;
		        if (18.5 > IMT) answer = 0;
		        if (IMT >= 25)answer = 2;
                ans.id = req.id;
                ans.res = answer;
                std::ofstream of(std::to_string(req.id), std::ios::binary | std::ios::app);
                of.write((char*) &ans, sizeof(ans));
                of.close();
                request_size += sizeof(req);
            }
        }
        request.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    config.close();
    connect.close();
    request.close();

    return 0;
}