#include <stdio.h>
#include <windows.h>
#include <conio.h>

using namespace std;

int main(int argc, char *argv[])
{
	printf("CHEKERRRRRRR\n");
	bool flag = true;
	HANDLE hEvent[3];
	hEvent[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Create");
	hEvent[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Delete");
	hEvent[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Terminate");
	while (flag)
	{
		int ch;
		ch = getch();
		if (ch == '+')
		{
			printf("ADD\n");
			PulseEvent(hEvent[0]);
			
		}
		if (ch == '-')
		{
			printf("DELETE\n");
			PulseEvent(hEvent[1]);
		}
		if (ch != '+' && ch != '-')
		{
			printf("TERMINATE\n");
			PulseEvent(hEvent[2]);
			flag = false;
			return 0;
		}
		
	}
}