#include <stdio.h>
#include <windows.h>

#define TIMEFILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\timer.bin"

using namespace std;

int main()
{
	int timer = 0;
	FILE *file;
	file = fopen(TIMEFILENAME, "r+b");
	fread(&timer, sizeof(int), 1, file);
	fclose(file);
	printf("%d", timer);
	bool flag = true;
	HANDLE hEvent;
	printf("i'm timer\n");
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Create");
	while (flag)
	{
		printf("pulse\n");
		PulseEvent(hEvent);
		Sleep(timer);
	}
	return 0;
}