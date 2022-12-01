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

char tmp[4096] = {0};
char out[4096] = {0};

void _str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

void *str_reverse(char *buf,int size) {
    

    struct Person *per;
    per = person__unpack(NULL,size,buf);
    int len = strlen(per->name);
    //反转字符串
    _str_reverse(per->name,len);
    //将处理好后的person反序列化到out
    person__pack(per,out);

}

int main(int argc,char *argv[]) {
    char str[] = "osmgoqmclgtjkakv";

    char buffer[2048];
    
    struct timespec beginTime;
    struct timespec endTime;
    int cnt = atoi(argv[1]);
    int size = 0;

        str_reverse(buffer,size);

    clock_gettime(CLOCK_REALTIME, &beginTime);
    for (int i = 0; i < cnt; i++) {

        Person test;
        person__init(&test);
        test.name = str;
        size = person__pack(&test,buffer);
        // str_reverse(buffer,size);

        // printf("p->name = %s\n",p->name);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);
        Person *p = person__unpack(NULL,size,out);



    long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long na = diff % 1000;
    long micro = diff / 1000;
    printf("out = %s\n",out);
    printf("wasm + serialization cnt:%d \t\tcost:%ld.%03ldus\n",cnt,micro,na);

    return 0;
}