#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <time.h>

#include "fcgi_config.h" 
#include "fcgiapp.h" 
#include <libpq-fe.h>

#include <json-c/json.h>

#define SOCKET_PATH "127.0.0.1:9000"
#define LIMIT "1000"
#define SIZEROW 4
#define SIZEROWLRW 8
#define SIZEROWMACHINE 2

//хранит дескриптор открытого сокета 
//ОСВОБОЖДАТЬ ПАМЯТЬ ИЗ-ЗА ЭТОГО МОЖЕТ БЫТЬ ОШИБКА SEGMENTATION FAULT (CORE DUMPED)
//СЕТ_КУКИС МОЖЕТ НАЛЕЗАТЬ НА КОНТЕНТ ЛЕНГС
int socketId; 
//char * idAttacker;
//char * idTarget;

typedef struct machine {
    int id;
    int strength;
    int ammunition;
    int speed;
} machine;

//machine * machines;

//int firstTime = 1;
//int isEmptyMachines = 1;

void readStrengthDamageSpeed(char * distance, char dist[3][5]) {
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
}

void do_exit(PGconn *conn) {
    PQfinish(conn);
    exit(1);
}

char* getCookie(char* name, FCGX_Request request) {
    char* cookies = FCGX_GetParam("HTTP_COOKIE", request.envp);
    char* innerName = malloc(25);
    strcpy(innerName, name);
    strcat(innerName, "=");
    if (cookies) {
        if (strstr(cookies, name) == cookies) {
            char* value = malloc(500);//ЕСЛИ БУДУТ ПРОБЛЕМЫ ЕЩЕ ПОВЫСИТЬ
            if (strstr(cookies, ";")) {
                strncpy(value, strchr(cookies, '=') + 1, strchr(cookies, ';') - strchr(cookies, '=') - 1);
                value[strchr(cookies, ';') - strchr(cookies, '=') - 1] = 0;
                return value;
            }
            else {
                strncpy(value, strchr(cookies, '=') + 1, strchr(cookies, '\0') - strchr(cookies, '=') - 1);
                value[strchr(cookies, '\0') - strchr(cookies, '=') - 1] = 0;
                return value;
            }
        }
        else {
            char* value = malloc(500);//ЕСЛИ ЧТО ПОВЫСИТЬ
            char* middle = malloc(25);
            middle[0] = 0;
            strcat(middle, "; ");
            strcat(middle, name);
            if (strstr(cookies, middle)) {
                if (strstr(strstr(cookies, middle) + strlen(middle), ";")) {
                    strncpy(value, strstr(cookies, middle) + strlen(middle) + 1, strchr(strstr(cookies, middle) + strlen(middle) + 1, ';') - (strstr(cookies, middle) + strlen(middle) + 1));
                    value[strchr(strstr(cookies, middle) + strlen(middle) + 1, ';') - (strstr(cookies, middle) + strlen(middle) + 1)] = 0;
                    return value;
                }
                else {
                    strncpy(value, strstr(cookies, middle) + strlen(middle) + 1, strchr(strstr(cookies, middle) + strlen(middle) + 1, '\0') - (strstr(cookies, middle) + strlen(middle) + 1));
                    value[strchr(strstr(cookies, middle) + strlen(middle) + 1, '\0') - (strstr(cookies, middle) + strlen(middle) + 1)] = 0;
                    return value;
                }
            }
            else {
                return NULL;
            }
            free(middle);
        }
    }
    else {
        return NULL;
    }
    free(innerName);
}

/*void deleteCookieRequest(char* name, FCGX_Request request) {
    if (getCookie(name, request) != NULL) {
        char* response = malloc(100);
        response[0] = 0;
        strcat(response, "Set-Cookie: ");
        strcat(response, name);
        strcat(response, "=");
    }
}*/

machine * getMachines(FCGX_Request request) {
    struct json_object* obj = json_tokener_parse(getCookie("object-machines", request));
    char* size = getCookie("size-object-machines", request);
    machine* response = (machine*)malloc(atoi(size) * sizeof(machine));
    int num = 0;
    json_object_object_foreach(obj, key, val) {
        for (int i = 0; i < json_object_array_length(val); i++) {
            struct json_object* tmp = json_object_array_get_idx(val, i);
            //printf("key = %s, arr[%d] = %s\n", key, i, json_object_to_json_string(tmp));
            switch (i) {
                case 0:
                    response[num].id = atoi(json_object_to_json_string(tmp));
                    break;
                case 1:
                    response[num].ammunition= atoi(json_object_to_json_string(tmp));
                    break;
                case 2:
                    response[num].strength = atoi(json_object_to_json_string(tmp));
                    break;
                case 3:
                    response[num].speed = atoi(json_object_to_json_string(tmp));
                    break;
            }
        }
        num++;
    }
    /*for (int i = 0; i < atoi(size); i++) {
        printf("obj:%d;id:%d;ammunition:%d;strength:%d;speed:%d\n", i, response[i].id, response[i].ammunition, response[i].strength, response[i].speed);
    }*/
    return response;
}

char* createJsonMachines(machine* machines, FCGX_Request request) {
    struct json_object *obj, *array;
    obj = json_object_new_object();
    for (int i = 0; i < atoi(getCookie("size-object-machines", request)); i++) {
        if (machines[i].id != 0) {
            array = json_object_new_array();
            json_object_array_add(array, json_object_new_int(machines[i].id));
            json_object_array_add(array, json_object_new_int(machines[i].ammunition));
            json_object_array_add(array, json_object_new_int(machines[i].strength));
            json_object_array_add(array, json_object_new_int(machines[i].speed));
            char* result = malloc(10);
            result[0] = 0;
            strcat(result, "obj");
            char* temp = malloc(5);
            sprintf(temp, "%d", i);
            strcat(result, temp);
            result[strlen(result) + strlen(temp)] = 0;
            json_object_object_add(obj, result, array);
        }
    }
    char* response = malloc(500);//увеличить если что
    strcpy(response, json_object_to_json_string(obj));
    return response;
}

