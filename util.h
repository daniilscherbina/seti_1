#define CONNECT 10
#define DISCONNECT -10

// ожидание ответа
#define WAITING_REG 0
// подключение успешно установлено
#define REG_OK 1
// для данного аккаунта уже создана сессия
#define REG_FAIL -1
// неверный пароль
#define REG_FAIL_LOGiN -2

// виды стипендий:
// задолженность по экзамену
#define DOLG 1
// отсутствует
#define NO 2
// обычная
#define DEFAULT 3
// повышенная
#define POW 4

#define END_FILE 0, std::ios::end
#define FILE_OPEN std::ios::binary | std::ios::app

// структура описывающая запрос на подключение/отключение
struct Connection {
    int id;
    int type;
    char pass[8];
    int status;
};

// структура описывающая студента
struct Person {
    int one;
    int two;
    int three;
    int four;
    char name[20];
};

// структура описывающая ответ
struct Answer {
    int res;
};

// структура описывающая запрос к серверу на проведение вычислений
struct Request {
    int id;
    Answer answer;
    Person person;
};

