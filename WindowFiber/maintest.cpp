
#include"windowcoroutine.h"
#include <stdio.h>
#include<thread>
 void testfunction(void * S, void * ud) {
    char * arg = ( char*) ud;
    int i;
    for (i = 0; i<2; i++) {
        printf("coroutine %d : %s   i=%d\n", coroutine_running((schedule*)S), arg,i);
        coroutine_yield((schedule*)S);
    }
}
 void testfunction2(void * S, void * ud) {
    char * arg = (char*)ud;
    int i;
    for (i = 0; i<2; i++) {
        printf("coroutine %d : %s   i=%d\n", coroutine_running((schedule*)S), arg, i);
        coroutine_yield((schedule*)S);
    }
}
 void test(void * S) {
	char a[] = "test1 hello";
	char b[] = "test1 hello";
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
 void test2(void * S) {
    char a[] = "test2 hello";
    char b[] = "test2 hi";
    int co1 = coroutine_new((schedule*)S, testfunction2, (void*)a);
    int co2 = coroutine_new((schedule*)S, testfunction2, (void*)b);
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
void testdemo() {
    void * S = coroutine_open();
    test(S);
    coroutine_close((schedule*)S);
}
void testdemo2() {
    void * S = coroutine_open();
    test2(S);
    coroutine_close((schedule*)S);
}
int main() {
    
    std::thread t(testdemo);
   // std::thread t2(testdemo2);
    t.join();
   // t2.join();
}

