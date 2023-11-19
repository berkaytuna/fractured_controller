#include <iostream>
#include <math.h>

#include <Windows.h>
#include <xinput.h>

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

using namespace std;

const POINT charPos({ 960, 475 });

bool canReleaseL2 = false;
bool canReleaseR2 = false;
bool isCursorHidden = false;
bool isMoving = false;

double prevRX = 32768;
double prevRY = 32768;

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

UINT sendNormalKeyPressDirect(int code)
{
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = code;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	return SendInput(1, &ip, sizeof(INPUT));
}

void hideCursor()
{
	if (!isCursorHidden)
	{
		sendNormalKeyPressDirect(219);
		isCursorHidden = true;
	}
}

void showCursor()
{
	if (isCursorHidden)
	{
		sendNormalKeyPressDirect(221);
		isCursorHidden = false;
	}
}

void sendLeftMousePress()
{
	INPUT Inputs[1] = { 0 };
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, Inputs, sizeof(INPUT));
	canReleaseR2 = true;
}

void sendLeftMouseRelease()
{
	INPUT Inputs[1] = { 0 };
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, Inputs, sizeof(INPUT));
	canReleaseR2 = false;
}

void sendRightMouse()
{
	INPUT Inputs[2] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(2, Inputs, sizeof(INPUT));

	if (isMoving)
	{
		isMoving = false;
		Sleep(500);
	}

	INPUT InputDown[1] = { 0 };
	InputDown[0].type = INPUT_MOUSE;
	InputDown[0].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, InputDown, sizeof(INPUT));
}

void sendNormalKeyPress(int code)
{
	hideCursor();

	INPUT Inputs[2] = { 0 };

	Inputs[0].type = INPUT_KEYBOARD;
	Inputs[0].ki.wScan = 0;
	Inputs[0].ki.time = 0;
	Inputs[0].ki.dwExtraInfo = 0;
	Inputs[0].ki.wVk = code;
	Inputs[0].ki.dwFlags = 0;

	Inputs[1].type = INPUT_KEYBOARD;
	Inputs[1].ki.wScan = 0;
	Inputs[1].ki.time = 0;
	Inputs[1].ki.dwExtraInfo = 0;
	Inputs[1].ki.wVk = code;
	Inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, Inputs, sizeof(INPUT));

	Sleep(200);
}

void sendTargetKeyPress(int code)
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

		if (true)
		{
			SetCursorPos(pos.x, pos.y);
			sendNormalKeyPressDirect(code);

			Sleep(1);
		}
	}
}

void sendAoEKeyPress(int code)
{
	double unitX = 0.0;
	double unitY = 0.0;
	getUnitVec(unitX, unitY);
	const double unitCoeff = 400;
	const double incrX = unitCoeff * unitX;
	const double incrY = unitCoeff * unitY;

	INPUT Inputs[3] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = (charPos.x + incrX) * 30;
	Inputs[0].mi.dy = (charPos.y + incrY) * 60;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	Inputs[1].type = INPUT_KEYBOARD;
	Inputs[1].ki.wScan = 0;
	Inputs[1].ki.time = 0;
	Inputs[1].ki.dwExtraInfo = 0;
	Inputs[1].ki.wVk = code;
	Inputs[1].ki.dwFlags = 0;

	Inputs[2].type = INPUT_KEYBOARD;
	Inputs[2].ki.wScan = 0;
	Inputs[2].ki.time = 0;
	Inputs[2].ki.dwExtraInfo = 0;
	Inputs[2].ki.wVk = code;
	Inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(3, Inputs, sizeof(INPUT));

	Sleep(200);
}

void sendKeyPress(int type, int code)
{
	hideCursor();

	if (type == 0)
	{
		sendNormalKeyPress(code);
	}
	else if (type == 1)
	{
		sendTargetKeyPress(code);
	}
	else if (type == 2)
	{
		sendAoEKeyPress(code);
	}
}

