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
void* ringBufferHead;
void* ringBufferTail;
//所有要发送出去的数据都放在out里
uint8_t *out;



//将长为len的buffer写入缓冲区
void writeBuffer(void* wasmPtr,uint32_t len) {
    if (len == 0) {
        return;
    }
    if (wasmPtr + len > ringBufferTail) {
        int preLen = ringBufferTail - wasmPtr;
        int lastLen = len - preLen;
        memcpy(wasmPtr,tmp,preLen);
        memcpy(ringBufferHead,tmp + preLen,lastLen);
    } else {
        memcpy(wasmPtr,tmp,len);
    }
}
//从缓冲区中读取len长度的数据到buffer
void readBuffer(void* wasmPtr,uint32_t len) {
    if (len == 0) {
        return;
    }
    if (wasmPtr + len > ringBufferTail) {
        int preLen = ringBufferTail - wasmPtr;
        int lastLen = len - preLen;
        memcpy(tmp,wasmPtr,preLen);
        memcpy(tmp + preLen,ringBufferHead,lastLen);
    } else {
        memcpy(tmp,wasmPtr,len);
    }
}

void _str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

int str_reverse(const uint8_t *buf,int size) {
    

    //将字节数组反序列化成Person
    struct Person *per;
    per = person__unpack(NULL,size,buf);
    int len = strlen(per->name);
    //反转字符串
    _str_reverse(per->name,len);
    //将处理好后的person反序列化到out
    int res = person__pack(per,(void*)out);
    return res;
}

int add(int a,int b) {
    return a + b;
}

void init_ringbuffer(void* head,void* tail,void *out_ptr) {
    ringBufferHead = head;
    ringBufferTail = tail;
    out            = out_ptr;
}

//不能nostdlib编译，因此加上main函数
//nostdlib编译会出现一个没有实现的函数__assert_fail,导致程序无法运行
int main(int argc,char *argv[]) {
    // char *str = "osmgoqmclgtjkakv";

    // char buffer[2048];
    
    // struct timespec beginTime;
    // struct timespec endTime;
    // int cnt = atoi(argv[1]);

    // Person test;
    // person__init(&test);
    // test.name = str;
    // int size = person__pack(&test,buffer);

    // clock_gettime(CLOCK_REALTIME, &beginTime);
    // for (int i = 0; i < cnt; i++) {
    //     Person *p = person__unpack(NULL,size,buffer);
    //     // printf("p->name = %s\n",p->name);
    // }
    // clock_gettime(CLOCK_REALTIME,&endTime);

    // long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    // long na = diff % 1000;
    // long micro = diff / 1000;
    // printf("person pack cnt:%d \t\tcost:%ld.%03ldus\n",cnt,micro,na);

    return 0;
}