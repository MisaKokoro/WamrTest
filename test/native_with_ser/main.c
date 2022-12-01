/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "person.pb-c.h"
#include "test.h"

char tmp[4096] = {0};
char out[4096] = {0};

enum {
    STR_REVERSE,
    STR_REVERSE_SER,
    STR_REVERSE_UNSER,


};
int cnt = 0;
const char *TestCase[] = {
    "str_reverse","str_reverse_ser","str_reverse_unser",

};
int main(int argc,char *argv[]) {

    cnt = atoi(argv[2]);
    int id = atoi(argv[1]);
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
        case STR_REVERSE_SER:
            test_str_reverse_ser();
            break;
        case STR_REVERSE_UNSER:
            test_str_reverse_unser();
            break;
        default:
            printf("no this test!\n");
            exit(EXIT_FAILURE);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);


    long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long na = diff % 1000;
    long micro = diff / 1000;
    printf("native + serialization: test case: %s  cnt:%d \t\tcost:%ld.%03ldus\n",test_name,cnt,micro,na);
}

void _str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

int str_reverse(char *buf,int size) {
    struct Person *per;
    per = person__unpack(NULL,size,buf);
    int len = strlen(per->name);
    //反转字符串
    _str_reverse(per->name,len);
    //将处理好后的person反序列化到out
    int res = person__pack(per,out);
    return res;
}

void test_str_reverse() {
    char str[] = "osmgoqmclgtjkakv";
    char buffer[2048];

    for (int i = 0; i < cnt; i++) {
        Person test;
        person__init(&test);
        test.name = str;
        int size = person__pack(&test,buffer);

        str_reverse(buffer,size);

        Person *p = person__unpack(NULL,size,out);
    }
}

void test_str_reverse_ser() {
    char str[] = "osmgoqmclgtjkakv";
    char buffer[2048];
    struct timespec beginTime;
    struct timespec endTime;

    for (int i = 0; i < cnt; i++) {
        Person test;
        person__init(&test);
        test.name = str;
        int size = person__pack(&test,buffer);
    }
}

void test_str_reverse_unser() {
    char str[] = "osmgoqmclgtjkakv";
    char buffer[2048];
    Person test;
    person__init(&test);
    test.name = str;
    int size = person__pack(&test,buffer);

    for (int i = 0; i < cnt; i++) {
        Person *p = person__unpack(NULL,size,buffer);
    }
}