int main()
{
	const bool isDebug = 0;

	const int upType = 0;
	const int downType = 0;
	const int leftType = 0;
	const int rightType = 0;
	const int aType0 = 1;
	const int bType0 = 0;
	const int xType0 = 1;
	const int yType0 = 0;
	const int l1Type0 = 0;
	const int r1Type0 = 0;
	const int aType1 = 0;
	const int bType1 = 0;
	const int xType1 = 0;
	const int yType1 = 0;
	const int l1Type1 = 0;
	const int r1Type1 = 0;
	const int l3Type = 0;
	const int r3Type = 2;
	const int backType = 0;
	const int startType = 0;

	const int upCode = 89;
	const int downCode = 89;
	const int leftCode = 89;
	const int rightCode = 89;
	const int aCode0 = 89;
	const int bCode0 = 90;
	const int xCode0 = 11;
	const int yCode0 = 12;
	const int l1Code0 = 13;
	const int r1Code0 = 14;
	const int aCode1 = 15;
	const int bCode1 = 23;
	const int xCode1 = 24;
	const int yCode1 = 25;
	const int l1Code1 = 21;
	const int r1Code1 = 22;
	const int l3Code = 89;
	const int r3Code = 89;
	const int backCode = 13;
	const int startCode = 14;

	while (1)
	{
		bool isAPressed = false;
		bool isBPressed = false;
		bool isYPressed = false;
		bool isXPressed = false;
		bool isL1Pressed = false;
		bool isL3Pressed = false;
		bool isR1Pressed = false;
		bool isR3Pressed = false;
		bool isBackPressed = false;
		bool isStartPressed = false;
		bool isUpPressed = false;
		bool isDownPressed = false;
		bool isLeftPressed = false;
		bool isRightPressed = false;
		bool isL2Pressed = false;
		bool isL2Released = true;
		bool isR2Pressed = false;
		bool isR2Released = true;

		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD const& dwResult = XInputGetState(0, &state);
		if (dwResult == ERROR_SUCCESS)
		{
			WORD buttons = state.Gamepad.wButtons;
			isAPressed = buttons == XINPUT_GAMEPAD_A;
			isBPressed = buttons == XINPUT_GAMEPAD_B;
			isYPressed = buttons == XINPUT_GAMEPAD_Y;
			isXPressed = buttons == XINPUT_GAMEPAD_X;
			isL1Pressed = buttons == XINPUT_GAMEPAD_LEFT_SHOULDER;
			isL3Pressed = buttons == XINPUT_GAMEPAD_LEFT_THUMB;
			isR1Pressed = buttons == XINPUT_GAMEPAD_RIGHT_SHOULDER;
			isR3Pressed = buttons == XINPUT_GAMEPAD_RIGHT_THUMB;
			isUpPressed = buttons == XINPUT_GAMEPAD_DPAD_UP;
			isDownPressed = buttons == XINPUT_GAMEPAD_DPAD_DOWN;
			isLeftPressed = buttons == XINPUT_GAMEPAD_DPAD_LEFT;
			isRightPressed = buttons == XINPUT_GAMEPAD_DPAD_RIGHT;
			isBackPressed = buttons == XINPUT_GAMEPAD_BACK;
			isStartPressed = buttons == XINPUT_GAMEPAD_START;
			isL2Pressed = state.Gamepad.bLeftTrigger > 150;
			isR2Pressed = state.Gamepad.bRightTrigger > 150;
			isL2Released = state.Gamepad.bLeftTrigger == 0;
			isR2Released = state.Gamepad.bRightTrigger == 0;

			double RX = state.Gamepad.sThumbRX + 32768;
			double RY = state.Gamepad.sThumbRY + 32768;

			double LX = state.Gamepad.sThumbLX + 32768;
			double LY = state.Gamepad.sThumbLY + 32768;
			
			if (isDebug)
			{
				cout << LX << " " << LY << endl;
			}

			const double deadzoneR = 8000;
			const double deadZoneL = 9600;
				
			const bool isRightStickMoving =
				(RX <= 32768 - deadzoneR || RX >= 32768 + deadzoneR)
				|| (RY <= 32768 - deadzoneR || RY >= 32768 + deadzoneR);
			const bool isLeftStickMoving =
				(LX <= 32768 - deadZoneL || LX >= 32768 + deadZoneL)
				|| (LY <= 32768 - deadZoneL || LY >= 32768 + deadZoneL);

			if (isR2Pressed)
			{
				sendLeftMousePress();
			}
			else if (canReleaseR2 && isR2Released)
			{
				sendLeftMouseRelease();
			}
			if (isL2Pressed)
			{
				sendRightMouse();
			}
			else if (isUpPressed)
			{
				sendKeyPress(upType, upCode);
			}
			else if (isDownPressed)
			{
				sendKeyPress(downType, downCode);
			}
			else if (isLeftPressed)
			{
				sendKeyPress(leftType, leftCode);
			}
			else if (isRightPressed)
			{
				sendKeyPress(rightType, rightCode);
			}
			else if (isAPressed)
			{
				sendKeyPress(
					isL2Pressed ? aType1 : aType0,
					isL2Pressed ? aCode1 : aCode0);
			}
			else if (isBPressed)
			{
				sendKeyPress(
					isL2Pressed ? bType1 : bType0,
					isL2Pressed ? bCode1 : bCode0);
			}
			else if (isXPressed)
			{
				sendKeyPress(
					isL2Pressed ? xType1 : xType0,
					isL2Pressed ? xCode1 : xCode0);
			}
			else if (isYPressed)
			{
				sendKeyPress(
					isL2Pressed ? yType1 : yType0,
					isL2Pressed ? yCode1 : yCode0);
			}
			else if (isL1Pressed)
			{
				sendKeyPress(
					isL2Pressed ? l1Type1 : l1Type0,
					isL2Pressed ? l1Code1 : l1Code0);
			}
			else if (isR1Pressed)
			{
				sendKeyPress(
					isL2Pressed ? r1Type1 : r1Type0,
					isL2Pressed ? r1Code1 : r1Code0);
			}
			else if (isL3Pressed)
			{
				sendKeyPress(l3Type, l3Code);
			}
			else if (isR3Pressed)
			{
				sendKeyPress(r3Type, r3Code);
			}
			else if (isBackPressed)
			{
				sendKeyPress(backType, backCode);
			}
			else if (isStartPressed)
			{
				sendKeyPress(startType, startCode);
			}
			else if (isRightStickMoving)
			{
				showCursor();

				double currentRX = prevRX + (RX - 32768) * 0.00045;
				currentRX = currentRX < 0 ? 0 : currentRX;
				currentRX = currentRX > 65536 ? 65536 : currentRX;

				double currentRY = prevRY + (65536 - RY - 32768) * 0.00045;
				currentRY = currentRY < 0 ? 0 : currentRY;
				currentRY = currentRY > 65536 ? 65536 : currentRY;

				INPUT Inputs[1] = { 0 };
				Inputs[0].type = INPUT_MOUSE;
				Inputs[0].mi.dx = currentRX;
				Inputs[0].mi.dy = currentRY;
				Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
				
				SendInput(1, Inputs, sizeof(INPUT));

				prevRX = Inputs[0].mi.dx;
				prevRY = Inputs[0].mi.dy;
			}
			else if (isLeftStickMoving)
			{
				hideCursor();

				prevRX = 32768;
				prevRY = 29350;

				if (isMoving)
				{
					INPUT Inputs[1] = { 0 };
					Inputs[0].type = INPUT_MOUSE;
					Inputs[0].mi.dx = 32768 + (LX - 32768) / 7;
					Inputs[0].mi.dy = 29350 + ((65536 - LY) - 32768) / 4.2;
					Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					SendInput(1, Inputs, sizeof(INPUT));
				}
				else
				{
					INPUT Inputs[2] = { 0 };
					Inputs[0].type = INPUT_MOUSE;
					Inputs[0].mi.dx = 32768 + (LX - 32768) / 7;
					Inputs[0].mi.dy = 29350 + ((65536 - LY) - 32768) / 4.2;
					Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					Inputs[1].type = INPUT_MOUSE;
					Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

					SendInput(2, Inputs, sizeof(INPUT));

					isMoving = true;
				}
			}
			else if (isMoving)
			{
				INPUT Inputs[2] = { 0 };

				Inputs[0].type = INPUT_MOUSE;
				Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

				Inputs[1].type = INPUT_MOUSE;
				Inputs[1].mi.dx = 32768;
				Inputs[1].mi.dy = 29350;
				Inputs[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

				SendInput(2, Inputs, sizeof(INPUT));

				isMoving = false;
			}
		}
		else
		{
			cout << "XInput Controller not found with error code: " << dwResult << endl;
		}
	}

	return 0;
}