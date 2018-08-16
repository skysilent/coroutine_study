/* Copyright 2018
 特别说明，此处代码严格参考云风的库coroutine(linux C++环境),基于Fiber
 github 地址：https://github.com/cloudwu/coroutine
 不知道会不会有内存溢出的情况,反正先理解一下纤程的用处
 依赖材料:
    https://docs.microsoft.com/en-us/windows/desktop/api/WinBase/nf-winbase-switchtofiber 
    windows下面有个co_yield的玩意是个关键字
 将主线程转为纤程，如果直接创建协程（CreateFiber）访问权限冲突，直接崩
    https://docs.microsoft.com/zh-cn/windows/desktop/api/winbase/nf-winbase-convertthreadtofiber
 将纤程转为主线程，程序跳不出来，指针不知道跑哪去了(调试时程序调完了，程序还在,协程的地址没出现过，不是主协程也不是一般协程)？
    https://docs.microsoft.com/zh-cn/windows/desktop/api/winbase/nf-winbase-convertfibertothread
 
 */

#ifndef WINDOWCOROUTINE_H_
#define WINDOWCOROUTINE_H_

/*协程管理表*/
struct schedule;

/*
* 创建运行纤程的主体, 等同于纤程创建需要执行的函数体.
* 管理器本身 函数返回的结果
* 参数用户自定义数据
*/
typedef void(*coroutine_function)(void* schedule, void * ud);

/*
* 开启纤程系统, 并创建主纤程
* 返回开启的纤程调度系统管理器
*/
extern struct schedule* coroutine_open(void);

/*
* 关闭开启的纤程系统
* S 是co_start 返回的纤程管理器
*/
extern void coroutine_close(struct schedule*S);
/*
* 创建一个纤程对象,并返回创建纤程的id. 创建好的纤程状态是CS_Ready
* S 是 coroutine_start 返回的纤程管理器
* func 是纤程运行的主体
* ud  是 用户传入的数据, coroutine_function 传入的参数(args)
*  返回创建好的纤程标识id(id指的是当前的管理器中的序号位置)
*/
extern int coroutine_new(struct schedule* S, coroutine_function func, void * ud);

/*
* resume恢复纤程对象.
* S   作为纤程管理器对象
* id  指的是coroutine_create 创建的纤程对象
* 将处于准备状态和挂起状态的纤程对象激活，
*/
extern void coroutine_resume(struct schedule* S, int id);

/*
* yield 挂起指令中断当前运行的的纤程, 并将CPU交给主纤程处理调度.
* S 纤程管理器对象  
*/
extern void coroutine_yield(struct schedule*S);

/*
* 获取当前纤程运行的状态
* S  纤程管理器对象
* id 是由 coroutine_create 创建的纤程对象，就是纤程在管理器中的位置
*返回状态
*/
extern int  coroutine_status(struct schedule*S, int id);

/*
* 得到当前纤程系统中运行的纤程
* S纤程管理器对象
* 返回当前运行的纤程id,-1表示没有纤程在运行
*/
extern int coroutine_running(struct schedule*S);

#endif // WINDOWCOROUTINE_H_

