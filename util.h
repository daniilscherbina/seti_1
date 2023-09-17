#define CONNECT 10
#define DISCONNECT -10

#define WAITING_REG 0
#define REG_OK 1
#define REG_FAIL -1
#define REG_FAIL_LOGiN -2

#define END_FILE 0, std::ios::end
#define FILE_OPEN std::ios::binary | std::ios::app

struct Connection {
    int type;
    char pass[8];
    int id;
    int status;
};

struct Person {
    char name[20];
    int height;
    int weight;
};

struct Request {
    int id;
    Person person;
};

struct Answer {
    int id;
    int res;
};