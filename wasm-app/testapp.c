/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "struct.pb-c.h"

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

int _fib(int n) {
    if (n <= 1) {
        return n;
    }
    return _fib(n - 1) + _fib(n - 2);
}

int fib(const uint8_t *buf,int size) {
    Fib *p = fib__unpack(NULL,size,buf);
    p->num = _fib(p->num);
    int res = fib__pack(p,out);
    return res;
}

unsigned int hash(char *str){
    register unsigned int h;
    register unsigned char *p; 
    for(h=0, p = (unsigned char *)str; *p ; p++)
    h = 31 * h + *p; 
    return h;
}

char* _build_user_id(char *user_id, char *imei_id){
    if(user_id != NULL && user_id != 0)
        return user_id;
    else if(imei_id != NULL && imei_id != ""){
        sprintf(user_id, "%u", hash(imei_id));
        return user_id;
    }else
        return "0";
}

int build_user_id(const uint8_t *buf, int size){
    UserId *user_id = user_id__unpack(NULL, size, buf);
    user_id->user_id = _build_user_id(user_id->user_id, user_id->imei_id);
    return fib__pack(user_id, out);
}

char **_spliter(char data[], size_t length){
    // char *sep = "\x01";
    char *sep = " ";
    char *res[length];
    char *exclude = "am";
    res[0] = strtok(data, sep);
    for(int i=0; res[i] != NULL; i++){
        res[i+1] = strtok(NULL, sep);
    }
    for(int i=0; i<length; i++){
        if(strcmp(res[i], exclude) == 0)
            res[i] = NULL;
        printf("res %d: %s\n", i, res[i]);
    }
    return res;
}   

int spliter(const uint8_t *buf, size_t size, size_t array_len){
    Spliter *spliter = spliter__unpack(NULL, size, buf);
    spliter->data =  _spliter(spliter->data[0], array_len);
    spliter->n_data = array_len;
    return spliter__pack(spliter, out);
}

double _point_polygen_distance(double point1[], double point2[], size_t size1, size_t size2){
    double distance = 0;
    if(size1 == 0 || size2 == 0 || size1 != size2)
        return distance;
    for(int i=0; i < size1; i++){
        distance = distance + (point1[i] - point2[i]) * (point1[i] - point2[i]);
    }
    return distance;
}
int point_polygen_distance(const uint8_t *buf, size_t size){
    PointPolygenDistance *point = point_polygen_distance__unpack(NULL, size, buf);
    point->distance = _point_polygen_distance(point->point1, point->point2, point->n_point1, point->n_point2);
    return point_polygen_distance__pack(point, out);
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