#include <Windows.h>
#include <vector>
#include <ctime>
#include <chrono>
#include <iostream>

using namespace std::chrono;

constexpr int nScreenWidth  = 120;
constexpr int nScreenHeight = 40;

struct Cell
{
	int x;
	int y;
};

unsigned CountNeighbor(wchar_t* screen, int x, int y)
{
	unsigned res = 0;
	if (x != 0 && screen[y * nScreenWidth + x - 1] != L' ')
		res++;
	if (y != 3 && screen[(y - 1) * nScreenWidth + x] != L' ')
		res++;
	if (x != nScreenWidth - 1 && screen[y * nScreenWidth + x + 1] != L' ')
		res++;
	if (y != nScreenHeight - 1 && screen[(y + 1) * nScreenWidth + x] != L' ')
		res++;
	if (x != 0 && y != 3 && screen[(y - 1) * nScreenWidth + x - 1] != L' ')
		res++;
	if (x != 0 && y != nScreenHeight - 1 && screen[(y + 1) * nScreenWidth + x - 1] != L' ')
		res++;
	if (x != nScreenWidth - 1 && y != 3 && screen[(y - 1) * nScreenWidth + x + 1] != L' ')
		res++;
	if (x != nScreenWidth - 1 && y != nScreenHeight - 1 && screen[(y + 1) * nScreenWidth + x + 1] != L' ')
		res++;

	return res;
}

int main()
{
	srand(time(0));

	wchar_t* screen = new wchar_t[nScreenHeight * nScreenWidth];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	int nBestScore = 0;

	bool bIsRunning  = true;
	bool bUpKey      = false;
	bool bDownKey    = false;
	bool bUpKeyOld   = false;
	bool bDownKeyOld = false;
	auto nDelay      = 200ms;
	int nGeneration  = 1;

	bool ScreenBuffer[nScreenWidth * nScreenHeight] = { false };
	for (int i = 0; i < 300; ++i)
	{
		int x = rand() % 120;
		int y = (rand() % 37) + 3;
		ScreenBuffer[y * nScreenWidth + x] = true;
	}
	/* Glyder
	ScreenBuffer[15 * nScreenWidth + 40] =
	ScreenBuffer[15 * nScreenWidth + 41] =
	ScreenBuffer[15 * nScreenWidth + 42] =
	ScreenBuffer[14 * nScreenWidth + 42] =
	ScreenBuffer[13 * nScreenWidth + 41] = true;
	*/

	while (bIsRunning)
	{
		auto t1 = high_resolution_clock::now();
		while ((high_resolution_clock::now() - t1) < nDelay)
		{
			if ((GetAsyncKeyState((unsigned char)'\x20') & 0x8000) != 0)
				while ((GetAsyncKeyState((unsigned char)'\x20') & 0x8000) != 0);
		}

		for (int i = 0; i < nScreenHeight; ++i)
			for (int j = 0; j < nScreenWidth; ++j)
				screen[i * nScreenWidth + j] = L' ';

		for (int i = 0; i < nScreenWidth; ++i)
		{
			screen[i] = L'#';
			screen[2 * nScreenWidth + i] = L'#';
		}
		screen[nScreenWidth] = L'#';
		screen[2 * nScreenWidth - 1] = L'#';

		for (int x = 0; x < nScreenWidth; ++x)
			for (int y = 3; y < nScreenHeight; ++y)
				screen[y * nScreenWidth + x] = (ScreenBuffer[y * nScreenWidth + x] ? 0x2588 : L' ');

		wsprintf(&screen[nScreenWidth + 20], L"CONWAY'S GAME OF LIFE");
		wsprintf(&screen[nScreenWidth + 83], L"GENERATION: %d", nGeneration++);

		// Logic
		for (int x = 0; x < nScreenWidth; ++x)
		{
			for (int y = 3; y < nScreenHeight; ++y)
			{
				unsigned nNeighborCount = CountNeighbor(screen, x, y);
				if (screen[y * nScreenWidth + x] == L' ' && nNeighborCount == 3)
				{
					ScreenBuffer[y * nScreenWidth + x] = true;
				}
				else if (screen[y * nScreenWidth + x] != L' ')
				{
					if (nNeighborCount < 2 || nNeighborCount > 3)
						ScreenBuffer[y * nScreenWidth + x] = false;
				}
			}
		}
		// Presentation

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	return 0;
}
