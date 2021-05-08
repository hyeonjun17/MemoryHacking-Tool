#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <conio.h>

BOOL StartMenu(void);
void PrintProcessList(void);
BOOL GetModulePath(DWORD pid, char* ModulePath, unsigned int size);
void Error(const char* message);
void gotoXY(int x, int y);

int main(void)
{
	DWORD pid;
	while (1)
	{
		system("cls");
		BOOL decision = StartMenu();
		if (decision == FALSE)
			return 0;
		system("cls");
		gotoXY(0, 0);

		PrintProcessList();

		printf("\n\nEnter \'0\' to return to Menu\n\nEnter Process id : ");
		scanf_s("%ld", &pid);
		getchar();

		if (pid == 0)
			continue;
		break;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == INVALID_HANDLE_VALUE)
		Error("Cannot Open Process");
	else
		printf("\n\nOpened Process successfully\n\n");
	
	if(hProcess != NULL)
		CloseHandle(hProcess);

	return 0;
}

BOOL StartMenu(void)
{
	BOOL result = FALSE;
	printf("\n\n\n\t\t\t\t\tMemoryHacking Tool\n");
	printf("\t\t\n");
	printf("\n\n\n\t\t\t\t\tPress \'S\' to search process\n");
	printf("\t\t\n");
	printf("\n\t\t\t\t\tPress \'E\' to exit\n\n");
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