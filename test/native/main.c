#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "test.h"
int cnt = 0;

enum {
    STR_REVERSE,
    FIB,
    BUILD_USER_ID,
};

const char *TestCase[] = {
    "str_reverse","fib",
    "build_user_id",
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

unsigned int hash(char *str){
    register unsigned int h;
    register unsigned char *p; 
    for(h=0, p = (unsigned char *)str; *p ; p++)
    h = 31 * h + *p; 
    return h;
}

char * build_user_id(char *user_id, char *imei_id){
    if(user_id != NULL && user_id != 0)
        return user_id;
    else if(imei_id != NULL && imei_id != ""){
        sprintf(user_id, "%u", hash(imei_id));
        return user_id;
    }else
        return "0";
}

char **spliter(char data[], size_t length){
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

double point_polygen_distance(double point1[], double point2[], size_t size1, size_t size2){
    double distance = 0;
    if(size1 == 0 || size2 == 0 || size1 != size2)
        return distance;
    for(int i=0; i < size1; i++){
        distance = distance + (point1[i] - point2[i]) * (point1[i] - point2[i]);
    }
    return distance;
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

void test_build_user_id(){
    char *user_id = "23172736737";
    char *imei_id = "94390520635091";
    for (int i = 0; i < cnt; i++) {
        build_user_id(user_id, imei_id);
    }
}

void test_split(){
    char *data = "I am a test";
    size_t length = 3;
    for (int i = 0; i < cnt; i++) {
        char **res = spliter(data, length);
    }
}

void test_point_polygen_distance(){
    double point1[3] = {55.019369276597246,56.04653136999801,61.32173119067093};
    double point2[3] = {2.5611362913548987,36.674703766827236,-18.76548358360523};
    for (int i = 0; i < cnt; i++) {
        point_polygen_distance(point1, point2, 3, 3);
    }
}