void printAllInfantry(PGconn *conn, FCGX_Request request, char* offset, int side) {
    char * query = malloc(300);
    strcpy(query, "select*from infantry where Сторона=\'");
    if (side) {
        strcat(query, "Торговый Протекторат");
    }
    else {
        strcat(query, "Империя Полярис");
    }
    strcat(query, "\' order by Название offset ");
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
    free(query);
    int nrows = PQntuples(res);
    /*char * temp = malloc(5);
    sprintf(temp, "%d", nrows);
    strcat(temp, "\0");
    printf("%s", temp);
    free(temp);*/
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
    int commonCounter = 0;
    int oldCommonCounter6;
    int oldCommonCounter3;
    for (int j = 0; j < nrows/SIZEROW + p; j++) {
        FCGX_PutS("<tr>", request.out);
        if (j == nrows/SIZEROW + p - 1 && t != 0) k = t;
        else k = SIZEROW;
        int oldcounter = counter;
        for (; counter < oldcounter + 6 * k;) {
            if (counter >= nrows) {
                break;
            }
            FCGX_PutS("<td class=\"unit\" id=\"inf", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 12), request.out);
            FCGX_PutS("\">", request.out);
            //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
            //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
            //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
            FCGX_PutS("<img title=\"Дальн=", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 5), request.out);
            FCGX_PutS(" Мощн=", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 6), request.out);
            FCGX_PutS(" Бр=", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 9), request.out);
            FCGX_PutS("\" src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 11), request.out);
            FCGX_PutS("\" width=\"133\" height=\"143\" alt=\"error\">", request.out);
            FCGX_PutS("</td>", request.out);
            counter += 6;
        }
        FCGX_PutS("</tr>", request.out);
    }
    FCGX_PutS("</table>\r\n", request.out);

    for (; commonCounter < nrows; commonCounter++) {
        if (commonCounter % 6 == 0) {
            oldCommonCounter6 = commonCounter;
            FCGX_PutS("<div style=\"display: none;\" class=\"spoiler\" id=\"spoiler", request.out);
            FCGX_PutS(PQgetvalue(res, commonCounter, 12), request.out);
            FCGX_PutS("\" style=\"display: none;\">", request.out);
            FCGX_PutS("<table>", request.out);
        }
        if (commonCounter % 3 == 0) {\
            oldCommonCounter3 = commonCounter;
            FCGX_PutS("<tr>", request.out);
        }
        FCGX_PutS("<td class=\"units", request.out);
        if ((!strcmp(PQgetvalue(res, commonCounter, 5), "-") && !strcmp(PQgetvalue(res, commonCounter, 5), "-")) || (!strcmp(PQgetvalue(res, commonCounter, 5), "0Д6") && !strcmp(PQgetvalue(res, commonCounter, 5), "0Д6"))) {
            FCGX_PutS(" melee", request.out);
        }
        FCGX_PutS("\" id=\"infs", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 12), request.out);
        FCGX_PutS("\">", request.out);
        //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
        //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
        //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
        FCGX_PutS("<img title=\"Дальн=", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 5), request.out);
        FCGX_PutS(" Мощн=", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 6), request.out);
        FCGX_PutS(" Бр=", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 9), request.out);
        FCGX_PutS("\" src=\"http://localhost/", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 11), request.out);
        FCGX_PutS("\" width=\"133\" height=\"143\" alt=\"error\">", request.out);
        FCGX_PutS("</td>", request.out);
        if (commonCounter - oldCommonCounter3 == 2 || commonCounter == nrows - 1) {
            FCGX_PutS("</tr>", request.out);
        }
        if (commonCounter - oldCommonCounter6 == 5 || commonCounter == nrows - 1) {
            /*char * temp = malloc(5);
            sprintf(temp, "%d", commonCounter);
            strcat(temp, "-cc\n\0");
            printf("%s", temp);
            free(temp);
            char * temp2 = malloc(5);
            sprintf(temp2, "%d", oldCommonCounter6);
            strcat(temp2, "-occ\n\0");
            printf("%s", temp2);
            free(temp2);*/
            FCGX_PutS("</table>", request.out);
            FCGX_PutS("</div>", request.out);
        }
    }
    PQclear(res);
}

