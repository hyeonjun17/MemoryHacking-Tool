#include <stdio.h>
#include <conio.h>

int main(void)
{
	char string[] = "HelloWorld";
	char ch;
	printf("\nPress \'C\' to continue printing \nPress \'A\' to print address\nPress \'E\' to exit\n\n");
	do
	{
		ch = _getch();
		if (ch == 'C' || ch == 'c')
			printf("%s", string);
		else if (ch == 'A' || ch == 'a')
			printf("%#X", string);
		putchar(' ');
	} while (ch != 'E' && ch != 'e');
	return 0;
}