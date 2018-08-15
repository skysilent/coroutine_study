
#include"windowcoroutine.h"
#include <stdio.h>

struct args {
    int n;
};

static void _foo(void * S, void * ud) {
    struct args * arg = (struct args*) ud;
    int start = arg->n;
    int i;
    for (i = 0; i<2; i++) {
        printf("coroutine %d : %d\n", coroutine_running((schedule*)S), start + i);
        coroutine_yield((schedule*)S);
    }
}

static void test(void * S) {
    struct args arg1 = { 0 };
    struct args arg2 = { 100 };

    int co1 = coroutine_create((schedule*)S, _foo, &arg1);
    int co2 = coroutine_create((schedule*)S, _foo, &arg2);
    printf("main start\n");
    int c1 = coroutine_status((schedule*)S, co1);
    int c2 = coroutine_status((schedule*)S, co2);
    while (c1&&c2) {
        coroutine_resume((schedule*)S, co1);
        coroutine_resume((schedule*)S, co2);
        c1 = coroutine_status((schedule*)S, co1);
        c2 = coroutine_status((schedule*)S, co2);
    }
    printf("main end\n");
}

int main(int argc, char * argv[]) {
    void * S = coroutine_start();
    test(S);
    coroutine_close((schedule*)S);
    return 0;
}

