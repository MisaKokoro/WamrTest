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
#include "test.h"

uint8_t tmp[4096] = {0};
uint8_t out[4096] = {0};

enum {
    STR_REVERSE,
    STR_REVERSE_SER,
    STR_REVERSE_UNSER,
    FIB,
    FIB_SER,
    FIB_UNSER,
    BUILD_USER_ID,
    BUILD_USER_ID_SER,
    BUILD_USER_ID_UNSER,
};
int cnt = 0;
const char *TestCase[] = {
    "str_reverse","str_reverse_ser","str_reverse_unser",
    "fib","fib_ser","fib_unser",
    "build_user_id", "build_user_id_ser", "build_user_id_unser",
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
        case FIB:
            test_fib();
            break;
        case FIB_SER:
            test_fib_ser();
            break;
        case FIB_UNSER:
            test_fib_unser();
            break;
        case BUILD_USER_ID:
            test_build_user_id();
            break;
        case BUILD_USER_ID_SER:
            test_build_user_id_ser();
            break;
        case BUILD_USER_ID_UNSER:
            test_build_user_id_unser();
            break;
        default:
            printf("no this test!\n");
            exit(EXIT_FAILURE);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);


    long long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long long na = diff % 1000;
    long long micro = diff / 1000;
    printf("native + serialization: test case: %s  cnt:%d \t\tcost:%lld.%03lldus\n",test_name,cnt,micro,na);
}

void _str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

int str_reverse(uint8_t *buf,int size) {
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
    uint8_t buffer[2048];

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
    uint8_t buffer[2048];

    for (int i = 0; i < cnt; i++) {
        Person test;
        person__init(&test);
        test.name = str;
        int size = person__pack(&test,buffer);
    }
}

void test_str_reverse_unser() {
    char str[] = "osmgoqmclgtjkakv";
    uint8_t buffer[2048];
    Person test;
    person__init(&test);
    test.name = str;
    int size = person__pack(&test,buffer);

    for (int i = 0; i < cnt; i++) {
        Person *p = person__unpack(NULL,size,buffer);
    }
}

int _fib(int n) {
    if (n <= 1) {
        return n;
    }
    return _fib(n - 1) + _fib(n - 2);
}

int fib(uint8_t *buf,int size) {
    Fib *p = fib__unpack(NULL,size,buf);
    p->num = _fib(p->num);
    int res = fib__pack(p,out);
    return res;
}

void test_fib() {
    uint8_t buffer[2048];
    for (int i = 0; i < cnt; i++) {
        Fib test;
        fib__init(&test);
        test.num = 42;
        int size = fib__pack(&test,buffer);
        size = fib(buffer,size);
        Fib *p = fib__unpack(NULL,size,out);
        printf("p->num = %d\n",p->num);
    }
}

void test_fib_ser() {
    uint8_t buffer[2048];
    for (int i = 0; i < cnt; i++) {
        Fib test;
        fib__init(&test);
        test.num = 42;
        int size = fib__pack(&test,buffer);
    }
}

void test_fib_unser() {
    uint8_t buffer[2048];
    Fib test;
    fib__init(&test);
    test.num = 42;
    int size = fib__pack(&test,buffer);

    for (int i = 0; i < cnt; i++) {
        Fib *p = fib__unpack(NULL,size,buffer);
        printf("p->num = %d\n",p->num);
    }
}

unsigned int hash(char *str){
    register unsigned int h;
    register unsigned char *p; 
    for(h=0, p = (unsigned char *)str; *p ; p++)
    h = 31 * h + *p; 
    return h;
}

char *_build_user_id(char *user_id, char *imei_id){
    if(user_id != NULL && user_id != 0)
        return user_id;
    else if(imei_id != NULL && imei_id != ""){
        sprintf(user_id, "%u", hash(imei_id));
        return user_id;
    }else
        return "0";
}

int build_user_id(uint8_t *buf, int size){
    UserId *user_id = user_id__unpack(NULL, size, buf);
    user_id->user_id = _build_user_id(user_id->user_id, user_id->imei_id);
    return fib__pack(user_id, out);
}

void test_build_user_id(){
    uint8_t buffer[2048];
    char user_id[] = "23172736737";
    char imei_id[] = "94390520635091";
    for (int i = 0; i < cnt; i++) {
        UserId user_id_input;
        user_id__init(&user_id_input);
        user_id_input.user_id = user_id;
        user_id_input.imei_id = imei_id;

        int size = user_id__pack(&user_id_input,buffer);
        size = build_user_id(buffer,size);
        UserId *user_id_output = user_id__unpack(NULL,size,out);
    }
}

