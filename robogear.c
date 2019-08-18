#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <time.h>

#include "fcgi_config.h" 
#include "fcgiapp.h" 
#include <libpq-fe.h>

#define SOCKET_PATH "127.0.0.1:9000"
#define LIMIT "6"
#define SIZEROW 3

//хранит дескриптор открытого сокета 
//ОСВОБОЖДАТЬ ПАМЯТЬ ИЗ-ЗА ЭТОГО МОЖЕТ БЫТЬ ОШИБКА SEGMENTATION FAULT (CORE DUMPED)
int socketId; 
char * idAttacker;
char * idTarget;

void do_exit(PGconn *conn) {
    PQfinish(conn);
    exit(1);
}

void printAllInfantry(PGconn *conn, FCGX_Request request, char* offset) {
    char * query = malloc(300);
    strcpy(query, "select*from infantry offset ");
    strcat(query, offset);
    strcat(query, " limit ");
    strcat(query, LIMIT);
    strcat(query, ";");
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");        
        PQclear(res);
        do_exit(conn);
    }
    int nrows = PQntuples(res);
    FCGX_PutS("<table border=\"0\" style=\"margin: auto;\">", request.out);
    int t = nrows%SIZEROW;
    int p;
    int k;
    int counter = 0;
    if (nrows%SIZEROW != 0) {
        p = 1;
    }
    else {
        p = 0;
    }
    for (int j = 0; j < nrows/SIZEROW + p; j++) {
        FCGX_PutS("<tr>", request.out);
        if (j == nrows/SIZEROW + p - 1 && t != 0) k = t;
        else k = SIZEROW;
        int oldcounter = counter;
        for (; counter < oldcounter + k; counter++) {
            FCGX_PutS("<td id=\"", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 12), request.out);
            FCGX_PutS("\">", request.out);
            //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
            //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
            //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
            FCGX_PutS("<img src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 11), request.out);
            FCGX_PutS("\" width=\"133\" height=\"143\" alt=\"error\">", request.out);
            FCGX_PutS("</td>", request.out);
        }
        FCGX_PutS("</tr>", request.out);
    }
    FCGX_PutS("</table>\r\n", request.out);
    PQclear(res);
}

