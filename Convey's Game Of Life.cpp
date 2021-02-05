#include <Windows.h>
#include <vector>
#include <ctime> // for time
#include <chrono>
#include <iostream>
#include <fstream>

using namespace std::chrono;

constexpr int nScreenWidth  = 120;
constexpr int nScreenHeight = 40;

struct Cell
{
	int x;
	int y;
};
// Parses file filled with coords
void ParseFigure(bool* sb, int width, std::ifstream& ifs)
{
	int x, y;
	while (ifs >> x >> y)
		sb[y * width + x] = true;
}

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
// if declared, randomly generates asked number of cells
// If not, reads coords from file
#define RANDOM 350

int main()
{
	srand(time(0));

	wchar_t* screen = new wchar_t[nScreenHeight * nScreenWidth];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	int nBestScore = 0;

	auto nDelay      = 100ms;
	int nGeneration  = 1;

	std::ifstream ifs("Glider Gun.txt");
	bool ScreenBuffer[nScreenWidth * nScreenHeight] = { false };
	
#ifdef RANDOM
	for (int i = 0; i < RANDOM; ++i)
	{
		int x = rand() % 120;
		int y = (rand() % 37) + 3;
		ScreenBuffer[y * nScreenWidth + x] = true;
	}
#else
	ParseFigure(ScreenBuffer, nScreenWidth, ifs);
#endif

	while (true)
	{
		// These lines allow user to stop app while holding space
		
		auto t1 = high_resolution_clock::now();
		while ((high_resolution_clock::now() - t1) < nDelay)
		{
			if ((GetAsyncKeyState((unsigned char)'\x20') & 0x8000) != 0)
				while ((GetAsyncKeyState((unsigned char)'\x20') & 0x8000) != 0);
		}
		
		// Clearing
		
		for (int i = 0; i < nScreenHeight; ++i)
			for (int j = 0; j < nScreenWidth; ++j)
				screen[i * nScreenWidth + j] = L' ';
		
		// Drawing to screen
		
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
		
		// Making new generation
		
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

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	return 0;
}
