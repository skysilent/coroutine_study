#include"windowcoroutine.h"
#include<Windows.h>
// 纤程栈大小
#define _INT_STACK        (1024 * 1024)
// 默认初始化创建纤程数目
#define _INT_COROUTINE    (16)
#define COROUTINE_DEAD 0
#define COROUTINE_READY 1
#define COROUTINE_RUNNING 2
#define COROUTINE_SUSPEND 3
/*
* 单个纤程单元 coroutine , 还有纤程集管理器 S
*/
// 纤程栈大小 1M
#define _INT_STACK        (1024 * 1024)
// 默认初始化创建纤程数目
#define _INT_COROUTINE    (16)

/*
* 单个纤程单元 coroutine , 还有纤程集管理器 S
*/
struct coroutine;

struct schedule {
    PVOID main;                 // 纤程管理器中保存的临时纤程对象
    int running;                // 当前纤程管理器中运行的纤程id
    int cur_index;              // 当前纤程集轮询中当前索引
    int cap;                    // 纤程集容量,
    int coroutine_count;        // 协程个数
    struct coroutine ** co;     // 保存的纤程集
};

struct coroutine {
    PVOID ctx;                    // 操作系统纤程对象                
    coroutine_function func;      // 纤程执行的函数体
    void * ud;                    // 纤程执行的额外参数
    int status;       // 当前纤程运行状态
};
/* 通过将主线程转换为纤程，此处需要和纤程关闭的时候使用(将纤程转为线程)
*
*/
struct schedule* coroutine_open(void)
{
    struct schedule*comanag = (schedule*)malloc(sizeof(struct schedule));
    comanag->cur_index = 0;
    comanag->cap = _INT_COROUTINE;
    comanag->running = -1;
    struct coroutine **co = (coroutine**)calloc(comanag->cap, sizeof(struct coroutine*));
    comanag->co = co;
    comanag->coroutine_count = 0;
	/*ConvertThreadToFiberEx(NULL,??);
	  CreateFiber()和CreateFiberEx()的方式不太适合用来主线程转主协程，
      主要是第一个参数SIZE_T  dwStackSize没办法写
	  这个方法在测试使用时候直接造成访问权限冲突了，应该是越界的问题
	*/
	comanag->main = ConvertThreadToFiber(NULL);
    return comanag;
}
/*
  清理某一个协程的信息数据，回收空间
*/
inline void _delete_coroutine(coroutine *co) {
    DeleteFiber(co->ctx);
    free(co);
}

inline struct coroutine* _new_coroutine(coroutine_function func, void * ud) {
    struct coroutine *co = (coroutine*)malloc(sizeof(struct coroutine));
    co->func = func;
    co->ud = ud;
    co->status = COROUTINE_READY;
    return co;
}

void coroutine_close(struct schedule* S)
{
    for (int i = 0; i < S->cap; i++) {
        struct coroutine *co = S->co[i];
        if (co) {
            _delete_coroutine(co);
            S->co[i] = NULL;
        }
    }
    free(S->co);
    S->co = NULL;
    free(S);
    S = NULL;
    ConvertFiberToThread();
}

int coroutine_new(struct schedule* S, coroutine_function func, void * ud)
{
    struct coroutine *co = _new_coroutine(func, ud);
    int index = S->cur_index;
    int count = S->cap;
    if (S->cur_index < S->cap) {
        for (int i = 0; i < S->cap; i++) {
            int id = (i + S->coroutine_count) % S->cap;
            if (NULL == S->co[id]) {
                S->co[id] = co;
                ++S->coroutine_count;
                ++S->cur_index;
                return id;
            }
        }
    }
    coroutine ** point = (coroutine **)realloc(S->co, sizeof(struct coroutine *) * S->cap * 2);
    memset(point + S->cap, 0, sizeof(struct coroutine *) * S->cap);
    point[count] = co;
    S->cap *= 2;
    S->co = point;
    S->coroutine_count++;
    return S->cur_index++;
}
/*该函数的设计是依赖于CreateFiberEx
 * mainfunc(uint32_t low32, uint32_t hi32)
 此处想要将参数也传过去，选择方案CreateFiberEx()
 LPVOID CreateFiberEx(
 SIZE_T                dwStackCommitSize,
 SIZE_T                dwStackReserveSize,
 DWORD                 dwFlags,
 LPFIBER_START_ROUTINE lpStartAddress,
 LPVOID                lpParameter
 );
 LPVOID CreateFiber(
 SIZE_T                dwStackSize,
 LPFIBER_START_ROUTINE lpStartAddress,
 LPVOID                lpParameter
 ); 
 */
static inline VOID WINAPI mainfunc(LPVOID ptr) {
    struct schedule * S = reinterpret_cast<struct schedule*>(ptr);
    int id = S->running;
    struct coroutine * co = S->co[id];

    co->func(S, co->ud);  //  ==>coroutine_function(S,co->ud)
    co = S->co[id];
    co->status = COROUTINE_DEAD;

    SwitchToFiber(S->main);
}

void coroutine_resume(struct schedule* S, int id)
{
    //  找一些不在运行中的协程进行管理，肯定不会再运行
    if (S->running != -1) {
        int runningid = S->running;
        coroutine * co = S->co[runningid];
        S->co[runningid] = NULL;
        --S->coroutine_count;
        S->cur_index = runningid;
        S->running = -1;
        _delete_coroutine(co);
        if (runningid == id) {
            return;
        }
    }
    else {
        coroutine * co = S->co[id];
        switch (co->status)
        {
        case COROUTINE_READY:
		
		   co->ctx = CreateFiber(_INT_STACK,  mainfunc,S); 
           // co->ctx = CreateFiberEx(_INT_STACK, 0, ??, mainfunc, S);
        case COROUTINE_SUSPEND:
            co->status = COROUTINE_RUNNING;
            S->running = id;
            S->main = GetCurrentFiber();
            SwitchToFiber(co->ctx);
        default:
            break;
        }
    }
}

void coroutine_yield(struct schedule* S)
{
    if (S) {
        int id = S->running;
        struct coroutine* co = S->co[id];

        co->status = COROUTINE_SUSPEND;
        S->running = -1;
        co->ctx = GetCurrentFiber();

        SwitchToFiber(S->main);
    }
}

int coroutine_status(struct schedule* S, int id)
{
    if (S&&id >= 0 && id<S->cap) {
        if (S->co[id]) {
            return S->co[id]->status;
        }
        return COROUTINE_DEAD;
    }
    return COROUTINE_DEAD;
}

int coroutine_running(struct schedule* S)
{
    if (S) {
        return S->running;
    }
    return -1;
}