int main(void) 
{ 
    //инициализация библилиотеки 
    FCGX_Init(); 
    printf("Lib is inited\n"); 
    
    //открываем новый сокет 
    socketId = FCGX_OpenSocket(SOCKET_PATH, 20); 
    if(socketId < 0) 
    { 
	    //ошибка при открытии сокета 
	    return 1; 
    } 
    printf("Socket is opened\n"); 

    int rc, i; 
    FCGX_Request request; 
    char *server_name; 

    if(FCGX_InitRequest(&request, socketId, 0) != 0) 
    { 
        //ошибка при инициализации структуры запроса 
        printf("Can not init request\n"); 
        return 1; 
    } 
    printf("Request is inited\n"); 

    PGconn *conn = PQconnectdb("user=postgres password=postgres dbname=robogear");
    if (PQstatus(conn) == CONNECTION_BAD) {
    
        //fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        printf("Connection to database failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }

    idAttacker = malloc(10);
    idTarget = malloc(10);
 
    for(;;) 
    {  

        //попробовать получить новый запрос 
        printf("Try to accept new request\n"); 
        rc = FCGX_Accept_r(&request); 

        if(rc < 0) 
        { 
            //ошибка при получении запроса 
            printf("Can not accept new request\n"); 
            break; 
        } 
        printf("request is accepted\n");  

        /*if (strstr(FCGX_GetParam("REQUEST_URI", request.envp), "img") != NULL) {
            FCGX_PutS("\
Content-type: image/jpeg\r\n\
\r\n\
            ",request.out);
            FILE *input;
            char get_char;
            input = fopen(FCGX_GetParam("Request URL", request.envp), "rb");
            while((get_char=fgetc(input))!= EOF) {
                char* get_str = malloc(2);
                get_str[0] = get_char;
                get_str[1] = '\0';
                FCGX_PutS(get_str, request.out);
            }
            fclose(input);
        }
        else {*/
        char * buf = malloc(500);
        FCGX_GetStr(buf, 500, request.in);
        char * ach = strchr(buf, '\x3B');
        if (ach - buf + 1 < 0) {

        }
        else {
            buf[(int)(ach - buf + 1)] = '\0';
        }
        if (strstr(buf, "testshot:chooseattacker")) {
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body>\r\n\
<p style=\"text-align: center;\">Выберите атакующего</p>\r\n\
            ", request.out);
            printAllInfantry(conn, request, "0");
            FCGX_PutS("\
<script>\r\n\
function xhrSend (s) {\r\n\
    var xhr = new XMLHttpRequest();\r\n\
    xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
    xhr.send(s);\r\n\
    xhr.onreadystatechange = function() {\r\n\
        if (xhr.readyState == XMLHttpRequest.DONE) {\r\n\
            document.open();\r\n\
            document.write(xhr.responseText);\r\n\
            document.close();\r\n\
        }\r\n\
    }\r\n\
}\r\n\
var tds = document.getElementsByTagName(\'td\');\r\n\
for (var i = 0; i < tds.length; i++) {\r\n\
    tds[i].addEventListener(\"click\", tdClickListener);\r\n\
}\r\n\
function tdClickListener() {\r\n\
    xhrSend(\"testshot:choosetarget=\" + this.id + \";\");\r\n\
}\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
        else if (strstr(buf, "testshot:choosetarget")) {
            strncpy(idAttacker, strchr(buf, '=') + 1, strchr(buf, ';') - strchr(buf, '=') - 1);
            idAttacker[strchr(buf, ';') - strchr(buf, '=') - 1] = '\0';
            //printf("%s", idAttacker);
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body>\r\n\
<p style=\"text-align: center;\">Выберите цель</p>\r\n\
            ", request.out);
            printAllInfantry(conn, request, "0");
            FCGX_PutS("\
<script>\r\n\
function xhrSend (s) {\r\n\
    var xhr = new XMLHttpRequest();\r\n\
    xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
    xhr.send(s);\r\n\
    xhr.onreadystatechange = function() {\r\n\
        if (xhr.readyState == XMLHttpRequest.DONE) {\r\n\
            document.open();\r\n\
            document.write(xhr.responseText);\r\n\
            document.close();\r\n\
        }\r\n\
    }\r\n\
}\r\n\
var tds = document.getElementsByTagName(\'td\');\r\n\
for (var i = 0; i < tds.length; i++) {\r\n\
    tds[i].addEventListener(\"click\", tdClickListener);\r\n\
}\r\n\
function tdClickListener() {\r\n\
    xhrSend(\"testshot:comparedistance=\" + this.id + \";\");\r\n\
}\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
        else if (strstr(buf, "testshot:comparedistance")) {
            strncpy(idTarget, strchr(buf, '=') + 1, strchr(buf, ';') - strchr(buf, '=') - 1);
            idTarget[strchr(buf, ';') - strchr(buf, '=') - 1] = '\0';
            //printf("idTarget: %s; idAttacker: %s", idTarget, idAttacker);
            char * query = malloc(300);
            strcpy(query, "select*from infantry where id=");
            strcat(query, idAttacker);
            strcat(query, ";");
            PGresult *res = PQexec(conn, query);
            char * dice = PQgetvalue(res, 0, 5);
            PQclear(res);
            char * num = malloc(2);
            char * side = malloc(3);
            char * mode = malloc(2);
            strncpy(num, dice, strchr(dice, '\xD0') - dice);
            num[strchr(dice, '\xD0') - dice] = '\0';
            if (strchr(dice, '+') != NULL) {
                strncpy(side, strchr(dice, '\x94') + 1, strchr(dice, '+') - strchr(dice, '\x94') - 1);
                side[strchr(dice, '+') - strchr(dice, '\x94') - 1] = '\0';
            }
            else {
                strncpy(side, strchr(dice, '\x94') + 1, strchr(dice, '\0') - strchr(dice, '\x94') - 1);
                side[strchr(dice, '\0') - strchr(dice, '\x94') - 1] = '\0';
            }
            if (strchr(dice, '+') != NULL) {
                strncpy(mode, strchr(dice, '+') + 1, strchr(dice, '\0') - strchr(dice, '+') - 1);
                mode[strchr(dice, '\0') - strchr(dice, '+') - 1] = '\0';
            }
            //printf("dice: %s; idAttacker: %s; idTarget: %s; num: %s; side: %s; mode: %s\n", dice, idAttacker, idTarget, num, side, mode);
            int inum = atoi(num);
            int iside = atoi(side);
            int imode = atoi(mode);
            srand(time(NULL));
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body>\r\n\
<p style=\"text-align: center;\">Пройден ли тест на дальность? Значения: \r\n\
            ", request.out);
            for (int i = 0; i < inum; i++) {
                char* str = malloc(5);
                sprintf(str, "%d", 1 + rand()%iside + imode);
                FCGX_PutS(str, request.out);
                FCGX_PutS(" ", request.out);
            }
            FCGX_PutS("\
</p>\r\n\
<br>\r\n\
<div style=\"text-align: center;\">\r\n\
<button id=\"yes\" style=\"text-align: center;\">Да</button>\r\n\
<button id=\"no\" style=\"text-align: center;\">Нет</button>\r\n\
</div>\r\n\
<script>\r\n\
    function xhrSend (s) {\r\n\
        var xhr = new XMLHttpRequest();\r\n\
        xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
        xhr.send(s);\r\n\
        xhr.onreadystatechange = function() {\r\n\
            if (xhr.readyState == XMLHttpRequest.DONE) {\r\n\
                document.open();\r\n\
                document.write(xhr.responseText);\r\n\
                document.close();\r\n\
            }\r\n\
        }\r\n\
    }\r\n\
    document.getElementById(\'yes\').addEventListener(\"click\", yesButtonListener);\r\n\
    document.getElementById(\'no\').addEventListener(\"click\", noButtonListener);\r\n\
    document.getElementsByTagName(\'body\')[0].addEventListener(\"keypress\", yesnoKeyListener);\r\n\
    function yesButtonListener() {\r\n\
        xhrSend(\"testshot:checkkill\");\r\n\
    }\r\n\
    function noButtonListener() {\r\n\
        xhrSend(\"menu\");\r\n\
    }\r\n\
    function yesnoKeyListener(e) {\r\n\
        if (e.keyCode == 13) {\r\n\
            xhrSend(\"testshot:checkkill\");\r\n\
        }\r\n\
        //if (e.keyCode == 27) {\r\n\
        //    xhrSend(\"menu\");\r\n\
        //}\r\n\
    }\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
        else if (strstr(buf, "testshot:checkkill")) {
            char * query = malloc(300);
            strcpy(query, "select*from infantry where id=");
            strcat(query, idAttacker);
            strcat(query, ";");
            //printf("%s", query);
            PGresult *res = PQexec(conn, query);
            char * power = malloc(10);
            strcpy(power, PQgetvalue(res, 0, 6));
            //printf("%s", power);
            PQclear(res);
            free(query);
            char * query2 = malloc(300);
            strcpy(query2, "select*from infantry where id=");
            strcat(query2, idTarget);
            strcat(query2, ";");
            PGresult *res2 = PQexec(conn, query2);
            char * armor = PQgetvalue(res2, 0, 9);
            PQclear(res2);
            free(query2);
            char * num = malloc(2);
            char * side = malloc(3);
            strncpy(num, power, strchr(power, '\xD0') - power);
            num[strchr(power, '\xD0') - power] = '\0';
            strncpy(side, strchr(power, '\x94') + 1, strchr(power, '\0') - strchr(power, '\x94') - 1);
            side[strchr(power, '\0') - strchr(power, '\x94') - 1] = '\0';
            /*side[0] = '6';
            side[1] = '\0';*/
            int inum = atoi(num);
            int iside = atoi(side);
            int iarmor = atoi(armor);
            //printf("power: %s; idAttacker: %s; idTarget: %s; num: %s; side: %s; armor: %s\n", power, idAttacker, idTarget, num, side, armor);
            srand(time(NULL));
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body>\r\n\
<p style=\"text-align: center;\">Значения бросков мощности: \r\n\
            ", request.out);
            int isKilled = 0;
            for (int i = 0; i < inum; i++) {
                char* str = malloc(5);
                int val = 1 + rand()%iside;
                if (val > iarmor) {
                    isKilled = 1;
                }
                sprintf(str, "%d", val);
                FCGX_PutS(str, request.out);
                FCGX_PutS(" ", request.out);
            }
            FCGX_PutS("\
</p>\r\n\
            ", request.out);
            if (isKilled) {
                FCGX_PutS("<p style=\"text-align: center;\">Боец убит</p>", request.out);
            }
            else {
                FCGX_PutS("<p style=\"text-align: center;\">Боец остался жив</p>", request.out);
            }
            FCGX_PutS("\
<br>\r\n\
<div style=\"text-align: center;\"><button id=\"ok\" style=\"text-align: center;\">Ok</button></div>\r\n\
<script>\r\n\
    function xhrSend (s) {\r\n\
        var xhr = new XMLHttpRequest();\r\n\
        xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
        xhr.send(s);\r\n\
        xhr.onreadystatechange = function() {\r\n\
            if (xhr.readyState == XMLHttpRequest.DONE) {\r\n\
                document.open();\r\n\
                document.write(xhr.responseText);\r\n\
                document.close();\r\n\
            }\r\n\
        }\r\n\
    }\r\n\
    document.getElementById(\'ok\').addEventListener(\"click\", okButtonListener);\r\n\
    document.getElementsByTagName(\'body\')[0].addEventListener(\"keypress\", menuKeyListener);\r\n\
    function okButtonListener() {\r\n\
        xhrSend(\"menu\");\r\n\
    }\r\n\
    function menuKeyListener(e) {\r\n\
        if (e.keyCode == 13) {\r\n\
            xhrSend(\"menu\");\r\n\
        }\r\n\
    }\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
        else {
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body>\r\n\
<div style=\"text-align: center;\"><button id=\"testshot\">Тест на выстрел</button></div>\r\n\
<script>\r\n\
    function xhrSend (s) {\r\n\
        var xhr = new XMLHttpRequest();\r\n\
        xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
        xhr.send(s);\r\n\
        xhr.onreadystatechange = function() {\r\n\
            if (xhr.readyState == XMLHttpRequest.DONE) {\r\n\
                document.open();\r\n\
                document.write(xhr.responseText);\r\n\
                document.close();\r\n\
            }\r\n\
        }\r\n\
    }\r\n\
    document.getElementById(\'testshot\').addEventListener(\"click\", testshotButtonListener);\r\n\
    function testshotButtonListener() {\r\n\
        xhrSend(\"testshot:chooseattacker;\");\r\n\
    }\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
    }

    return 0; 
}