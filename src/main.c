
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "wasm_export.h"
#include "bh_read_file.h"
#include "bh_getopt.h"

#include <time.h>
#include <sys/time.h>

#define BUFF_MAX_LEN 8192
#define VOS_OK 0
#define VOS_ERR -1


void
print_usage(void)
{
    fprintf(stdout, "Options:\r\n");
    fprintf(stdout, "[cnt of malloc] [bytes of malloc] \n");
}


char *pHead = NULL;			//环形缓冲区首地址
char *pValidRead = NULL;	//已使用环形缓冲区首地址
char *pValidWrite = NULL;	//已使用环形缓冲区尾地址
char *pTail = NULL;			//环形缓冲区尾地址
// uint32_t wasm_buffer_base = 0;
//环形缓冲区初始化
uint32_t InitRingBuff(wasm_module_inst_t module_inst)
{
    uint32_t wasm_buffer_base = 0;
	if(NULL == pHead)
	{
		wasm_buffer_base = wasm_runtime_module_malloc(module_inst,BUFF_MAX_LEN,(void**)&pHead);
	}
	
	memset(pHead, 0 , sizeof(BUFF_MAX_LEN));
	
	pValidRead = pHead;
	pValidWrite = pHead;
	pTail = pHead + BUFF_MAX_LEN;

    return wasm_buffer_base;
}

//环形缓冲区释放
void FreeRingBuff(wasm_module_inst_t module,uint32_t wasmPtr)
{
	if(NULL != pHead)
	{
		wasm_runtime_module_free(module,wasmPtr);
	}
}

//向缓冲区写数据
int WriteRingBuff(char *pBuff, int AddLen)
{
	if(NULL == pHead)
	{
		printf("WriteRingBuff:RingBuff is not Init!\n");
		return VOS_ERR;
	}

	if(AddLen > pTail - pHead)
	{
		printf("WriteRingBuff:New add buff is too long\n");	
		return VOS_ERR;
	}
	
	//若新增的数据长度大于写指针和尾指针之间的长度
	if(pValidWrite + AddLen > pTail)
	{
		int PreLen = pTail - pValidWrite;
		int LastLen = AddLen - PreLen;
		memcpy(pValidWrite, pBuff, PreLen);	
		memcpy(pHead, pBuff + PreLen, LastLen);
		
		pValidWrite = pHead + LastLen;	//新环形缓冲区尾地址
	}
	else
	{
		memcpy(pValidWrite, pBuff, AddLen);	//将新数据内容添加到缓冲区
		pValidWrite += AddLen;	//新的有效数据尾地址
	}
	return VOS_OK;
}

//从缓冲区读数据
int ReadRingBuff(char *pBuff, int len)
{
	if(NULL == pHead)
	{
		printf("ReadRingBuff:RingBuff is not Init!\n");
		return VOS_ERR;
	}
	if(len > pTail - pHead)
	{
		printf("ReadRingBuff:Read buff size is too long\n");	
		return VOS_ERR;
	}
	if(0 == len)
	{
		return VOS_OK;
	}
	if(pValidRead + len > pTail)
	{
		int PreLen = pTail - pValidRead;
		int LastLen = len - PreLen;
		memcpy(pBuff, pValidRead, PreLen);
		memcpy(pBuff + PreLen, pHead, LastLen);
		
		pValidRead = pHead + LastLen;
	}
	else
	{
		memcpy(pBuff, pValidRead, len);
		pValidRead += len;
	}
	
	return len;
}
int
main(int argc, char *argv_main[])
{
    if (argc != 2) {
        print_usage();
        return 0;
    }
    static char global_heap_buf[4096 * 1024];
    char *buffer, error_buf[128];
    char *wasm_path = "/home/yanxiang/Desktop/WamrTest/build/wasm-app/testapp.aot";

    wasm_module_t module = NULL;
    wasm_module_inst_t module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    uint32 buf_size, stack_size = 8092, heap_size = (uint32)2147483648;
    wasm_function_inst_t str_reverse_func = NULL;
    wasm_function_inst_t add_func = NULL;
    wasm_function_inst_t init_ringbuffer_func = NULL;
    char *native_buffer = NULL;
    uint32_t wasm_buffer = 0;

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        return -1;
    }

    buffer = bh_read_file_to_buffer(wasm_path, &buf_size);

    if (!buffer) {
        printf("Open wasm app file [%s] failed.\n", wasm_path);
        goto fail;
    }

    module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));
    if (!module) {
        printf("Load wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    module_inst = wasm_runtime_instantiate(module, stack_size, heap_size,
                                           error_buf, sizeof(error_buf));

    if (!module_inst) {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if (!exec_env) {
        printf("Create wasm execution environment failed.\n");
        goto fail;
    }

    if (!(str_reverse_func = wasm_runtime_lookup_function(module_inst, "str_reverse",
                                              NULL))) {
        printf("The str_reverse wasm function is not found.\n");
        goto fail;
    }

    if (!(add_func = wasm_runtime_lookup_function(module_inst, "add",
                                              NULL))) {
        printf("The add wasm function is not found.\n");
        goto fail;
    }

    if (!(init_ringbuffer_func = wasm_runtime_lookup_function(module_inst, "init_ringbuffer",
                                              NULL))) {
        printf("The init_ringbuffer wasm function is not found.\n");
        goto fail;
    }


    struct timespec beginTime;
    struct timespec endTime;
    char my_buffer[] = "osmgoqmclgtjkakv";
    int cnt = atoi(argv_main[1]);
    int bytes = sizeof(my_buffer);
    InitRingBuff(module_inst);
    uint32_t ring_head = wasm_runtime_addr_native_to_app(module_inst,pHead);
    uint32_t ring_tail = wasm_runtime_addr_native_to_app(module_inst,pTail);
    uint32_t tmp[2] = {ring_head,ring_tail};
    wasm_runtime_call_wasm(exec_env,init_ringbuffer_func,2,tmp);
    clock_gettime(CLOCK_REALTIME, &beginTime);
    for (int i = 0; i < cnt; i++) {
        WriteRingBuff(my_buffer,sizeof(my_buffer));
        wasm_buffer = wasm_runtime_addr_native_to_app(module_inst,pValidRead);
        if (wasm_buffer == 0) {
            printf("unkonwn error \n");
            return 0;
        }
        uint32 argv2[2] = {wasm_buffer,sizeof(my_buffer) - 1};
        if (wasm_runtime_call_wasm(exec_env, str_reverse_func, 2, argv2)) {
            // printf("Native finished calling wasm function: str_reverse, "
            //     "returned a reversed string: %s\n",
            //         native_buffer);
        }
        else {
            printf("call wasm function str_reverse failed. error: %s\n",
                wasm_runtime_get_exception(module_inst));
            goto fail;
        }
        ReadRingBuff(my_buffer,sizeof(my_buffer));
        printf("mybuffer = %s\n",my_buffer);
        // wasm_runtime_module_free(module_inst,wasm_buffer);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);

    long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long na = diff % 1000;
    long micro = diff / 1000;
    printf("malloc and set and run : size:%dK cnt:%d \t\tcost:%ld.%03ldus\n",bytes,cnt,micro,na);

fail:
    if (exec_env)
        wasm_runtime_destroy_exec_env(exec_env);
    if (module_inst) {
        if (wasm_buffer)
            wasm_runtime_module_free(module_inst, wasm_buffer);
        wasm_runtime_deinstantiate(module_inst);
    }
    if (module)
        wasm_runtime_unload(module);
    if (buffer)
        BH_FREE(buffer);
    wasm_runtime_destroy();
    return 0;
}
