#include <iostream>
#include <math.h>

#include <Windows.h>
#include <xinput.h>

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

//const POINT charPos({ 680, 315 });
const POINT charPos({ 960, 475 });

using namespace std;

void getUnitVec(double& unitX, double& unitY)
{
	POINT initialPos;
	GetCursorPos(&initialPos);
	const int diffX = initialPos.x - charPos.x;
	const int diffY = initialPos.y - charPos.y;
	const double length = std::sqrt(std::pow(diffX, 2) + std::pow(diffY, 2));
	unitX = diffX / length;
	unitY = diffY / length;
}

UINT sendInputW(int keyCode)
{
	// This structure will be used to create the keyboard
	// input event.
	INPUT ip;

	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press the key
	ip.ki.wVk = keyCode; // virtual-key code for the "w" key
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release the key
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	return SendInput(1, &ip, sizeof(INPUT));
}

void doSomething(char key, int code)
{
	if (GetKeyState(key) & 0x8000)
	{
		double unitX = 0.0;
		double unitY = 0.0;
		getUnitVec(unitX, unitY);
		const double unitCoeff = 20;
		const double incrX = unitCoeff * unitX;
		const double incrY = unitCoeff * unitY;

		POINT pos({ charPos.x, charPos.y });

		for (int j = 0; j < 25; j++)
		{
			pos.x += incrX;
			pos.y += incrY;

			/*HDC hdc, hdcTemp;
			RECT rect;
			BYTE* bitPointer;
			int x, y;

			hdc = GetDC(HWND_DESKTOP);
			const int half = 10;
			rect.top = pos.y - half;
			rect.bottom = pos.y + half;
			rect.left = pos.x - half;
			rect.right = pos.x + half;
			int MAX_WIDTH = rect.right;
			int MAX_HEIGHT = rect.bottom;
			int MIN_WIDTH = rect.left;
			int MIN_HEIGHT = rect.top;

			hdcTemp = CreateCompatibleDC(hdc);
			BITMAPINFO bitmap;
			bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
			bitmap.bmiHeader.biWidth = MAX_WIDTH - MIN_WIDTH;
			bitmap.bmiHeader.biHeight = MAX_HEIGHT - MIN_HEIGHT;
			bitmap.bmiHeader.biPlanes = 1;
			bitmap.bmiHeader.biBitCount = 32;
			bitmap.bmiHeader.biCompression = BI_RGB;
			bitmap.bmiHeader.biSizeImage = (MAX_WIDTH) * 4 * (MAX_HEIGHT);
			bitmap.bmiHeader.biClrUsed = 0;
			bitmap.bmiHeader.biClrImportant = 0;
			HBITMAP hBitmap2 = CreateDIBSection(hdcTemp, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, NULL);
			SelectObject(hdcTemp, hBitmap2);
			BitBlt(hdcTemp, 0, 0, (MAX_WIDTH - MIN_WIDTH), (MAX_HEIGHT - MIN_HEIGHT), hdc, rect.left, rect.top, SRCCOPY);

			int sumRed = 0;
			int sumGreen = 0;
			int sumBlue = 0;
			for (int i = 0; i < ((MAX_WIDTH - MIN_WIDTH) * 4 * (MAX_HEIGHT - MIN_HEIGHT)); i += 4)
			{
				int red = (int)bitPointer[i + 2];
				int green = (int)bitPointer[i + 1];
				int blue = (int)bitPointer[i];

				//cout << red << " ";

				sumRed += red;
				sumGreen += green;
				sumBlue += blue;
			}
			//cout << endl;

			int w = MAX_WIDTH - MIN_WIDTH;
			int h = MAX_HEIGHT - MIN_HEIGHT;
			double avgRed = sumRed / (w * h);
			double avgGreen = sumGreen / (w * h);
			double avgBlue = sumBlue / (w * h);

			const bool cond1 = avgRed > avgGreen * 1.1;
			const bool cond2 = avgRed > avgBlue * 1.2;
			const bool isTarget = cond1 && cond2;

			std::cout
				<< "pos: " << pos.x << " " << pos.y
				<< ", maxH: " << MAX_HEIGHT << ", minH: " << MIN_HEIGHT
				<< ", maxW: " << MAX_WIDTH << ", minW: " << MIN_WIDTH
				<< ", RGB: " << avgRed
				<< " " << avgGreen
				<< " " << avgBlue
				<< " " << (isTarget ? "true" : "false") << endl;*/

			if (true)
			{
				SetCursorPos(pos.x, pos.y);
				sendInputW(code);

				//break;
				Sleep(1);
			}

			//ReleaseDC(NULL, hDC);
		}

		Sleep(200);
	}
}

