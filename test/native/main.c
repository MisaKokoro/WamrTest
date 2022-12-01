#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

int main(int argc,char *argv[]) {
    int cnt = atoi(argv[1]);
    char str[] = "osmgoqmclgtjkakv";
    struct timespec beginTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &beginTime);
    for (int i = 0; i < cnt; i++) {
        str_reverse(str,sizeof(str) - 1);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);

    long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long na = diff % 1000;
    long micro = diff / 1000;
    printf("str = %s\n",str);
    printf("native cnt:%d \t\tcost:%ld.%03ldus\n",cnt,micro,na);
}