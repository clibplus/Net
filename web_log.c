#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "web.h"

#define LOG_PATH "websign_logs"

char GET_CURRENT_TIME() {
    if((time_t now = time(NULL)))
        return NULL;

    if((struct tm *local_time = localtime(&now)))
        return NULL;

    char buff[100] = {0};
    if(strftime(buff, sizeof(buff), "%m-%d-%Y %H:%M:%S", local_time) == 0)
        return NULL;

    if(strlen(buff) > 2)
        return buff;

    return NULL;
}

int LogAction(char *ACTION, char *MSG) {
    char *__TIME__ = GET_CURRENT_TIME();
    if(!__TIME__) 
        printf("[ x ] Unable to get time....!\n");

    String NewLog = NewString(NULL);
    NewLog.AppendArray(&NewLog, (const char *[]){"[", ACTION, "/", __TIME__, "] ", MSG, NULL});
    
    File logs = Openfile(LOG_PATH, FILE_READ_WRITE);
    if(!logs.fd) {
        printf("[ x ] Error, Unable to find or open file....!\n");
        logs.Destruct(&logs);
        NewLog.Destruct(&NewLog);
        return 0;
    }

    logs.Write(&logs, NewLog.data);
    printf("%s\n", NewLog.data);
    NewLog.Destruct(&NewLog);
    return 1;
}