void test_build_user_id_ser(){
    uint8_t buffer[2048];
    for (int i = 0; i < cnt; i++) {
        UserId user_id_input;
        user_id__init(&user_id_input);
        user_id_input.user_id = "23172736737";
        user_id_input.imei_id = "94390520635091";
        int size = user_id__pack(&user_id_input,buffer);
    }
}

void test_build_user_id_unser(){
    uint8_t buffer[2048];
    UserId user_id_input;
    user_id__init(&user_id_input);
    user_id_input.user_id = "23172736737";
    user_id_input.imei_id = "94390520635091";
    int size = user_id__pack(&user_id_input,buffer);

    for (int i = 0; i < cnt; i++) {
        UserId *user_id_output = user_id__unpack(NULL,size,buffer);
    }
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
    }
    return res;
}   

int spliter(const uint8_t *buf, size_t size, size_t array_len){
    Spliter *spliter = spliter__unpack(NULL, size, buf);
    spliter->data =  _spliter(spliter->data[0], array_len);
    spliter->n_data = array_len;
    return spliter__pack(spliter, out);
}

void test_spliter(){
    uint8_t buffer[2048];
    for (int i = 0; i < cnt; i++) {
        Spliter input;
        spliter__init(&input);
        input.data = (char **)malloc(sizeof(char *) * 1);
        input.data[0] = "I am asb";
        input.n_data = 1;

        int size = user_id__pack(&input, buffer);
        size_t length = 3;
        size = spliter(buffer,size, length);
        Spliter *output = spliter__unpack(NULL,size,out);
        for(int j=0; j < output->n_data; j++)
            printf("spliter str = %s\n",output->data[j]);
    }
}

void test_spliter_ser(){
    uint8_t buffer[2048];
    for (int i = 0; i < cnt; i++) {
        Spliter input;
        spliter__init(&input);
        input.data = (char **)malloc(sizeof(char *) * cnt);
        input.data[0] = "I am asb";
        input.n_data = 1;
        int size = user_id__pack(&input, buffer);
    }
}

void test_spliter_unser(){
    uint8_t buffer[2048];
    Spliter input;
    spliter__init(&input);
    input.data = (char **)malloc(sizeof(char *) * cnt);
    input.data[0] = "I am asb";
    input.n_data = 1;
    int size = user_id__pack(&input, buffer);

    for (int i = 0; i < cnt; i++) {
        Spliter *output = spliter__unpack(NULL,size,buffer);
    }
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

void test_point_polygen_distance(){
    uint8_t buffer[2048];
    double point1[3] = {55.019369276597246,56.04653136999801,61.32173119067093};
    double point2[3] = {2.5611362913548987,36.674703766827236,-18.76548358360523};
    for (int i = 0; i < cnt; i++) {
        PointPolygenDistance input;
        point_polygen_distance__init(&input);
        input.point1 = point1;
        input.point2 = point2;
        input.n_point1 = 3;
        input.n_point2 = 3;

        int size = point_polygen_distance__pack(&input, buffer);
        size = point_polygen_distance(buffer,size);
        PointPolygenDistance *output = point_polygen_distance__unpack(NULL,size,out);
        printf("distance: %lf\n", output->distance);
    }
}

void test_point_polygen_distance_ser(){
    uint8_t buffer[2048];
    double point1[3] = {55.019369276597246,56.04653136999801,61.32173119067093};
    double point2[3] = {2.5611362913548987,36.674703766827236,-18.76548358360523};
    for (int i = 0; i < cnt; i++) {
        PointPolygenDistance input;
        point_polygen_distance__init(&input);
        input.point1 = point1;
        input.point2 = point2;
        input.n_point1 = 3;
        input.n_point2 = 3;
        int size = point_polygen_distance__pack(&input, buffer);
    }
}

void test_point_polygen_distance_unser(){
    uint8_t buffer[2048];
    double point1[3] = {55.019369276597246,56.04653136999801,61.32173119067093};
    double point2[3] = {2.5611362913548987,36.674703766827236,-18.76548358360523};
    PointPolygenDistance input;
    point_polygen_distance__init(&input);
    input.point1 = point1;
    input.point2 = point2;
    input.n_point1 = 3;
    input.n_point2 = 3;
    int size = point_polygen_distance__pack(&input, buffer);
    for (int i = 0; i < cnt; i++) {
        PointPolygenDistance *output = point_polygen_distance__unpack(NULL,size,buffer);
        printf("distance: %lf\n", output->distance);
    }
}