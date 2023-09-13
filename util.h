#define CONNECT 10
#define DISCONNECT -10

#define WAITING_REG 0
#define REG_OK 1
#define REG_FAIL -1

struct Connection {
    int type;
    int id;
    int status;
};

struct Person {
    char name[25];
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