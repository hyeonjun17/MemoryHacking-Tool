#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <conio.h>
#define BUFSIZE 1024

unsigned int StartMenu(void);
BOOL SearchProcess(void);
void PrintProcessList(void);
BOOL CmpStr(BYTE* find_buffer, BYTE* source, unsigned int size);
void ReadAndPrintMemory(void);
void ReadAndWriteMemory(void);
void DllInjection(void);
void CodeInjection(void);
void PressAnyKeyToProceed(void);
void Error(const char* message);
void gotoXY(int x, int y);

int main(void)
{
	unsigned int choice;
	while (1)
	{
		system("cls");
		choice = StartMenu();
		if (choice == 0)
			break;
		BOOL decision = SearchProcess();
		if (decision == FALSE)
			continue;
		system("cls");
		gotoXY(0, 0);

		PrintProcessList();

		switch (choice)
		{
		case 1:
			ReadAndPrintMemory();
			break;
		case 2:
			ReadAndWriteMemory();
			break;
		case 3:
			DllInjection();
			break;
		case 4:
			CodeInjection();
		default:
			break;
		}
		PressAnyKeyToProceed();
	}

	return 0;
}

unsigned int StartMenu(void)
{
	unsigned int choice = 0;
	printf("\n\n");
	printf("\t\t\t\tMemory Hacking Tool");
	printf("\n\n\n");
	printf("\t\t\t\t1. Read and Print Process' Memory\n");
	printf("\t\t\t\t2. Find and Write String\n");
	printf("\t\t\t\t3. DLL Injection (CreateRemoteThread, kernel32.dll)\n");
	printf("\t\t\t\t4. Code Injection (CreateRemoteThread, user32.dll, MessageBoxA, msvcrt.dll, printf)\n");
	printf("\t\t\t\t0. Exit\n\n");
	printf("\t\t\t\tInput : ");
	scanf_s("%d", &choice);
	getchar();
	return choice;
}

BOOL SearchProcess(void)
{
	BOOL result = FALSE;
	printf("\t\t\n");
	printf("\n\n\n\t\t\t\tPress \'S\' to search process\n");
	printf("\t\t\n");
	printf("\n\t\t\t\tPress \'E\' to go back to menu\n\n");
	while (1)
	{
		if (_kbhit())
		{
			char input = _getch();
			if (input == 's' || input == 'S')
			{
				result = TRUE;
				break;
			}
			else if (input == 'e' || input == 'E')
			{
				result = FALSE;
				break;
			}
		}
	}
	return result;
}

void PrintProcessList(void)
{
	HANDLE hProcessSnap = 0;
	HANDLE hProcess = 0;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		Error("Invalid ProcessSnap Handle");

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		if(hProcessSnap != NULL)
			CloseHandle(hProcessSnap);
		Error("Process32First Error");
		printf("%ld", GetLastError());
	}

	char Path[MAX_PATH];
	do
	{
		memset(Path, 0, sizeof(char) * MAX_PATH);

		HANDLE hProcess = 
			OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

		if (hProcess == NULL)
			continue;

		int size = MAX_PATH * sizeof(char);
		if (QueryFullProcessImageNameW(hProcess, 0, Path, &size))
		{
			_tprintf(_T("\n\nProcess Name : %s\nPath : %s\nID : %d"),
				pe32.szExeFile, Path, pe32.th32ProcessID);
		}
		if(hProcess != NULL)
			CloseHandle(hProcess);
		
	} while (Process32Next(hProcessSnap, &pe32));

	if(hProcessSnap != NULL)
		CloseHandle(hProcessSnap);
}

BOOL CmpStr(BYTE* find_buffer, BYTE* source_buffer, unsigned int size)
{
	BOOL result = TRUE;
	for(int i = 0; i < size; i++)
		if (source_buffer + i == NULL || find_buffer[i] != source_buffer[i])
		{
			result = FALSE;
			break;
		}
	return result;
}

void ReadAndPrintMemory(void)
{
	DWORD pid;
	printf("\n\nEnter Process id : ");
	scanf_s("%ld", &pid);
	getchar();

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == INVALID_HANDLE_VALUE)
		Error("Cannot Open Process");
	else
		printf("\n\nOpened Process successfully\n\n");

	SYSTEM_INFO si;
	MEMORY_BASIC_INFORMATION mbi;
	BYTE* read_buffer;
	DWORD nMem;

	GetSystemInfo(&si);

	nMem = (DWORD)si.lpMinimumApplicationAddress;

	BOOL isFound = FALSE;

	do
	{
		if (VirtualQueryEx(hProcess, nMem, &mbi, sizeof(mbi)) == sizeof(mbi))
		{
			if (mbi.RegionSize > 0 && mbi.Type == MEM_PRIVATE &&
				mbi.State == MEM_COMMIT)
			{
				read_buffer = (BYTE*)malloc(mbi.RegionSize * sizeof(BYTE));

				if (ReadProcessMemory(hProcess, mbi.BaseAddress, read_buffer, mbi.RegionSize, NULL) != 0)
				{
					int counter = 0;
					for (int i = 0; i < (DWORD)mbi.RegionSize - 1; i += 12)
					{
						for (int j = i; j < i + 12; j++)
						{
							printf("%02x ", read_buffer[j]);
						}
						putchar('\t');
						putchar('\t');
						for (int j = i; j < i + 12; j++)
						{
							printf("%c ", read_buffer[j]);
						}
						putchar('\t');
						putchar('\t');
						for (int j = i; j < i + 12; j++)
						{
							printf("%d ", read_buffer[j]);
						}
						putchar('\n');
					}
				}
				free(read_buffer);
			}
			nMem = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
		}
	} while (nMem < (DWORD)si.lpMaximumApplicationAddress && isFound == FALSE);

	if (hProcess != NULL)
		CloseHandle(hProcess);
}

