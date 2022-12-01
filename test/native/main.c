#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "test.h"
int cnt = 0;

enum {
    STR_REVERSE,
    FIB,

};

const char *TestCase[] = {
    "str_reverse","fib",
};

int main(int argc,char *argv[]) {
    
    int id = atoi(argv[1]);
    cnt = atoi(argv[2]);
    test_main(id,TestCase[id]);

    return 0;
}

void test_main(int id,const char *test_name) {
    struct timespec beginTime;
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &beginTime);

    switch (id) {
        case STR_REVERSE:
            test_str_reverse();
            break;
        case FIB:
            test_fib();
            break;
        default:
            printf("no this test!\n");
            exit(EXIT_FAILURE);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);


    long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long na = diff % 1000;
    long micro = diff / 1000;
    printf("native  test case: %s  cnt:%d \t\tcost:%ld.%03ldus\n",test_name,cnt,micro,na);
}

void str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 2) + fib(n - 1);
}

void test_str_reverse() {
    char str[] = "osmgoqmclgtjkakv";
    for (int i = 0; i < cnt; i++) {
        str_reverse(str,sizeof(str) - 1);
    }
}

void test_fib() {
    int res;
    for (int i = 0; i < cnt; i++) {
        res = fib(42);
    }
    printf("res = %d\n",res);
}