void printLongRangeWeapons(PGconn *conn, FCGX_Request request, char* offset, machine m) {
    char * query1 = malloc(300);
    strcpy(query1, "select Орудия from warmachine where id=");
    char* idstr = malloc(5);
    sprintf(idstr, "%d", m.id);
    strcat(query1, idstr);
    strcat(query1, ";");
    PGresult *res = PQexec(conn, query1);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");        
        PQclear(res);
        do_exit(conn);
    }
    //printf("test3\n");
    //printf("%s", PQgetvalue(res, 0, 0));
    char * query = malloc(300);
    strcpy(query, "select*from longrangeweapon where Название in (");
    strcat(query, PQgetvalue(res, 0, 0));
    strcat(query, ") offset ");
    strcat(query, offset);
    strcat(query, " limit ");
    strcat(query, LIMIT);
    strcat(query, ";");
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");        
        PQclear(res);
        do_exit(conn);
    }
    free(query);
    int nrows = PQntuples(res);
    /*FCGX_PutS("<table border=\"0\" style=\"margin: auto;\">", request.out);
    int t = nrows%SIZEROWLRW;
    int p;
    int k;
    int counter = 0;
    if (nrows%SIZEROWLRW != 0) {
        p = 1;
    }
    else {
        p = 0;
    }
    for (int j = 0; j < nrows/SIZEROWLRW + p; j++) {
        FCGX_PutS("<tr>", request.out);
        if (j == nrows/SIZEROWLRW + p - 1 && t != 0) k = t;
        else k = SIZEROWLRW;
        int oldcounter = counter;
        for (; counter < oldcounter + k; counter++) {
            FCGX_PutS("<td id=\"lrw", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 4), request.out);
            FCGX_PutS("\">", request.out);
            //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
            //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
            //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
            FCGX_PutS("<img title=\"Урон=", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 5), request.out);
            FCGX_PutS("\" src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 3), request.out);
            FCGX_PutS("\" width=\"200\" height=\"99\" alt=\"error\">", request.out);
            FCGX_PutS("</td>", request.out);
        }
        FCGX_PutS("</tr>", request.out);
    }
    FCGX_PutS("</table>\r\n", request.out);*/

    FCGX_PutS("<div style=\"display: none;\" class=\"spoiler-lrw\" id=\"spoiler-lrw", request.out);
    FCGX_PutS(idstr, request.out);
    FCGX_PutS("\" style=\"display: none;\">", request.out);
    FCGX_PutS("<table>", request.out);
    for (int commonCounter = 0; commonCounter < nrows; commonCounter++) {
        FCGX_PutS("<tr>", request.out);
        FCGX_PutS("<td id=\"lrw", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 4), request.out);
        FCGX_PutS("\">", request.out);
        //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
        //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
        //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
        FCGX_PutS("<img title=\"Урон=", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 5), request.out);
        FCGX_PutS("\" src=\"http://localhost/", request.out);
        FCGX_PutS(PQgetvalue(res, commonCounter, 3), request.out);
        FCGX_PutS("\" width=\"200\" height=\"99\" alt=\"error\">", request.out);
        FCGX_PutS("</td>", request.out);
        FCGX_PutS("</tr>", request.out);
    }
    FCGX_PutS("</table></div>", request.out);

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
    free(query);
    int nrows = PQntuples(res);
    FCGX_PutS("<table border=\"0\" style=\"margin: auto;\">", request.out);
    int t = nrows%SIZEROWMACHINE;
    int p;
    int k;
    int counter = 0;
    if (nrows%SIZEROWMACHINE != 0) {
        p = 1;
    }
    else {
        p = 0;
    }
    for (int j = 0; j < nrows/SIZEROWMACHINE + p; j++) {
        FCGX_PutS("<tr>", request.out);
        if (j == nrows/SIZEROWMACHINE + p - 1 && t != 0) k = t;
        else k = SIZEROWMACHINE;
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

void printObjectsMachines(PGconn *conn, FCGX_Request request, machine * machines, int hasPlusMinus, int side) {
    /*int nrows = 0;
    for (int i = 0; i < 20; i++) {
        if (machines[i].id != 0) {
            nrows++;
        }
    }*/
    int nrows = atoi(getCookie("size-object-machines", request));
    if (nrows != 0) {
        int row = 0;
        FCGX_PutS("<table border=\"0\" style=\"margin: auto;\">", request.out);
        FCGX_PutS("<tr>", request.out);
        for (int i = 0; i < nrows; i++) {
            /*if (machines[i].id == 0) {
                continue;
            }*/
            row++;
            if (row != 1 && row%SIZEROWMACHINE == 1) {
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
            if (!hasPlusMinus) {
                if (side) {
                    FCGX_PutS("-protectorat", request.out);
                }
                else {
                    FCGX_PutS("-polaris", request.out);
                }
            }
            FCGX_PutS("\" class=\"obj", request.out);
            FCGX_PutS(idstr, request.out);
            FCGX_PutS(" machine\">", request.out);
            FCGX_PutS("<img title=\"Скорострельность=", request.out);
            FCGX_PutS(PQgetvalue(res, 0, 5), request.out);
            FCGX_PutS(" Боезапас=", request.out);
            char * ammo = malloc(5);
            sprintf(ammo, "%d", machines[i].ammunition);
            FCGX_PutS(ammo, request.out);
            FCGX_PutS(" Прочность=", request.out);
            FCGX_PutS(PQgetvalue(res, 0, 7), request.out);
            FCGX_PutS(" Прочность=", request.out);
            char * strength = malloc(5);
            sprintf(strength, "%d", machines[i].strength);
            FCGX_PutS(strength, request.out);
            FCGX_PutS(" Скорость=", request.out);
            char * speed = malloc(5);
            sprintf(speed, "%d", machines[i].speed);
            FCGX_PutS(speed, request.out);
            FCGX_PutS("\" src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, 0, 9), request.out);
            FCGX_PutS("\" width=\"266\" height=\"200\" alt=\"error\">", request.out);
            if (hasPlusMinus) {
                FCGX_PutS("\
<br>\r\n\
<div class=\"strength\">Прочность<button class=\"plus\">+</button><button class=\"minus\">-</button></div>\r\n\
<br>\r\n\
<div class=\"ammunition\">Боезапас<button class=\"plus\">+</button><button class=\"minus\">-</button></div>\r\n\
                ", request.out);
            }
            FCGX_PutS("</td>", request.out);
            PQclear(res);
        }
        FCGX_PutS("</tr>", request.out);
        FCGX_PutS("</table>\r\n", request.out);
    }

    if (!hasPlusMinus) {
        int * temp = (int*)malloc(nrows*sizeof(int));
        for (int i = 0; i < nrows; i++) {
            temp[i] = machines[i].id;
        }
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < nrows; j++) {
                if (temp[i] == temp[j] && i != j) {
                    temp[j] = 0;
                }
            }
        }
        //printf("test1\n");
        for (int i = 0; i < nrows; i++) {
            if (temp[i] != 0) {
                //printf("test2\n");
                printLongRangeWeapons(conn, request, "0", machines[i]);
            }
        }
    }

}

void printAllBlowUps(PGconn *conn, FCGX_Request request, char * offset) {
    char * query = malloc(300);
    strcpy(query, "select*from blowup offset ");
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
            FCGX_PutS("<td id=\"blu", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 4), request.out);
            FCGX_PutS("\" class=\"blowup\">", request.out);
            //FCGX_PutS("<img src=\"data:image/gif;base64,", request.out);
            //FCGX_PutS(PQgetvalue(res, i, 11), request.out);
            //FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
            FCGX_PutS("<img title=\"Урон=", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 2), request.out);
            FCGX_PutS("\" src=\"http://localhost/", request.out);
            FCGX_PutS(PQgetvalue(res, counter, 3), request.out);
            FCGX_PutS("\" width=\"100\" height=\"100\" alt=\"error\">", request.out);
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

    //idAttacker = malloc(10);
    //idTarget = malloc(10);
    //machines = malloc(20 * sizeof(machine));
    /*for (int i = 0; i < 20; i++) {
        machines[i].id = 0;
    }*/
 
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
            /*printf("%s\n", FCGX_GetParam("HTTP_COOKIE", request.envp));
            printf("%s\n", getCookie(FCGX_GetParam("HTTP_COOKIE", request.envp), "abcd"));
            printf("%s\n", getCookie(FCGX_GetParam("HTTP_COOKIE", request.envp), "abc"));
            printf("%s\n", getCookie(FCGX_GetParam("HTTP_COOKIE", request.envp), "bcddse"));
            printf("%s\n", getCookie(FCGX_GetParam("HTTP_COOKIE", request.envp), "bcde"));*/





            /*struct json_object *obj, *array;
            obj = json_object_new_object();
            array = json_object_new_array();
            json_object_array_add(array, json_object_new_int(1));
            json_object_array_add(array, json_object_new_int(2));
            json_object_array_add(array, json_object_new_int(3));
            json_object_array_add(array, json_object_new_int(4));
            json_object_object_add(obj, "obj0", array);
            array = json_object_new_array();
            json_object_array_add(array, json_object_new_int(10));
            json_object_array_add(array, json_object_new_int(20));
            json_object_array_add(array, json_object_new_int(30));
            json_object_array_add(array, json_object_new_int(40));
            json_object_object_add(obj, "obj1", array);
            //printf("%s\n", json_object_to_json_string(obj));
            json_object_object_foreach(obj, key, val) {
                for (i = 0; i < json_object_array_length(val); i++) {
                    struct json_object* tmp = json_object_array_get_idx(val, i);
                    printf("key = %s, arr[%d] = %s\n", key, i, json_object_to_json_string(tmp));
                }
            }*/





            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
