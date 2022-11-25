/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char buff[1024] = {0};
void* ringBufferHead;
void* ringBufferTail;



//将长为len的buffer写入缓冲区
void writeBuffer(void* wasmPtr,char *buffer,uint32_t len) {
    if (len == 0) {
        return;
    }
    int preLen = ringBufferTail - wasmPtr;
    int lastLen = len - preLen;
    memcpy(wasmPtr,buff,preLen);
    memcpy(ringBufferHead,buff + preLen,lastLen);
}
//从缓冲区中读取len长度的数据到buffer
void readBuffer(void* wasmPtr,char *buffer,uint32_t len) {
    if (len == 0) {
        return;
    }
    int preLen = ringBufferTail - wasmPtr;
    int lastLen = len - preLen;
    memcpy(buff,wasmPtr,preLen);
    memcpy(buff + preLen,ringBufferHead,lastLen);

}

void _str_reverse(char *str,int len) {
    for (int i = 0; i < len / 2; i++) {
            char t = str[i];
            str[i] = str[len - 1 - i];
            str[len - 1 - i] = t;
    }
}

void *str_reverse(char *str,int len) {
    
    //如果没有越过ringbuffer末尾，那么可以原地操作
    if ((uint32_t)str + len <= (uint32_t)ringBufferTail) {
        _str_reverse(str,len);
    } else {
        readBuffer(str,buff,len + 1);
        _str_reverse(buff,len);
        writeBuffer(str,buff,len + 1);
    }
    return str;
}

int add(int a,int b) {
    return a + b;
}

void init_ringbuffer(void* head,void* tail) {
    ringBufferHead = head;
    ringBufferTail = tail;
}