
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */
#include "wasm_export.h"
#include "bh_read_file.h"
#include "bh_getopt.h"
#include "struct.pb-c.h"
#include "test.h"

#include <time.h>
#include <sys/time.h>

#define BUFF_MAX_LEN 8192
#define VOS_OK 0
#define VOS_ERR -1

//测试相关参数
int cnt = 0;
void *native_buffer;
uint32_t wasm_buffer = 0;
//运行函数相关参数
wasm_module_t module = NULL;
wasm_module_inst_t module_inst = NULL;
wasm_exec_env_t exec_env = NULL;
uint32 buf_size, stack_size = 8192, heap_size = 8192;
wasm_function_inst_t benchMarkFunc = NULL;
wasm_function_inst_t init_ringbuffer_func = NULL;

//环形缓冲区相关参数
char *pHead = NULL;			//环形缓冲区首地址
char *pValidRead = NULL;	//已使用环形缓冲区首地址
char *pValidWrite = NULL;	//已使用环形缓冲区尾地址
char *pTail = NULL;			//环形缓冲区尾地址
//wasm函数运行后的序列化数组通过这个指针获取
void *pResult = NULL;       //存放结果的首指针

enum {
    STR_REVERSE,STR_REVERSE_EXEC,
    FIB,FIB_EXEC,
};
const char *TestCase[] = {
    "str_reverse","str_reverse_exec",
    "fib","fib_exec",
};

const char *FuncName[] = {
    "str_reverse","_str_reverse",
    "fib","_fib",
};
void
print_usage(void)
{
    fprintf(stdout, "Options:\r\n");
    fprintf(stdout, "[id of test case] [cnt of run times] \n");
}



// uint32_t wasm_buffer_base = 0;
//环形缓冲区初始化
uint32_t InitRingBuff(wasm_module_inst_t module_inst)
{
    uint32_t wasm_buffer_base = 0;
	if(NULL == pHead)
	{
		wasm_buffer_base =  wasm_runtime_module_malloc(module_inst,BUFF_MAX_LEN,(void**)&pHead);
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

int main(int argc, char *argv_main[])
{
    if (argc != 3) {
        print_usage();
        return 0;
    }
    static char global_heap_buf[4096 * 1024];
    char *buffer, error_buf[128];
    char *wasm_path = "/home/yanxiang/Desktop/WamrTest-probuf/build/wasm-app/testapp.aot";



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

    if (!(init_ringbuffer_func = wasm_runtime_lookup_function(module_inst, "init_ringbuffer",
                                              NULL))) {
        printf("The init_ringbuffer wasm function is not found.\n");
        goto fail;
    }

    //查找导出的函数
    int id = atoi(argv_main[1]);
    cnt = atoi(argv_main[2]);
    if (!(benchMarkFunc = wasm_runtime_lookup_function(module_inst, FuncName[id],
                                              NULL))) {
        printf("The %s wasm function is not found.\n",FuncName[id]);
        goto fail;
    }

    InitRingBuff(module_inst);
    wasm_runtime_module_malloc(module_inst,BUFF_MAX_LEN,(void**)&pResult);
    uint32_t ring_head = wasm_runtime_addr_native_to_app(module_inst,pHead);
    uint32_t ring_tail = wasm_runtime_addr_native_to_app(module_inst,pTail);
    uint32_t out_ptr   = wasm_runtime_addr_native_to_app(module_inst,pResult);
    uint32_t tmp[3] = {ring_head,ring_tail,out_ptr};
    wasm_runtime_call_wasm(exec_env,init_ringbuffer_func,3,tmp);

    wasm_buffer =  wasm_runtime_module_malloc(module_inst,BUFF_MAX_LEN,(void**)&native_buffer);
    if (wasm_buffer == 0) {
        printf("unkonwn error \n");
        return 0;
    }


    test_main(id,TestCase[id]);


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

void test_main(int id,const char *test_name) {
    struct timespec beginTime;
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &beginTime);

    switch (id) {
        case STR_REVERSE:
            test_str_reverse();
            break;
        case STR_REVERSE_EXEC:
            test_str_reverse_exec();
            break;
        case FIB:
            test_fib();
            break;
        case FIB_EXEC:
            test_fib_exec();
            break;
        default:
            printf("no this test!\n");
            exit(EXIT_FAILURE);
    }
    clock_gettime(CLOCK_REALTIME,&endTime);


    long long diff = (endTime.tv_sec-beginTime.tv_sec)*1000000000 + (endTime.tv_nsec - beginTime.tv_nsec);
    long long na = diff % 1000;
    long long micro = diff / 1000;
    printf("API  test case: %s  cnt:%d \t\tcost:%lld.%03lldus\n",test_name,cnt,micro,na);
}

void test_str_reverse() {
    char my_str[] = "osmgoqmclgtjkakv";
    for (int i = 0; i < cnt; i++) {
        // 创建test，并将其序列化
        Person test;
        person__init(&test);
        test.name = my_str;
        int size = person__pack(&test,native_buffer);

        //传递参数，开始执行
        uint32_t argv[2] = {wasm_buffer,size};
        if (wasm_runtime_call_wasm(exec_env, benchMarkFunc, 2, argv)) {
            // printf("run str_reverse ok \n");
        }
        else {
            printf("call wasm function str_reverse failed. error: %s\n",
                wasm_runtime_get_exception(module_inst));
            exit(EXIT_FAILURE);
        }
        //将得到的结果反序列化
        size = argv[0];
        Person *p = person__unpack(NULL,size,pResult);
        // printf("p->name = %s\n",p->name);
    }
}

void test_str_reverse_exec() {
    char my_str[] = "osmgoqmclgtjkakv";
    for (int i = 0; i < cnt; i++) {
        uint32_t argv[2] = {wasm_buffer,sizeof(my_str) - 1};
        memcpy(native_buffer,my_str,sizeof(my_str));
        if (wasm_runtime_call_wasm(exec_env, benchMarkFunc, 2, argv)) {
            // printf("run str_reverse ok \n");
        }
        else {
            printf("call wasm function str_reverse failed. error: %s\n",
                wasm_runtime_get_exception(module_inst));
            exit(EXIT_FAILURE);
        }
        // printf("after: str = %s\n",(char*)native_buffer);
    }
}

void test_fib() {
    for (int i = 0; i < cnt; i++) {
        // 创建test，并将其序列化
        Fib test;
        fib__init(&test);
        test.num = 42;
        int size = fib__pack(&test,native_buffer);
        //传递参数，开始执行
        uint32_t argv[2] = {wasm_buffer,size};
        if (wasm_runtime_call_wasm(exec_env, benchMarkFunc, 2, argv)) {
            // printf("run str_reverse ok \n");
        }
        else {
            printf("call wasm function fib failed. error: %s\n",
                wasm_runtime_get_exception(module_inst));
            exit(EXIT_FAILURE);
        }
        //将得到的结果反序列化
        size = argv[0];
        Fib *p = fib__unpack(NULL,size,pResult);
        printf("p->num = %d\n",p->num);
    }
}

void test_fib_exec() {
    for (int i = 0; i < cnt; i++) {
        uint32_t argv[2] = {42};
        if (wasm_runtime_call_wasm(exec_env, benchMarkFunc, 1, argv)) {
            // printf("run str_reverse ok \n");
        } else {
            printf("call wasm function fib failed. error: %s\n",
                wasm_runtime_get_exception(module_inst));
            exit(EXIT_FAILURE);
        }
        int res = argv[0];
        printf("res = %d\n",res);
    }
}