void ReadAndWriteMemory(void)
{
	DWORD pid;
	printf("\n\nEnter Process id : ");
	scanf_s("%ld", &pid);
	getchar();

	BYTE find_buffer[BUFSIZE];
	memset(find_buffer, 0, sizeof(BYTE) * BUFSIZE);
	printf("\n\nEnter string to find : ");
	scanf_s("%s", find_buffer, sizeof(find_buffer));
	getchar();
	unsigned int find_buffer_size = strlen(find_buffer);

	BYTE write_buffer[BUFSIZE];
	memset(write_buffer, 0, sizeof(write_buffer));
	printf("\n\nEnter string to write : ");
	scanf_s("%s", write_buffer, sizeof(write_buffer));
	getchar();
	unsigned int write_buffer_size = strlen(write_buffer);


	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == INVALID_HANDLE_VALUE)
		Error("Cannot Open Process");
	else
		printf("\n\nOpened Process successfully\n\n");

	SYSTEM_INFO si;
	MEMORY_BASIC_INFORMATION mbi;
	BYTE* read_buffer;
	DWORD nMem;

	GetSystemInfo(&si);

	nMem = (DWORD)si.lpMinimumApplicationAddress;

	BOOL isFound = FALSE;

	do
	{
		if (VirtualQueryEx(hProcess, nMem, &mbi, sizeof(mbi)) == sizeof(mbi))
		{
			if (mbi.RegionSize > 0 && mbi.Type == MEM_PRIVATE &&
				mbi.State == MEM_COMMIT)
			{
				read_buffer = (BYTE*)malloc(mbi.RegionSize * sizeof(BYTE));

				if (ReadProcessMemory(hProcess, mbi.BaseAddress, read_buffer, mbi.RegionSize, NULL) != 0)
				{
					printf("\n\nReading Process Memory..\n");
					for (int i = 0; i < (DWORD)mbi.RegionSize; i++)
						if (CmpStr(find_buffer, &read_buffer[i], find_buffer_size))
						{
							isFound = TRUE;
							printf("\n\nFound Data\n");
							if (WriteProcessMemory(hProcess, (DWORD)mbi.BaseAddress + i, write_buffer, write_buffer_size, NULL) != 0)
								printf("\n\nWriting Memory..\n");
						}
				}
				free(read_buffer);
			}
			nMem = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
		}
	} while (nMem < (DWORD)si.lpMaximumApplicationAddress && isFound == FALSE);

	if (isFound == FALSE)
		printf("\n\nFailed to Find string\n\n");

	if (hProcess != NULL)
		CloseHandle(hProcess);
}

void DllInjection(void)
{
	HANDLE hProcess;
	HANDLE hThread;
	HMODULE hMod;
	HANDLE hModAddr;
	LPVOID lpRemoteBuffer;

	DWORD pid;
	printf("\n\nEnter Process id : ");
	scanf_s("%ld", &pid);
	getchar();

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == INVALID_HANDLE_VALUE)
		Error("Cannot Open process");
	else
		printf("\n\nOpened Process successfully\n\n");

	BYTE DllPath[MAX_PATH];
	printf("Enter Dll path to inject : ");
	scanf_s("%s", &DllPath);
	getchar();
	DWORD dwDllpathSize = strlen(DllPath) + 1;

	lpRemoteBuffer = VirtualAllocEx(hProcess, NULL, dwDllpathSize, MEM_COMMIT, PAGE_READWRITE);
	if (lpRemoteBuffer == NULL)
		Error("VirtualAllocEx Failed");

	if (WriteProcessMemory(hProcess, lpRemoteBuffer, (LPVOID)DllPath, dwDllpathSize, NULL) == 0)
	{
		printf("\n\nFailed to write process memory");
		return;
	}
	else
		printf("\n\nWriting Memory..\n\n");

	hMod = GetModuleHandle(_T("kernel32.dll"));
	if (hMod == INVALID_HANDLE_VALUE)
		Error("Cannot get handle of \"kernel32.dll\"");

	hModAddr = GetProcAddress(hMod, "LoadLibraryA");
	if (hModAddr == INVALID_HANDLE_VALUE)
		Error("Cannot get address of \"LoadLibraryA\"");

	hThread = CreateRemoteThread(hProcess, NULL, 0, hModAddr, lpRemoteBuffer, 0, NULL);
	if (hThread == INVALID_HANDLE_VALUE)
		Error("Cannot create remotethread");
	else
		printf("\n\nCreated RemoteThread\n\n");

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hProcess);
	CloseHandle(hThread);
	FreeLibrary(hMod);
}

