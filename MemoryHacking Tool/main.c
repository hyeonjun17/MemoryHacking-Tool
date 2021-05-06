#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <conio.h>
#define BUFSIZE 100

BOOL StartMenu(void);
DWORD* FindAndWriteMemory(HANDLE hProc, unsigned int find, unsigned int size);
void Error(const char* message);
void gotoXY(int x, int y);

int main(void)
{
	BOOL decision = StartMenu();
	if (decision == FALSE)
		return 0;
	system("cls");
	gotoXY(0, 0);

	PROCESSENTRY32 pe;

	DWORD pid;
	puts("Enter process id : ");
	scanf_s("%d", &pid);
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (h != NULL)
		puts("\n\nOpened process successfully\n\n");
	DWORD bufsize = 0;
	puts("Find Memory size : ");
	scanf_s("%ld", bufsize, sizeof(unsigned long));
	BYTE* buffer = (BYTE*)malloc(bufsize * sizeof(BYTE));
	puts("Find Memory buf : ");
	scanf_s("%s", buffer, (unsigned int)(bufsize * sizeof(BYTE)));

	free(buffer);
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

DWORD* FindAndWriteMemory(HANDLE hProc, unsigned int find, unsigned int size)
{
	SYSTEM_INFO si;
	
}

void Error(const char* message)
{
	putchar('\n');
	putchar('\n');
	puts(message);
	exit(1);
}

void gotoXY(int x, int y)
{
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}