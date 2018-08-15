
#include"windowcoroutine.h"
#include <stdio.h>

static void testfunction(void * S, void * ud) {
    char * arg = ( char*) ud;
    int i;
    for (i = 0; i<2; i++) {
        printf("coroutine %d : %s   i=%d\n", coroutine_running((schedule*)S), arg,i);
        coroutine_yield((schedule*)S);
    }
}

static void test(void * S) {
	char a[] = "hello";
	char b[] = "hi";
    int co1 = coroutine_new((schedule*)S, testfunction,(void*)a);
    int co2 = coroutine_new((schedule*)S, testfunction, (void*)b);
    printf("coroutine start\n");
    int c1 = coroutine_status((schedule*)S, co1);
    int c2 = coroutine_status((schedule*)S, co2);
    while (c1&&c2) {
        coroutine_resume((schedule*)S, co1);
        coroutine_resume((schedule*)S, co2);
        c1 = coroutine_status((schedule*)S, co1);
        c2 = coroutine_status((schedule*)S, co2);
    }
    printf("coroutine end  \n");
}

int main() {
    void * S = coroutine_open();
    test(S);
    coroutine_close((schedule*)S);
    return 0;
}