typedef HMODULE(WINAPI* myLoadLibraryA)(LPCSTR lpLibFileName);
typedef FARPROC(WINAPI* myGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
typedef INT(WINAPI* myMessageBoxA)(HWND hWind, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
typedef INT(_cdecl myPrintf)(const char* const _Format, ...);

typedef struct Inject_Data
{
	FARPROC loadlibraryA;
	FARPROC getprocaddress;
	BYTE user32dll[20];
	BYTE msvcrtdll[20];
	BYTE messagebox_func[20];
	BYTE printf_func[20];
	BYTE str[2][20];
} Inject_Data;

void WINAPI ThreadFuncForCodeInjection(LPVOID nParam)
{
	Inject_Data* Data = (Inject_Data*)nParam;
	HMODULE hMod1;
	HMODULE hMod2;

	hMod1 = ((myLoadLibraryA)Data->loadlibraryA)(Data->user32dll);
	myMessageBoxA pFunc1 = (myMessageBoxA)((myGetProcAddress)Data->getprocaddress)(hMod1, Data->messagebox_func);
	hMod2 = ((myLoadLibraryA)Data->loadlibraryA)(Data->msvcrtdll);
	myPrintf* pFunc2 = (myPrintf*)((myGetProcAddress)Data->getprocaddress)(hMod2, Data->printf_func);

	pFunc1(NULL, Data->str[0], Data->str[1], MB_OK);
	for(int i = 0; i < 100000; i++)
		pFunc2(Data->str[0]);
}

void AfterFunc() {}

void CodeInjection(void)
{
	HANDLE hProcess;
	HANDLE hThread;
	LPVOID ThreadVirtualAddr;
	LPVOID DataVirtualAddr;
	LPVOID PrintfVirtualAddr;
	DWORD ThreadFuncSize;

	DWORD pid;
	printf("\n\nEnter Process id : ");
	scanf_s("%ld", &pid);
	getchar();

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == INVALID_HANDLE_VALUE)
		Error("Cannot Open process");
	else
		printf("\n\nOpened Process successfully\n\n");

	ThreadFuncSize = (DWORD)AfterFunc - (DWORD)ThreadFuncForCodeInjection;

	ThreadVirtualAddr = VirtualAllocEx(hProcess, NULL, ThreadFuncSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (ThreadVirtualAddr == NULL)
		Error("VirtualAllocEx Failed");

	if (WriteProcessMemory(hProcess, ThreadVirtualAddr, (LPVOID)ThreadFuncForCodeInjection,
		ThreadFuncSize, NULL) != 0)
		printf("\n\nWriting Memory..\n\n");
	else
	{
		printf("\n\nCannot write memory\n\n");
		return;
	}

	DataVirtualAddr = VirtualAllocEx(hProcess, NULL, sizeof(Inject_Data), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (DataVirtualAddr == NULL)
		Error("VirtualAllocEx Failed");

	Inject_Data data;
	HMODULE hMod = GetModuleHandleA("kernel32.dll");
	if (hMod == INVALID_HANDLE_VALUE)
		Error("Cannot getmodulehandle");
	data.loadlibraryA = GetProcAddress(hMod, "LoadLibraryA");
	data.getprocaddress = GetProcAddress(hMod, "GetProcAddress");
	strcpy_s(data.user32dll, sizeof("user32.dll"), "user32.dll");
	strcpy_s(data.msvcrtdll, sizeof("msvcrt.dll"), "msvcrt.dll");
	strcpy_s(data.messagebox_func, sizeof("MessageBoxA"), "MessageBoxA");
	strcpy_s(data.printf_func, sizeof("printf"), "printf");
	strcpy_s(data.str[0], sizeof("you're hacked!"), "you're hacked!");
	strcpy_s(data.str[1], sizeof("Alarm"), "Alarm");


	if (WriteProcessMemory(hProcess, DataVirtualAddr, (LPVOID)&data,
		sizeof(Inject_Data), NULL) != 0)
		printf("\n\nWriting Memory..\n\n");
	else
	{
		printf("\n\nCannot write memory\n\n");
		return;
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)ThreadVirtualAddr,
		DataVirtualAddr, 0, NULL);
	if (hThread == INVALID_HANDLE_VALUE)
		Error("Cannot create remotethread");
	else
		printf("\n\nCreated RemoteThread\n\n");

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hProcess);
	CloseHandle(hThread);
	FreeLibrary(hMod);
}

void PressAnyKeyToProceed(void)
{
	puts("Press any key to proceed..");
	_getch();
}

void Error(const char* message)
{
	putchar('\n');
	putchar('\n');
	puts(message);
	_getch();
	exit(1);
}

void gotoXY(int x, int y)
{
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}