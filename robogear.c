#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <time.h>

#include "fcgi_config.h" 
#include "fcgiapp.h" 
#include <libpq-fe.h>

#define SOCKET_PATH "127.0.0.1:9000"
#define LIMIT "1000"
#define SIZEROW 3

//хранит дескриптор открытого сокета 
//ОСВОБОЖДАТЬ ПАМЯТЬ ИЗ-ЗА ЭТОГО МОЖЕТ БЫТЬ ОШИБКА SEGMENTATION FAULT (CORE DUMPED)
int socketId; 
char * idAttacker;
char * idTarget;

typedef struct machine {
    int id;
    int strength;
    int ammunition;
    int speed;
} machine;

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
            FCGX_PutS("<td id=\"inf", request.out);
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

void printAllLongRangeWeapons(PGconn *conn, FCGX_Request request, char* offset) {
    char * query = malloc(300);
    strcpy(query, "select*from longrangeweapon offset ");
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
            FCGX_PutS("<td id=\"lrw", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 4), request.out);
            FCGX_PutS("\">", request.out);
            //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
            //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
            //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
            FCGX_PutS("<img src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 3), request.out);
            FCGX_PutS("\" width=\"200\" height=\"99\" alt=\"error\">", request.out);
            FCGX_PutS("</td>", request.out);
        }
        FCGX_PutS("</tr>", request.out);
    }
    FCGX_PutS("</table>\r\n", request.out);
    PQclear(res);
}

void printAllWarMachines(PGconn *conn, FCGX_Request request, char* offset) {
    char * query = malloc(300);
    strcpy(query, "select*from warmachine offset ");
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
            FCGX_PutS(PQgetvalue(res, counter, 10), request.out);
            FCGX_PutS("\">", request.out);
            //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
            //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
            //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
            FCGX_PutS("<img src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 9), request.out);
            FCGX_PutS("\" width=\"266\" height=\"200\" alt=\"error\">", request.out);
            FCGX_PutS("<br>\r\n", request.out);
            FCGX_PutS("<input>\r\n", request.out);
            FCGX_PutS("</td>", request.out);
        }
        FCGX_PutS("</tr>", request.out);
    }
    FCGX_PutS("</table>\r\n", request.out);
    PQclear(res);
}