int main() {

	bool isMoving = false;
	while (1)
	{
		//POINT posA;
		//GetCursorPos(&posA);
		//cout << posA.x << " " << posA.y << endl;

		DWORD dwResult;
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState(i, &state);
			if (dwResult == ERROR_SUCCESS)
			{
				//cout << "a" << i << " " << dwResult << endl;

				double LX = state.Gamepad.sThumbLX + 32768;
				double LY = state.Gamepad.sThumbLY + 32768;
				cout << LX << " " << LY << endl;

				const double deadZone = 9600;
				if (
					(LX <= 32768 - deadZone || LX >= 32768 + deadZone)
					|| (LY <= 32768 - deadZone || LY >= 32768 + deadZone))
				{
					INPUT Inputs[2] = { 0 };
					Inputs[0].type = INPUT_MOUSE;
					Inputs[0].mi.dx = 32768 + (LX - 32768) / 10; // desired X coordinate
					Inputs[0].mi.dy = 29350 + ((65536 - LY) - 32768) / 6; // desired Y coordinate
					Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					Inputs[1].type = INPUT_MOUSE;
					Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

					SendInput(2, Inputs, sizeof(INPUT));

					isMoving = true;
				}
				else if (isMoving)
				{
					INPUT Inputs[2] = { 0 };

					Inputs[0].type = INPUT_MOUSE;
					Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

					Inputs[1].type = INPUT_MOUSE;
					Inputs[1].mi.dx = 32768; // desired X coordinate
					Inputs[1].mi.dy = 29350; // desired Y coordinate
					Inputs[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					SendInput(2, Inputs, sizeof(INPUT));

					isMoving = false;
				}
			}
			else
			{
				//cout << "b" << i << " " << dwResult << endl;
			}
			//Sleep(200);
		}

		//doSomething('W', 87);
		doSomething('2', 50);
		doSomething('3', 51);
		if (GetKeyState('J') & 0x8000)
		{
			//SetCursorPos(936, 468);
			//Sleep(100);

			INPUT Inputs[3] = { 0 };

			Inputs[0].type = INPUT_MOUSE;
			Inputs[0].mi.dx = 32500; // desired X coordinate
			Inputs[0].mi.dy = 25800; // desired Y coordinate
			Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

			Inputs[1].type = INPUT_MOUSE;
			Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

			Inputs[2].type = INPUT_MOUSE;
			Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

			SendInput(3, Inputs, sizeof(INPUT));
			//SetCursorPos(charPos.x, charPos.y);

			Sleep(200);
		}
		if (GetKeyState('G') & 0x8000)
		{
			double unitX = 0.0;
			double unitY = 0.0;
			getUnitVec(unitX, unitY);
			const double unitCoeff = 400;
			const double incrX = unitCoeff * unitX;
			const double incrY = unitCoeff * unitY;
			//SetCursorPos(charPos.x + incrX, charPos.y + incrY);
			//Sleep(200);
			//sendInputW(84);

			INPUT Inputs[3] = { 0 };

			Inputs[0].type = INPUT_MOUSE;
			Inputs[0].mi.dx = (charPos.x + incrX) * 30; // desired X coordinate
			Inputs[0].mi.dy = (charPos.y + incrY) * 60; // desired Y coordinate
			Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

			//cout << Inputs[0].mi.dx << " " << Inputs[0].mi.dy << endl;

			Inputs[1].type = INPUT_KEYBOARD;
			Inputs[1].ki.wScan = 0; // hardware scan code for key
			Inputs[1].ki.time = 0;
			Inputs[1].ki.dwExtraInfo = 0;
			Inputs[1].ki.wVk = 84; // virtual-key code for the "w" key
			Inputs[1].ki.dwFlags = 0;

			Inputs[2].type = INPUT_KEYBOARD;
			Inputs[2].ki.wScan = 0; // hardware scan code for key
			Inputs[2].ki.time = 0;
			Inputs[2].ki.dwExtraInfo = 0;
			Inputs[2].ki.wVk = 84; // virtual-key code for the "w" key
			Inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

			SendInput(3, Inputs, sizeof(INPUT));

			Sleep(200);
		}
	}

	return 0;
}