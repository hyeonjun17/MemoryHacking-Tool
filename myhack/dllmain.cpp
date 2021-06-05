// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include <cstdio>
#include <Windows.h>
#include <process.h>

_beginthreadex_proc_type ThreadFunc(LPVOID nParam)
{
    MessageBoxA(NULL, "you're hacked!", "Alarm", MB_OK);
    for (int i = 0; i < 100000; i++)
        printf("you're hacked!");
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    HANDLE hProcess;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hProcess = (HANDLE)_beginthreadex(
            NULL, 0,
            (_beginthreadex_proc_type)ThreadFunc, NULL,
            0, NULL
        );
        CloseHandle(hProcess);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