<div style=\"text-align: center; position: relative; bottom: 20px;\"><span style=\"color: white; font-size: 20;\">Нажмите чтобы поменять атакующую сторону</span><img id=\"icon\" style=\"position: relative; top: 20px;  left: 20px;\" src=\"\" width=\"60\"></div>\r\n\
            ", request.out);
            FCGX_PutS("<table style=\"margin: auto; border-spacing: 100px 0px;\">\r\n", request.out);
            FCGX_PutS("<tr>\r\n", request.out);
            FCGX_PutS("<td id=\"polaris\">\r\n", request.out);
            printAllInfantry(conn, request, "0", 0);
            FCGX_PutS("</td><td id=\"protectorat\">\r\n", request.out);
            printAllInfantry(conn, request, "0", 1);
            FCGX_PutS("</td></tr></table>\r\n", request.out);
            /*char* sflag = getCookie("isEmptyMachines", request);
            int flag;
            if (sflag == NULL) {
                flag = 0;
            }
            else {
                flag = atoi(sflag);
            }*/
            int flag = atoi(getCookie("isEmptyMachines", request));
            if (!flag) {
                machine* machines = getMachines(request);
                FCGX_PutS("<hr align=\"center\" width=\"400\" size=\"5\" color=\"Black\" />\r\n", request.out);
                FCGX_PutS("<table style=\"margin: auto; border-spacing: 100px 0px;\">\r\n", request.out);
                FCGX_PutS("<tr>\r\n", request.out);
                FCGX_PutS("<td id=\"polaris-machines\">\r\n", request.out);
                printObjectsMachines(conn, request, machines, 0, 0);
                FCGX_PutS("</td><td id=\"protectorat-machines\">\r\n", request.out);
                printObjectsMachines(conn, request, machines, 0, 1);
                FCGX_PutS("</td></tr></table>\r\n", request.out);
                //printAllLongRangeWeapons(conn, request, "0");
                free(machines);
            }
            FCGX_PutS("<hr align=\"center\" width=\"400\" size=\"5\" color=\"Black\" />\r\n", request.out);
            printAllBlowUps(conn, request, "0");
            FCGX_PutS("\
<div style=\"text-align: center; padding-top: 35px;\" id=\"send\"><button>Отправить</button></div>\r\n\
<div style=\"text-align: center; padding-top: 35px;\" id=\"menu\"><button>Меню</button></div>\r\n\
</body>\r\n\
<script src=\"testshot.js\"></script>\r\n\
</html>\r\n\
            ", request.out);
        }
        else if (strstr(buf, "setttacker")) {
            char* idAttacker = malloc(10);
            strncpy(idAttacker, strchr(buf, '=') + 1, strchr(buf, ';') - strchr(buf, '=') - 1);
            idAttacker[strchr(buf, ';') - strchr(buf, '=') - 1] = '\0';
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: idAttacker=\
            ", request.out);
            FCGX_PutS(idAttacker, request.out);
            FCGX_PutS("\r\n", request.out);
            FCGX_PutS("\r\n", request.out);
            //printf("idAttacker:%s", idAttacker);
        }
        /*else if (strstr(buf, "testshot:ammunition")) {
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
<p style=\"text-align: center; color: white; font-size: 20;\">Выберите стреляющую машину</p>\r\n\
            ", request.out);
            printObjectsMachines(conn, request, machines, 0);
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
    tds[i].addEventListener(\"click\", tdListener);\r\n\
}\r\n\
function tdListener() {\r\n\
    var req = \"machinesammunition=\" + this.id + \";\";\r\n\
    var xhr = new XMLHttpRequest();\r\n\
    xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
    xhr.send(req);\r\n\
    xhr.onreadystatechange = function() {\r\n\
        if (xhr.readyState == XMLHttpRequest.DONE) {\r\n\
            if (xhr.responseText == \"ok\") {\r\n\
                xhrSend(\"testshot:choosetarget;\");\r\n\
            }\r\n\
            else if (xhr.responseText == \"error\") {\r\n\
                alert(\"Не хватает боезапаса!\");\r\n\
                xhrSend(\"menu\");\r\n\
            }\r\n\
        }\r\n\
    }\r\n\
}\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }*/
        else if (strstr(buf, "machinesammunition")) {
            char * num = malloc(5);
            strncpy(num, strchr(buf, '=') + 4, strchr(buf, ';') - strchr(buf, '=') - 4);
            num[strchr(buf, ';') - strchr(buf, '=') - 4] = '\0';
            machine* machines = getMachines(request);
            if (machines[atoi(num)].ammunition > 0) {
                machines[atoi(num)].ammunition -= 1;
                FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: object-machines=\
                ", request.out);
                //printf("%s", createJsonMachines(machines, request));
                FCGX_PutS(createJsonMachines(machines, request), request.out);
                FCGX_PutS("\r\n", request.out);
                FCGX_PutS("\
\r\n\
ok", request.out);
            }
            else {
                FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
error", request.out);
            }
            free(machines);
            /*for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }*/
        }
        /*else if (strstr(buf, "testshot:choosetarget")) {
            //printf("%s", idAttacker);
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
<p style=\"text-align: center; color: white; font-size: 20;\">Выберите цель</p>\r\n\
            ", request.out);
            printAllInfantry(conn, request, "0", 0);
            FCGX_PutS("<hr align=\"center\" width=\"400\" size=\"5\" color=\"Black\" />\r\n", request.out);
            printObjectsMachines(conn, request, machines, 0);
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
    var req = \"settarget=\" + this.id + \";\";\r\n\
    var xhr = new XMLHttpRequest();\r\n\
    xhr.open(\'POST\', \'http://localhost/\', true);\r\n\
    xhr.send(req);\r\n\
            ", request.out);
            if (strstr(idAttacker, "lrw") || strstr(idAttacker, "inf")) {
                FCGX_PutS("\
    xhrSend(\"testshot:comparedistance;\");\r\n\
                ", request.out);
            }
            else if (strstr(idAttacker, "blu")) {
                FCGX_PutS("\
    xhrSend(\"testshot:checkkill=1000;\");\r\n\
                ", request.out);
            }
            FCGX_PutS("\
}\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }*/
        else if (strstr(buf, "settarget")) {
            char* idTarget = malloc(10);
            strncpy(idTarget, strchr(buf, '=') + 1, strchr(buf, ';') - strchr(buf, '=') - 1);
            idTarget[strchr(buf, ';') - strchr(buf, '=') - 1] = '\0';
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: idTarget=\
            ", request.out);
            FCGX_PutS(idTarget, request.out);
            FCGX_PutS("\r\n", request.out);
            FCGX_PutS("\r\n", request.out);
            //printf("idTarget:%s", idTarget);
        }




















        else if (strstr(buf, "testshot:comparedistance")) {
            //printf("idTarget: %s; idAttacker: %s", idTarget, idAttacker);
            char * query = malloc(300);
            char * dice;
            char* idTarget = getCookie("idTarget", request);
            char* idAttacker = getCookie("idAttacker", request);
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
Content-type: text/plain\r\n\
\r\n\
<p style=\"text-align: center; color: white; font-size: 20;\">Пройден ли тест на дальность? Значения: \r\n\
            ", request.out);
            for (int i = 0; i < inum; i++) {
                char* str = malloc(5);
                sprintf(str, "%d", 1 + rand()%iside + imode);
                FCGX_PutS(str, request.out);
                FCGX_PutS(" ", request.out);
            }
            FCGX_PutS("\
</p>\r\n\
            ", request.out);
            if (strstr(idAttacker, "lrw") && strstr(idTarget, "obj")) {
                FCGX_PutS("\
<br>\r\n\
<div style=\"text-align: center;\">\r\n\
<label style=\"color: white; font-size: 16;\"><input type=\"radio\">Близко</label>\r\n\
<label style=\"color: white; font-size: 16;\"><input type=\"radio\">Средне</label>\r\n\
<label style=\"color: white; font-size: 16;\"><input type=\"radio\">Далеко</label>\r\n\
</div>\r\n\
                ", request.out);
            }
            FCGX_PutS("\
<br>\r\n\
<div style=\"text-align: center;\">\r\n\
<button id=\"yes\" style=\"text-align: center;\">Да</button>\r\n\
<button id=\"no\" style=\"text-align: center;\">Нет</button>\r\n\
</div>\r\n\
            ", request.out);
        }






















        else if (strstr(buf, "logs:getattackerdefender")) {
            machine* machines = getMachines(request);
            char * query = malloc(300);
            char* idAttacker = getCookie("idAttacker", request);
            char* idTarget = getCookie("idTarget", request);
            if (strstr(idAttacker, "inf")) {
                strcpy(query, "select Название, Номер from infantry where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
            }
            else if (strstr(idAttacker, "lrw")) {
                strcpy(query, "select Название from longrangeweapon where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
            }
            else if (strstr(idAttacker, "blu")) {
                strcpy(query, "select Название from blowup where id=");
                strcat(query, idAttacker + 3);
                strcat(query, ";");
            }
            PGresult *res = PQexec(conn, query);
            char * response = malloc(250);
            if (strstr(idAttacker, "inf")) {
                strcpy(response, "<div class=\"turn\"><p>Атакующий боец из отряда:");
                strcat(response, PQgetvalue(res, 0, 0));
                strcat(response, ", под номером:");
                strcat(response, PQgetvalue(res, 0, 1));
            }
            else if (strstr(idAttacker, "lrw")) {
                strcpy(response, "<div class=\"turn\"><p>Атакующее орудие:");
                strcat(response, PQgetvalue(res, 0, 0));
            }
            else if (strstr(idAttacker, "blu")) {
                strcpy(response, "<div class=\"turn\"><p>Размер взрыва:");
                strcat(response, PQgetvalue(res, 0, 0));
            }
            //printf("%s", power);
            PQclear(res);
            free(query);

            strcat(response, " ");
            query = malloc(300);
            if (strstr(idTarget, "inf")) {
                strcpy(query, "select Название, Номер from infantry where id=");
                strcat(query, idTarget + 3);
                strcat(query, ";");
            }
            else if (strstr(idTarget, "obj")) {
                char* idObj = malloc(5);
                sprintf(idObj, "%d", machines[atoi(idTarget + 3)].id);
                strcpy(query, "select Название from warmachine where id=");
                strcat(query, idObj);
                strcat(query, ";");
            }
            res = PQexec(conn, query);
            if (strstr(idTarget, "inf")) {
                strcat(response, "Цель боец из отряда:");
                strcat(response, PQgetvalue(res, 0, 0));
                strcat(response, ", под номером:");
                strcat(response, PQgetvalue(res, 0, 1));
            }
            else if (strstr(idTarget, "obj")) {
                strcat(response, "Цель машина:");
                strcat(response, PQgetvalue(res, 0, 0));
            }
            //printf("%s", power);
            PQclear(res);
            free(query);

            strcat(response, "</p>");

            FCGX_PutS("\
Content-type: plain/text\r\n\
\r\n", request.out);
            FCGX_PutS(response, request.out);
            free(response);
        }






        else if (strstr(buf, "testshot:checkkill")) {
            machine* machines = getMachines(request);
            char * query = malloc(300);
            char* idAttacker = getCookie("idAttacker", request);
            char* idTarget = getCookie("idTarget", request);
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
            else if (strstr(idAttacker, "blu")) {
                strcpy(query, "select*from blowup where id=");
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
            else if (strstr(idAttacker, "blu")) {
                strcpy(power, PQgetvalue(res, 0, 1));
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
            char * sdamage = malloc(5);
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
                readStrengthDamageSpeed(distance, dist);
                sdamage = dist[iindexDistance];
                damage = atoi(dist[iindexDistance]);
                PQclear(res3);
                free(query3);
            }
            else if (strstr(idAttacker, "blu")) {
                char * query3 = malloc(300);
                strcpy(query3, "select*from blowup where id=");
                strcat(query3, idAttacker + 3);
                strcat(query3, ";");
                PGresult *res3 = PQexec(conn, query3);
                strcpy(sdamage, PQgetvalue(res3, 0, 2));
                damage = atoi(sdamage);
                PQclear(res3);
                free(query3);
            }
            if (strstr(idAttacker, "inf")) {
                damage = 1;
            }
            char* logs = malloc(1000);
            logs[0] = 0;
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
            char* response = malloc(1000);//ЕСЛИ ЧТО ПОВЫСИТЬ
            response[0] = 0;
            strcat(response, "\
Content-type: plain/text\r\n");
            char* tempHTML = malloc(1000);
            tempHTML[0] = 0;
            if (strstr(idTarget, "inf")) {
                FCGX_PutS(response, request.out);
                FCGX_PutS("\r\n\
<p style=\"text-align: center; color: white; font-size: 20;\">Значения бросков мощности: \r\n\
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
                    FCGX_PutS("<p style=\"text-align: center; color: white; font-size: 20;\">Боец убит</p>", request.out);
                    strcat(logs, "<p>Боец убит</p>");
                }
                else {
                    FCGX_PutS("<p style=\"text-align: center; color: white; font-size: 20;\">Боец остался жив</p>", request.out);
                    strcat(logs, "<p>Боец остался жив</p>");
                }
            }
            else if (strstr(idTarget, "obj")) {
                strcat(tempHTML, "<p style=\"text-align: center; color: white; font-size: 20;\">Значения бросков мощности: \r\n");
                int isKilled = 0;
                for (int i = 0; i < inum; i++) {
                    char* str = malloc(5);
                    int val = 1 + rand()%iside;
                    if (val > iarmor) {
                        isKilled = 1;
                    }
                    sprintf(str, "%d", val);
                    //FCGX_PutS(str, request.out);
                    //FCGX_PutS(" ", request.out);
                    strcat(tempHTML, str);
                    strcat(tempHTML, " ");
                }
                /*FCGX_PutS("\
</p>\r\n\
                ", request.out);*/
                strcat(tempHTML, "</p>\r\n");
                if (isKilled) {
                    char * query = malloc(300);
                    strcpy(query, "select*from warmachine where id=");
                    char * reqId = malloc(5);
                    sprintf(reqId, "%d", machines[atoi(idTarget + 3)].id);
                    strcat(query, reqId);
                    strcat(query, ";");
                    PGresult *res = PQexec(conn, query);
                    char * distance = malloc(10);
                    strcpy(distance, PQgetvalue(res, 0, 7));
                    free(query);
                    int temp = machines[atoi(idTarget + 3)].strength;
                    machines[atoi(idTarget + 3)].strength -= damage;
                    strcat(tempHTML, "<p style=\"text-align: center; color: white; font-size: 20;\">Броня пробита</p>");
                    strcat(logs, "<p>Броня пробита</p>");
                    strcat(tempHTML, "<p style=\"text-align: center; color: white; font-size: 20;\">Урон - ");
                    strcat(logs, "<p>Урон - ");
                    strcat(tempHTML, sdamage);
                    strcat(logs, sdamage);
                    strcat(tempHTML, "</p>");
                    strcat(logs, "</p>");
                    char dist[3][5];
                    int strength[3];
                    readStrengthDamageSpeed(distance, dist);
                    for (int i = 0; i < 3; i++) {
                        strength[i] = atoi(dist[i]);
                    }
                    int levelBefore;
                    int levelAfter;
                    if (machines[atoi(idTarget + 3)].strength <= strength[0] && machines[atoi(idTarget + 3)].strength > strength[1]) {
                        levelAfter = 0;
                    }
                    else if (machines[atoi(idTarget + 3)].strength <= strength[1] && machines[atoi(idTarget + 3)].strength > strength[2]) {
                        levelAfter = 1;
                    }
                    else if (machines[atoi(idTarget + 3)].strength <= strength[2] && machines[atoi(idTarget + 3)].strength > 0) {
                        levelAfter = 2;
                    }
                    else if (machines[atoi(idTarget + 3)].strength <= 0) {
                        levelAfter = 3;
                    }
                    query = malloc(300);
                    strcpy(query, "select*from warmachine where id=");
                    strcat(query, reqId);
                    strcat(query, ";");
                    res = PQexec(conn, query);
                    free(distance);
                    distance = malloc(10);
                    strcpy(distance, PQgetvalue(res, 0, 8));
                    free(query);
                    char dist1[3][5];
                    int speed[3];
                    readStrengthDamageSpeed(distance, dist1);
                    for (int i = 0; i < 3; i++) {
                        speed[i] = atoi(dist1[i]);
                    }
                    if (levelAfter != 3) {
                        machines[atoi(idTarget + 3)].speed = speed[levelAfter];
                    }
                    if (temp <= strength[0] && temp > strength[1]) {
                        levelBefore = 0;
                    }
                    else if (temp <= strength[1] && temp > strength[2]) {
                        levelBefore = 1;
                    }
                    else if (temp <= strength[2] && temp > 0) {
                        levelBefore = 2;
                    }
                    else if (temp <= 0) {
                        levelBefore = 3;
                    }
                    int numOfDice = levelAfter - levelBefore;
                    if (numOfDice > 0) {
                        strcat(tempHTML, "\
<p style=\"text-align: center; color: white; font-size: 20;\">Значения бросков теста на смерть пилота: \r\n");
                        int isPilotKilled = 0;
                        for (int i = 0; i < numOfDice; i++) {
                            char* str = malloc(5);
                            int val = 1 + rand()%6;
                            if (val > 4) {
                                isPilotKilled = 1;
                            }
                            sprintf(str, "%d", val);
                            strcat(tempHTML, str);
                            strcat(tempHTML, " ");
                        }
                        FCGX_PutS("\
</p>\r\n\
                        ", request.out);
                        if (isPilotKilled) {
                            strcat(tempHTML, "<p style=\"text-align: center; color: white; font-size: 20;\">Пилот убит</p>");
                            strcat(logs, "<p>Пилот убит</p>");
                        }
                    }
                    if (machines[atoi(idTarget + 3)].strength <= 0) {
                        machines[atoi(idTarget + 3)].id = 0;
                        strcat(tempHTML, "<p style=\"text-align: center; color: white; font-size: 20;\">Машина уничтожена</p>");
                        strcat(logs, "<p>Машина уничтожена</p>");
                        strcat(response, "Set-Cookie: size-object-machines=");
                        int size = atoi(getCookie("size-object-machines", request));
                        size--;
                        if (size < 0) {
                            size = 0;
                        }
                        if (size == 0) {
                            strcat(tempHTML, "Set-Cookie: isEmptyMachines=1\r\n");
                        }
                        char * ssize = malloc(5);
                        sprintf(ssize, "%d", size);
                        strcat(response, ssize);
                        strcat(response, "\r\n");
                    }
                    strcat(response, "\
Set-Cookie: object-machines=\
                    ");
                    strcat(response, createJsonMachines(machines, request));
                    strcat(response, "\r\n");
                    strcat(response, "\r\n");
                    //printf("%s%s", response, tempHTML);
                    FCGX_PutS(response, request.out);
                    FCGX_PutS(tempHTML, request.out);
                    free(response);
                    free(tempHTML);
                }
                else {
                    strcat(response, "\r\n");
                    FCGX_PutS(response, request.out);
                    FCGX_PutS(tempHTML, request.out);
                    //printf("%s%s", response, tempHTML);
                    free(response);
                    free(tempHTML);
                    FCGX_PutS("<p style=\"text-align: center; color: white; font-size: 20;\">Броня не пробита</p>", request.out);
                    strcat(logs, "<p>Броня не пробита</p>");
                }
                /*for (int i = 0; i < 20; i++) {
                    printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
                }*/
            }
            FCGX_PutS("\
<br>\r\n\
<div style=\"text-align: center;\"><button id=\"ok\" style=\"text-align: center;\">Ok</button></div>\r\n\
            ", request.out);
            FCGX_PutS("//logs//", request.out);
            FCGX_PutS(logs, request.out);
            free(logs);
            free(machines);
        }
        else if (strstr(buf, "setmachines")) {
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: firstTime=0\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
<p style=\"text-align: center; color: white; font-size: 20;\">Выберите участвующие в сражении машины</p>\r\n\
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
            struct json_object *obj, *array;
            obj = json_object_new_object();
            char* size = malloc(5);
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
                    array = json_object_new_array();
                    json_object_array_add(array, json_object_new_int(iid));
                    char * query = malloc(100);
                    strcpy(query, "select * from warmachine where id=");
                    strcat(query, id);
                    strcat(query, ";");
                    PGresult *res = PQexec(conn, query);
                    char * ammunition = PQgetvalue(res, 0, 6);
                    int iammunition = atoi(ammunition);
                    json_object_array_add(array, json_object_new_int(iammunition));
                    char * strength = PQgetvalue(res, 0, 7);
                    char * tstrength = malloc(5);
                    strncpy(tstrength, strength, strchr(strength, '-') - strength);
                    tstrength[strchr(strength, '-') - strength] = '\0';
                    int istrength = atoi(tstrength);
                    json_object_array_add(array, json_object_new_int(istrength));
                    char * speed = PQgetvalue(res, 0, 8);
                    char * tspeed = malloc(5);
                    strncpy(tspeed, speed, strchr(speed, '-') - speed);
                    tspeed[strchr(speed, '-') - speed] = '\0';
                    int ispeed = atoi(tspeed);
                    json_object_array_add(array, json_object_new_int(ispeed));
                    char* key = malloc(5);
                    key[0] = 0;
                    strcat(key, "obj");
                    sprintf(size, "%d", i);
                    strcat(key, size);
                    key[strlen(key) + strlen(size)] = 0;
                    json_object_object_add(obj, key, array);
                    PQclear(res);
                    free(query);
                    /*machines[i].id = iid;
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
                    free(query);*/
                    index++;
                }
                buf1 = point + 1;
            }
            /*if (machines[0].id > 0) {
                isEmptyMachines = 0;
            }*/
            /*for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }*/
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: object-machines=\
", request.out);
            FCGX_PutS(json_object_to_json_string(obj), request.out);
            FCGX_PutS("\r\n",request.out);
            if (strcmp(json_object_to_json_string(obj), "{ }")) {
                FCGX_PutS("Set-Cookie: isEmptyMachines=0\r\n", request.out);
                FCGX_PutS("Set-Cookie: size-object-machines=", request.out);
                int temp = atoi(size);
                temp++;
                sprintf(size, "%d", temp);
                FCGX_PutS(size, request.out);
            }
            //printf("%s", json_object_to_json_string(obj));
            FCGX_PutS("\r\n",request.out);
            FCGX_PutS("\r\n",request.out);
        }
        else if (strstr(buf, "editvalues")) {
            machine* machines = getMachines(request);
            /*for (int i = 0; i < atoi(getCookie("size-object-machines", request)); i++) {
                printf("obj:%d;id:%d;ammunition:%d;strength:%d;speed:%d\n", i, machines[i].id, machines[i].ammunition, machines[i].strength, machines[i].speed);
            }*/
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
            ", request.out);
            printObjectsMachines(conn, request, machines, 1, 0);
            FCGX_PutS("\
<div style=\"text-align: center;\"><button id=\"menu\">Ок</button></div>\r\n\
<script src=\"http://localhost/editvalues.js\"></script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
            free(machines);
        }
        else if (strstr(buf, "plusstrength")) {
            machine* machines = getMachines(request);
            char * i = malloc(5);
            strncpy(i, strchr(buf, '=') + 4, strchr(buf, ';') - strchr(buf, '=') - 4);
            i[strchr(buf, ';') - strchr(buf, '=') - 4] = '\0';
            machines[atoi(i)].strength += 1;



            char * query = malloc(300);
            strcpy(query, "select*from warmachine where id=");
            char * reqId = malloc(5);
            sprintf(reqId, "%d", machines[atoi(i)].id);
            strcat(query, reqId);
            strcat(query, ";");
            PGresult *res = PQexec(conn, query);
            char * distance = malloc(10);
            strcpy(distance, PQgetvalue(res, 0, 7));
            free(query);



            char dist[3][5];
            int strength[3];
            readStrengthDamageSpeed(distance, dist);
            for (int i = 0; i < 3; i++) {
                strength[i] = atoi(dist[i]);
            }
            int levelBefore;
            int levelAfter = 1000;
            if (machines[atoi(i)].strength <= strength[0] && machines[atoi(i)].strength > strength[1]) {
                levelAfter = 0;
            }
            else if (machines[atoi(i)].strength <= strength[1] && machines[atoi(i)].strength > strength[2]) {
                levelAfter = 1;
            }
            else if (machines[atoi(i)].strength <= strength[2] && machines[atoi(i)].strength > 0) {
                levelAfter = 2;
            }
            else if (machines[atoi(i)].strength <= 0) {
                levelAfter = 3;
            }
            if (levelAfter == 1000) {
                machines[atoi(i)].strength -= 1;
            }
            query = malloc(300);
            strcpy(query, "select*from warmachine where id=");
            strcat(query, reqId);
            strcat(query, ";");
            res = PQexec(conn, query);
            free(distance);
            distance = malloc(10);
            strcpy(distance, PQgetvalue(res, 0, 8));
            free(query);
            char dist1[3][5];
            int speed[3];
            readStrengthDamageSpeed(distance, dist1);
            for (int i = 0; i < 3; i++) {
                speed[i] = atoi(dist1[i]);
            }
            if (levelAfter < 3 && levelAfter >= 0) {
                machines[atoi(i)].speed = speed[levelAfter];
            }
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: object-machines=\
            ", request.out);
            //printf("%s", createJsonMachines(machines, request));
            FCGX_PutS(createJsonMachines(machines, request), request.out);
            FCGX_PutS("\r\n", request.out);
            FCGX_PutS("\r\n", request.out);
            free(machines);




            /*for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }*/
        }
        else if (strstr(buf, "minusstrength")) {
            machine* machines = getMachines(request);
            char * i = malloc(5);
            strncpy(i, strchr(buf, '=') + 4, strchr(buf, ';') - strchr(buf, '=') - 4);
            i[strchr(buf, ';') - strchr(buf, '=') - 4] = '\0';
            machines[atoi(i)].strength -= 1;




            char * query = malloc(300);
            strcpy(query, "select*from warmachine where id=");
            char * reqId = malloc(5);
            sprintf(reqId, "%d", machines[atoi(i)].id);
            strcat(query, reqId);
            strcat(query, ";");
            PGresult *res = PQexec(conn, query);
            char * distance = malloc(10);
            strcpy(distance, PQgetvalue(res, 0, 7));
            free(query);



            char dist[3][5];
            int strength[3];
            readStrengthDamageSpeed(distance, dist);
            for (int i = 0; i < 3; i++) {
                strength[i] = atoi(dist[i]);
            }
            int levelBefore;
            int levelAfter;
            if (machines[atoi(i)].strength <= strength[0] && machines[atoi(i)].strength > strength[1]) {
                levelAfter = 0;
            }
            else if (machines[atoi(i)].strength <= strength[1] && machines[atoi(i)].strength > strength[2]) {
                levelAfter = 1;
            }
            else if (machines[atoi(i)].strength <= strength[2] && machines[atoi(i)].strength > 0) {
                levelAfter = 2;
            }
            else if (machines[atoi(i)].strength <= 0) {
                levelAfter = 3;
            }
            if (levelAfter == 3) {
                machines[atoi(i)].id = 0;
                FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: size-object-machines=\
                    ", request.out);
                char* ssize = getCookie("size-object-machines", request);
                int size = atoi(ssize) - 1;
                if (size < 0) {
                    size = 0;
                }
                sprintf(ssize, "%d", size);
                FCGX_PutS(ssize, request.out);
                FCGX_PutS("\r\n", request.out);
                if (size == 0) {
                    FCGX_PutS("\
Set-Cookie: isEmptyMachines=1\r\n\
                    ", request.out);
                }
            }
            query = malloc(300);
            strcpy(query, "select*from warmachine where id=");
            strcat(query, reqId);
            strcat(query, ";");
            res = PQexec(conn, query);
            free(distance);
            distance = malloc(10);
            strcpy(distance, PQgetvalue(res, 0, 8));
            free(query);
            char dist1[3][5];
            int speed[3];
            readStrengthDamageSpeed(distance, dist1);
            for (int i = 0; i < 3; i++) {
                speed[i] = atoi(dist1[i]);
            }
            if (levelAfter != 3) {
                machines[atoi(i)].speed = speed[levelAfter];
            }
            //printf("%d", machines[atoi(i)].strength);
            if (levelAfter != 3) {
                FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: object-machines=\
            ", request.out);
            }
            else {
                FCGX_PutS("\
Set-Cookie: object-machines=\
            ", request.out);
            }
            //printf("%s", createJsonMachines(machines, request));
            FCGX_PutS(createJsonMachines(machines, request), request.out);
            FCGX_PutS("\r\n", request.out);
            FCGX_PutS("\r\n", request.out);
            free(machines);



            /*for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }*/
        }
        else if (strstr(buf, "plusammunition")) {
            machine* machines = getMachines(request);
            char * i = malloc(5);
            strncpy(i, strchr(buf, '=') + 4, strchr(buf, ';') - strchr(buf, '=') - 4);
            i[strchr(buf, ';') - strchr(buf, '=') - 4] = '\0';
            machines[atoi(i)].ammunition += 1;
            char * query = malloc(100);
            strcpy(query, "select * from warmachine where id=");
            char * id = malloc(5);
            sprintf(id, "%d", machines[atoi(i)].id);
            strcat(query, id);
            strcat(query, ";");
            PGresult *res = PQexec(conn, query);
            char * ammunition = PQgetvalue(res, 0, 6);
            int iammunition = atoi(ammunition);
            if (machines[atoi(i)].ammunition > iammunition) {
                machines[atoi(i)].ammunition -= 1;
            }
            PQclear(res);
            free(query);
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: object-machines=\
            ", request.out);
            //printf("%s", createJsonMachines(machines, request));
            FCGX_PutS(createJsonMachines(machines, request), request.out);
            FCGX_PutS("\r\n", request.out);
            FCGX_PutS("\r\n", request.out);
            free(machines);
            /*for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }*/
        }
        else if (strstr(buf, "minusammunition")) {
            machine* machines = getMachines(request);
            char * i = malloc(5);
            strncpy(i, strchr(buf, '=') + 4, strchr(buf, ';') - strchr(buf, '=') - 4);
            i[strchr(buf, ';') - strchr(buf, '=') - 4] = '\0';
            machines[atoi(i)].ammunition -= 1;
            if (machines[atoi(i)].ammunition < 0) {
                machines[atoi(i)].ammunition = 0;
            }
            /*for (int i = 0; i < 20; i++) {
                printf("i:%d;id:%d;strength:%d;ammunition:%d;speed:%d;\n", i, machines[i].id, machines[i].strength, machines[i].ammunition, machines[i].speed);
            }*/
            FCGX_PutS("\
Content-type: text/html\r\n\
Set-Cookie: object-machines=\
            ", request.out);
            //printf("%s", createJsonMachines(machines, request));
            FCGX_PutS(createJsonMachines(machines, request), request.out);
            FCGX_PutS("\r\n", request.out);
            FCGX_PutS("\r\n", request.out);
            free(machines);
        }
        else if (strstr(buf, "newgame")) {
            FCGX_PutS("\
Content-type: text/html\r\n\
            ", request.out);
        }
        else if (strstr(buf, "showlogs")) {
            FCGX_PutS("\
Content-type: text/html\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
<link rel=\"stylesheet\" href=\"http://localhost/logs.css\">\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
<div id=\"logs\">\r\n\
</div>\r\n\
<div style=\"text-align: center;\"><button id=\"ok\">Ок</button></div>\r\n\
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
    if (localStorage.getItem(\"logs\")) {\r\n\
        document.getElementById(\"logs\").innerHTML = localStorage.getItem(\"logs\");\r\n\
    }\r\n\
    else {\r\n\
        document.getElementById(\"logs\").innerHTML = \"Здесь будет ваша история битвы\";\r\n\
    }\r\n\
    document.getElementById(\"ok\").addEventListener(\"click\", okButtonListener);\r\n\
    function okButtonListener() {\r\n\
        xhrSend(\"menu\");\r\n\
    }\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
", request.out);
        }
        else {
/*Set-Cookie: abc=123;\r\n\
Set-Cookie: bcde=23456;\r\n\
Set-Cookie: abcd=0123;\r\n\
Set-Cookie: bcddse=red;\r\n\
*/
            int firstTime;
            if (getCookie("firstTime", request) == NULL) {
                firstTime = 1;
            }
            else if (!strcmp(getCookie("firstTime", request), "0")) {
                firstTime = 0;
            }
            FCGX_PutS("\
Content-type: text/html\r\n\
", request.out);
            if (firstTime) {
                FCGX_PutS("Set-Cookie: isEmptyMachines=1\r\n", request.out);
            }
            FCGX_PutS("\
\r\n\
<html>\r\n\
<head>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>Бронепехота</title>\r\n\
</head>\r\n\
<body style=\"background: url(background.jpg) no-repeat; background-size: 100% 100vh; background-attachment: fixed;\">\r\n\
<div style=\"text-align: center;\"><button id=\"newgame\">Новая игра</button></div>\r\n\
<br>\r\n\
<div style=\"text-align: center;\"><button id=\"testshot\">Тест на выстрел</button></div>\r\n\
<br>\r\n\
            ", request.out);
            if (firstTime) {
                FCGX_PutS("\
<div style=\"text-align: center;\"><button id=\"setmachines\">Создать технику</button></div>\r\n\
<br>\r\n\
                ", request.out);
            }
            FCGX_PutS("\
<div style=\"text-align: center;\"><button id=\"editvalues\">Отредактировать боезапас или прочность</button></div>\r\n\
<br>\r\n\
<div style=\"text-align: center;\"><button id=\"logs\">История</button></div>\r\n\
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
    document.getElementById(\'newgame\').addEventListener(\"click\", newgameButtonListener);\r\n\
    document.getElementById(\'testshot\').addEventListener(\"click\", testshotButtonListener);\r\n\
            ", request.out);
            if (firstTime) {
                FCGX_PutS("\
    document.getElementById(\'setmachines\').addEventListener(\"click\", setmachinesButtonListener);\r\n\
                ", request.out);
            }
            FCGX_PutS("\
    document.getElementById(\'editvalues\').addEventListener(\"click\", editvaluesButtonListener);\r\n\
    document.getElementById(\'logs\').addEventListener(\"click\", logsButtonListener);\r\n\
    function newgameButtonListener() {\r\n\
        var answer = confirm(\"Действительно начать новую игру?\");\r\n\
        if (answer) {\r\n\
            document.cookie = \"firstTime = ; expires = Thu, 01 Jan 1970 00:00:00 GMT\";\r\n\
            document.cookie = \"idAttacker = ; expires = Thu, 01 Jan 1970 00:00:00 GMT\";\r\n\
            document.cookie = \"idTarget = ; expires = Thu, 01 Jan 1970 00:00:00 GMT\";\r\n\
            document.cookie = \"isEmptyMachines = ; expires = Thu, 01 Jan 1970 00:00:00 GMT\";\r\n\
            document.cookie = \"object-machines = ; expires = Thu, 01 Jan 1970 00:00:00 GMT\";\r\n\
            document.cookie = \"size-object-machines = ; expires = Thu, 01 Jan 1970 00:00:00 GMT\";\r\n\
            window.location.reload(false);\r\n\
        }\r\n\
    }\r\n\
    function testshotButtonListener() {\r\n\
        xhrSend(\"testshot:chooseattacker;\");\r\n\
    }\r\n\
            ", request.out);
            if (firstTime) {
                FCGX_PutS("\
    function setmachinesButtonListener() {\r\n\
        xhrSend(\"setmachines;\");\r\n\
    }\r\n\
                ", request.out);
            }
            FCGX_PutS("\
    function editvaluesButtonListener() {\r\n\
        xhrSend(\"editvalues;\");\r\n\
    }\r\n\
    function logsButtonListener() {\r\n\
        xhrSend(\"showlogs;\");\r\n\
    }\r\n\
</script>\r\n\
</body>\r\n\
</html>\r\n\
            ", request.out);
        }
    }

    return 0; 
}