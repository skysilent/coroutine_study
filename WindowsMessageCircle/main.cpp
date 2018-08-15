#include<Windows.h>
#include<stdio.h>
DWORD g_dwMainThreadId;
BOOL WINAPI ConsoleCtrlhandler(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
         printf("CTRL_C_EVENT\n"); 
         PostThreadMessage(g_dwMainThreadId, WA_INACTIVE, 0, 0);
         break;
    default:
        printf("what the hell");
        PostThreadMessage(g_dwMainThreadId, WM_QUIT, 0, 0);
        break;
    }
    /*if (dwCtrlType==CTRL_C_EVENT)
    {
        printf("CTRL_C_EVENT\n");*/
   
   /* }*/
    return TRUE;
}


int main()
{
    g_dwMainThreadId = GetCurrentThreadId();
    SetConsoleCtrlHandler(ConsoleCtrlhandler, TRUE);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    SetConsoleCtrlHandler(ConsoleCtrlhandler, FALSE);

    return 0;
}