void printObjectsMachines(PGconn *conn, FCGX_Request request, machine * machines) {
    int nrows = 0;
    for (int i = 0; i < 20; i++) {
        if (machines[i].id != 0) {
            nrows++;
        }
    }
    if (nrows != 0) {
        int row = 0;
        FCGX_PutS("<table border=\"0\" style=\"margin: auto;\">", request.out);
        FCGX_PutS("<tr>", request.out);
        for (int i = 0; i < 20; i++) {
            if (machines[i].id == 0) {
                continue;
            }
            row++;
            if (row != 1 && row%SIZEROW == 1) {
                FCGX_PutS("</tr><tr>", request.out);
            }
            char * query = malloc(300);
            strcpy(query, "select*from warmachine where id=");
            char * idstr = malloc(5);
            sprintf(idstr, "%d", machines[i].id);
            strcat(query, idstr);
            strcat(query, ";");
            PGresult *res = PQexec(conn, query);
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                printf("No data retrieved\n");        
                PQclear(res);
                do_exit(conn);
            }
            free(query);
            FCGX_PutS("<td id=\"obj", request.out);
            char * str = malloc(10);
            sprintf(str, "%d", i);
            FCGX_PutS(str, request.out);
            FCGX_PutS("\">", request.out);
            FCGX_PutS("<img src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, 0, 9), request.out);
            FCGX_PutS("\" width=\"266\" height=\"200\" alt=\"error\">", request.out);
            FCGX_PutS("</td>", request.out);
            PQclear(res);
        }
        FCGX_PutS("</tr>", request.out);
        FCGX_PutS("</table>\r\n", request.out);
    }
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
    machine * machines = malloc(20 * sizeof(machine));
    for (int i = 0; i < 20; i++) {
        machines[i].id = 0;
    }
 
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
            FCGX_PutS("<hr align=\"center\" width=\"400\" size=\"5\" color=\"Black\" />\r\n", request.out);
            printAllLongRangeWeapons(conn, request, "0");
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
            FCGX_PutS("<hr align=\"center\" width=\"400\" size=\"5\" color=\"Black\" />\r\n", request.out);
            printObjectsMachines(conn, request, machines);
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
            char * dice;
            if (strstr(idAttacker, "inf")) {
                strcpy(query, "select*from infantry where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
                PGresult *res = PQexec(conn, query);
                dice = PQgetvalue(res, 0, 5);
                PQclear(res);
            }
            else if (strstr(idAttacker, "lrw")) {
                strcpy(query, "select*from longrangeweapon where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
                PGresult *res = PQexec(conn, query);
                dice = PQgetvalue(res, 0, 1);
                PQclear(res);
            }
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
<label><input type=\"radio\">Близко</label>\r\n\
<label><input type=\"radio\">Средне</label>\r\n\
<label><input type=\"radio\">Далеко</label>\r\n\
</div>\r\n\
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
    var radio = document.getElementsByTagName(\'input\');\r\n\
    for (var i = 0; i < 3; i++) {\r\n\
        radio[i].checked = false;\r\n\
        radio[i].addEventListener(\"change\", changeRadio);\r\n\
    }\r\n\
    function changeRadio() {\r\n\
        var is;\r\n\
        if (this.checked == true) {\r\n\
            is = true;\r\n\
        }\r\n\
        else {\r\n\
            is = false;\r\n\
        }\r\n\
        for (var i = 0; i < 3; i++) {\r\n\
            radio[i].checked = false;\r\n\
        }\r\n\
        if (is == true) {\r\n\
            this.checked = true;\r\n\
        }\r\n\
        else {\r\n\
            this.checked = false;\r\n\
        }\r\n\
    }\r\n\
    function yesButtonListener() {\r\n\
        var radio = document.getElementsByTagName(\'input\');\r\n\
        var s;\r\n\
        if (radio[0].checked) {\r\n\
            s = 0;\r\n\
        }\r\n\
        else if (radio[1].checked) {\r\n\
            s = 1;\r\n\
        }\r\n\
        else if (radio[2].checked) {\r\n\
            s = 2;\r\n\
        }\r\n\
        xhrSend(\"testshot:checkkill=\" + s + \";\");\r\n\
    }\r\n\
    function noButtonListener() {\r\n\
        xhrSend(\"menu\");\r\n\
    }\r\n\
    function yesnoKeyListener(e) {\r\n\
        if (e.keyCode == 13) {\r\n\
            var radio = document.getElementsByTagName(\'input\');\r\n\
            var s;\r\n\
            if (radio[0].checked) {\r\n\
                s = 0;\r\n\
            }\r\n\
            else if (radio[1].checked) {\r\n\
                s = 1;\r\n\
            }\r\n\
            else if (radio[2].checked) {\r\n\
                s = 2;\r\n\
            }\r\n\
            xhrSend(\"testshot:checkkill=\" + s + \";\");\r\n\
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
            if (strstr(idAttacker, "inf")) {
                strcpy(query, "select*from infantry where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
            }
            else if (strstr(idAttacker, "lrw")) {
                strcpy(query, "select*from longrangeweapon where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
            }
            //printf("%s", query);
            PGresult *res = PQexec(conn, query);
            char * power = malloc(10);
            if (strstr(idAttacker, "inf")) {
                strcpy(power, PQgetvalue(res, 0, 6));
            }
            else if (strstr(idAttacker, "lrw")) {
                strcpy(power, PQgetvalue(res, 0, 2));
            }
            //printf("%s", power);
            PQclear(res);
            free(query);
            char * armor = malloc(10);
            if (strstr(idTarget, "inf")) {
                char * query2 = malloc(300);
                strcpy(query2, "select*from infantry where id=");
                strcat(query2, idTarget + 3);
                strcat(query2, ";");
                PGresult *res2 = PQexec(conn, query2);
                strcpy(armor, PQgetvalue(res2, 0, 9));
                PQclear(res2);
                free(query2);
            }
            else if (strstr(idTarget, "obj")) {
                char * query3 = malloc(300);
                strcpy(query3, "select*from warmachine where id=");
                char * getid = malloc(10);
                sprintf(getid, "%d", machines[atoi(idTarget + 3)].id);
                strcat(query3, getid);
                strcat(query3, ";");
                PGresult *res3 = PQexec(conn, query3);
                char * strength = malloc(10);
                strcpy(strength, PQgetvalue(res3, 0, 7));
                char * tstrength = malloc(5);
                strncpy(tstrength, strength, strchr(strength, '-') - strength);
                tstrength[strchr(strength, '-') - strength] = '\0';
                armor = tstrength;
                PQclear(res3);
                free(query3);
            }
            int damage;
            if (strstr(idAttacker, "lrw")) {
                char * query3 = malloc(300);
                strcpy(query3, "select*from longrangeweapon where id=");
                strcat(query3, idAttacker + 3);
                strcat(query3, ";");
                PGresult *res3 = PQexec(conn, query3);
                char * distance = malloc(10);
                strcpy(distance, PQgetvalue(res3, 0, 5));
                char * indexDistance = malloc(2);
                strncpy(indexDistance, strchr(buf, '=') + 1, strchr(buf, ';') - strchr(buf, '=') - 1);
                indexDistance[strchr(buf, ';') - strchr(buf, '=') - 1] = '\0';
                int iindexDistance = atoi(indexDistance);
                char dist[3][5];
                int u = strchr(distance, '-') - distance;
                strncpy(dist[0], distance, u);
                dist[0][u] = '\0';
                u++;
                strncpy(dist[1], distance + u, strchr(distance + u, '-') - distance - u);
                dist[1][strchr(distance + u, '-') - distance - u] = '\0';
                u += strchr(distance + u, '-') - distance - u;
                u++;
                strncpy(dist[2], distance + u, strchr(distance + u, '\0') - distance - u);
                dist[2][strchr(distance + u, '\0') - distance - u] = '\0';
                damage = atoi(dist[iindexDistance]);
                PQclear(res3);
                free(query3);
            }
            if (strstr(idAttacker, "inf")) {
                damage = 1;
            }
            printf("DAMAGE%dDAMAGE", damage);
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
            if (strstr(idTarget, "inf")) {
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
            }
            else if (strstr(idTarget, "obj")) {
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
                    machines[atoi(idTarget + 3)].strength -= damage;
                    FCGX_PutS("<p style=\"text-align: center;\">Броня пробита</p>", request.out);
                    if (machines[atoi(idTarget + 3)].strength <= 0) {
                        machines[atoi(idTarget + 3)].id = 0;
                        FCGX_PutS("<p style=\"text-align: center;\">Машина уничтожена</p>", request.out);
                    }
                }
                else {
                    FCGX_PutS("<p style=\"text-align: center;\">Броня не пробита</p>", request.out);
                }
                for (int i = 0; i < 20; i++) {
                    printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
                }
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
        else if (strstr(buf, "setmachines")) {
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body>\r\n\
<p style=\"text-align: center;\">Выберите участвующие в сражении машины</p>\r\n\
            ", request.out);
            printAllWarMachines(conn, request, "0");
            FCGX_PutS("\
<div style=\"text-align: center;\"><button id=\"setmachines\">Ок</button></div>\r\n\
<script>\r\n\
var t = document.getElementsByTagName(\'input\');\r\n\
for (var i = 0; i < t.length; i++) {\r\n\
    t[i].value = \"0\";\r\n\
}\r\n\
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
document.getElementsByTagName(\'button\')[0].addEventListener(\"click\", clickButton);\r\n\
function clickButton() {\r\n\
    var inputs = document.getElementsByTagName(\'input\');\r\n\
    var req = \"fillarraymachines:\";\r\n\
    for (var i = 0; i < inputs.length; i++) {\r\n\
        if (i != inputs.length - 1) {\r\n\
            req += inputs[i].parentNode.id + \"=\" + inputs[i].value + \",\";\r\n\
        }\r\n\
        else {\r\n\
            req += inputs[i].parentNode.id + \"=\" + inputs[i].value + \";\";\r\n\
        }\r\n\
    }\r\n\
    var xhr = new XMLHttpRequest();\r\n\
    xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
    xhr.send(req);\r\n\
    xhrSend(\"menu\");\r\n\
}\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
        else if (strstr(buf, "fillarraymachines")) {
            char * buf1 = buf + (strchr(buf, ':') - buf) + 1;
            int counter = 0;
            for (int i = 0; buf[i] != '\0'; i++) {
                if (buf[i] == '=') {
                    counter++;
                }
            }
            int index = 0;
            for (int i = 0; i < counter; i++) {
                char * equalence = strchr(buf1, '=');
                char * id = malloc(5);
                strncpy(id, buf1, equalence - buf1);
                id[equalence - buf1] = '\0';
                char * point = strchr(buf1, ',');
                char * num = malloc(5);
                if (point != NULL) {
                    strncpy(num, equalence + 1, point - equalence);
                    num[point - equalence] = '\0';
                }
                else {
                    point = strchr(buf1, ';');
                    strncpy(num, equalence + 1, point - equalence);
                    num[point - equalence] = '\0';
                }
                int iid = atoi(id);
                int inum = atoi(num);
                int bindex = index;
                for (int i = bindex; i < bindex + inum; i++) {
                    machines[i].id = iid;
                    char * query = malloc(100);
                    strcpy(query, "select * from warmachine where id=");
                    strcat(query, id);
                    strcat(query, ";");
                    PGresult *res = PQexec(conn, query);
                    char * ammunition = PQgetvalue(res, 0, 6);
                    int iammunition = atoi(ammunition);
                    machines[i].ammunition = iammunition;
                    char * strength = PQgetvalue(res, 0, 7);
                    char * tstrength = malloc(5);
                    strncpy(tstrength, strength, strchr(strength, '-') - strength);
                    tstrength[strchr(strength, '-') - strength] = '\0';
                    int istrength = atoi(tstrength);
                    machines[i].strength = istrength;
                    char * speed = PQgetvalue(res, 0, 8);
                    char * tspeed = malloc(5);
                    strncpy(tspeed, speed, strchr(speed, '-') - speed);
                    tspeed[strchr(speed, '-') - speed] = '\0';
                    int ispeed = atoi(tspeed);
                    machines[i].speed = ispeed;
                    PQclear(res);
                    free(query);
                    index++;
                }
                buf1 = point + 1;
            }
            for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }
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
<br>\r\n\
<div style=\"text-align: center;\"><button id=\"setmachines\">Создать технику</button></div>\r\n\
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
    document.getElementById(\'setmachines\').addEventListener(\"click\", setmachinesButtonListener);\r\n\
    function testshotButtonListener() {\r\n\
        xhrSend(\"testshot:chooseattacker;\");\r\n\
    }\r\n\
    function setmachinesButtonListener() {\r\n\
        xhrSend(\"setmachines;\");\r\n\
    }\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
    }

    return 0; 
}