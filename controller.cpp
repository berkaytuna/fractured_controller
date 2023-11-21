#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <string>

#include <windows.h>
#include <xinput.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

using namespace std;

std::string ahkPath;
int cursorHideCode = 0;
int cursorShowCode = 0;

int upType = 0;
int downType = 0;
int leftType = 0;
int rightType = 0;
int aType0 = 0;
int bType0 = 0;
int xType0 = 0;
int yType0 = 0;
int l1Type0 = 0;
int r1Type0 = 0;
int aType1 = 0;
int bType1 = 0;
int xType1 = 0;
int yType1 = 0;
int l1Type1 = 0;
int r1Type1 = 0;
int l3Type = 0;
int r3Type = 0;
int backType = 0;
int startType = 0;

int upCode = 0;
int downCode = 0;
int leftCode = 0;
int rightCode = 0;
int aCode0 = 0;
int bCode0 = 0;
int xCode0 = 0;
int yCode0 = 0;
int l1Code0 = 0;
int r1Code0 = 0;
int aCode1 = 0;
int bCode1 = 0;
int xCode1 = 0;
int yCode1 = 0;
int l1Code1 = 0;
int r1Code1 = 0;
int l3Code = 0;
int r3Code = 0;
int backCode = 0;
int startCode = 0;

POINT screenRes({ 0, 0 });
POINT charPos({ 0, 0 });

int aoeKeyDistance = 0;
int targetKeyRepeat = 0;
int targetKeyDistance = 0;

int thresholdL2 = 0;
int thresholdR2 = 0;

int deadzoneL = 0;
int deadzoneR = 0;
int sensitivityR = 0;
int initialLX = 0;
int initialLY = 0;
int movCoeffLX = 0;
int movCoeffLY = 0;

HANDLE ahkHandle;

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
		sendNormalKeyPressDirect(cursorHideCode);
		isCursorHidden = true;
	}
}

void showCursor()
{
	if (isCursorHidden)
	{
		sendNormalKeyPressDirect(cursorShowCode);
		isCursorHidden = false;
	}
}

void sendLeftMousePress()
{
	INPUT Inputs[1] = { 0 };
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, Inputs, sizeof(INPUT));
}

void sendLeftMouseRelease()
{
	INPUT Inputs[1] = { 0 };
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, Inputs, sizeof(INPUT));
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
	const double unitCoeff = targetKeyDistance;
	const double incrX = unitCoeff * unitX;
	const double incrY = unitCoeff * unitY;

	POINT pos({ charPos.x, charPos.y });

	for (int j = 0; j < targetKeyRepeat; j++)
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
	const double unitCoeff = aoeKeyDistance;
	const double incrX = unitCoeff * unitX;
	const double incrY = unitCoeff * unitY;

	INPUT Inputs[3] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = (charPos.x + incrX) * (65536 / screenRes.x);
	Inputs[0].mi.dy = (charPos.y + incrY) * (65536 / screenRes.y);
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

void readSettings()
{	
	TCHAR arr[256];
	GetPrivateProfileString(_T("cursor"), _T("ahk_path"), _T(""), arr, 256, _T(".\\settings.ini"));
	std::string arr_w(arr);
	ahkPath = std::string(arr_w.begin(), arr_w.end());

	cursorHideCode = GetPrivateProfileInt(_T("cursor"), _T("hide_code"), 0, _T(".\\settings.ini"));
	cursorShowCode = GetPrivateProfileInt(_T("cursor"), _T("show_code"), 0, _T(".\\settings.ini"));
	
	upCode = GetPrivateProfileInt(_T("button"), _T("up"), 0, _T(".\\settings.ini"));
	downCode = GetPrivateProfileInt(_T("button"), _T("down"), 0, _T(".\\settings.ini"));
	leftCode = GetPrivateProfileInt(_T("button"), _T("left"), 0, _T(".\\settings.ini"));
	rightCode = GetPrivateProfileInt(_T("button"), _T("right"), 0, _T(".\\settings.ini"));
	aCode0 = GetPrivateProfileInt(_T("button"), _T("a"), 0, _T(".\\settings.ini"));
	bCode0 = GetPrivateProfileInt(_T("button"), _T("b"), 0, _T(".\\settings.ini"));
	xCode0 = GetPrivateProfileInt(_T("button"), _T("x"), 0, _T(".\\settings.ini"));
	yCode0 = GetPrivateProfileInt(_T("button"), _T("y"), 0, _T(".\\settings.ini"));
	l1Code0 = GetPrivateProfileInt(_T("button"), _T("l1"), 0, _T(".\\settings.ini"));
	r1Code0 = GetPrivateProfileInt(_T("button"), _T("r1"), 0, _T(".\\settings.ini"));
	aCode1 = GetPrivateProfileInt(_T("button"), _T("a_2"), 0, _T(".\\settings.ini"));
	bCode1 = GetPrivateProfileInt(_T("button"), _T("b_2"), 0, _T(".\\settings.ini"));
	xCode1 = GetPrivateProfileInt(_T("button"), _T("x_2"), 0, _T(".\\settings.ini"));
	yCode1 = GetPrivateProfileInt(_T("button"), _T("y_2"), 0, _T(".\\settings.ini"));
	l1Code1 = GetPrivateProfileInt(_T("button"), _T("l1_2"), 0, _T(".\\settings.ini"));
	r1Code1 = GetPrivateProfileInt(_T("button"), _T("r1_2"), 0, _T(".\\settings.ini"));
	l3Code = GetPrivateProfileInt(_T("button"), _T("l3"), 0, _T(".\\settings.ini"));
	r3Code = GetPrivateProfileInt(_T("button"), _T("r3"), 0, _T(".\\settings.ini"));
	backCode = GetPrivateProfileInt(_T("button"), _T("back"), 0, _T(".\\settings.ini"));
	startCode = GetPrivateProfileInt(_T("button"), _T("start"), 0, _T(".\\settings.ini"));
	
	upType = GetPrivateProfileInt(_T("type"), _T("up"), 0, _T(".\\settings.ini"));
	downType = GetPrivateProfileInt(_T("type"), _T("down"), 0, _T(".\\settings.ini"));
	leftType = GetPrivateProfileInt(_T("type"), _T("left"), 0, _T(".\\settings.ini"));
	rightType = GetPrivateProfileInt(_T("type"), _T("right"), 0, _T(".\\settings.ini"));
	aType0 = GetPrivateProfileInt(_T("type"), _T("a"), 0, _T(".\\settings.ini"));
	bType0 = GetPrivateProfileInt(_T("type"), _T("b"), 0, _T(".\\settings.ini"));
	xType0 = GetPrivateProfileInt(_T("type"), _T("x"), 0, _T(".\\settings.ini"));
	yType0 = GetPrivateProfileInt(_T("type"), _T("y"), 0, _T(".\\settings.ini"));
	l1Type0 = GetPrivateProfileInt(_T("type"), _T("l1"), 0, _T(".\\settings.ini"));
	r1Type0 = GetPrivateProfileInt(_T("type"), _T("r1"), 0, _T(".\\settings.ini"));
	aType1 = GetPrivateProfileInt(_T("type"), _T("a_2"), 0, _T(".\\settings.ini"));
	bType1 = GetPrivateProfileInt(_T("type"), _T("b_2"), 0, _T(".\\settings.ini"));
	xType1 = GetPrivateProfileInt(_T("type"), _T("x_2"), 0, _T(".\\settings.ini"));
	yType1 = GetPrivateProfileInt(_T("type"), _T("y_2"), 0, _T(".\\settings.ini"));
	l1Type1 = GetPrivateProfileInt(_T("type"), _T("l1_2"), 0, _T(".\\settings.ini"));
	r1Type1 = GetPrivateProfileInt(_T("type"), _T("r1_2"), 0, _T(".\\settings.ini"));
	backType = GetPrivateProfileInt(_T("type"), _T("back"), 0, _T(".\\settings.ini"));
	startType = GetPrivateProfileInt(_T("type"), _T("start"), 0, _T(".\\settings.ini"));

	screenRes.x = GetPrivateProfileInt(_T("screen"), _T("res_x"), 0, _T(".\\settings.ini"));
	screenRes.y = GetPrivateProfileInt(_T("screen"), _T("res_y"), 0, _T(".\\settings.ini"));

	charPos.x = GetPrivateProfileInt(_T("character"), _T("pos_x"), 0, _T(".\\settings.ini"));
	charPos.y = GetPrivateProfileInt(_T("character"), _T("pos_y"), 0, _T(".\\settings.ini"));

	aoeKeyDistance = GetPrivateProfileInt(_T("ability"), _T("aoe_key_distance"), 0, _T(".\\settings.ini"));
	targetKeyRepeat = GetPrivateProfileInt(_T("ability"), _T("target_key_repeat"), 0, _T(".\\settings.ini"));
	targetKeyDistance = GetPrivateProfileInt(_T("ability"), _T("target_key_distance"), 0, _T(".\\settings.ini"));

	thresholdL2 = GetPrivateProfileInt(_T("trigger"), _T("threshold_l"), 0, _T(".\\settings.ini"));
	thresholdR2 = GetPrivateProfileInt(_T("trigger"), _T("threshold_r"), 0, _T(".\\settings.ini"));

	deadzoneL = GetPrivateProfileInt(_T("stick"), _T("deadzone_l"), 0, _T(".\\settings.ini"));
	deadzoneR = GetPrivateProfileInt(_T("stick"), _T("deadzone_r"), 0, _T(".\\settings.ini"));
	sensitivityR = GetPrivateProfileInt(_T("stick"), _T("sensitivity_r"), 0, _T(".\\settings.ini"));
	initialLX = GetPrivateProfileInt(_T("stick"), _T("initial_lx"), 0, _T(".\\settings.ini"));
	initialLY = GetPrivateProfileInt(_T("stick"), _T("initial_ly"), 0, _T(".\\settings.ini"));
	movCoeffLX = GetPrivateProfileInt(_T("stick"), _T("mov_coeff_lx"), 0, _T(".\\settings.ini"));
	movCoeffLY = GetPrivateProfileInt(_T("stick"), _T("mov_coeff_ly"), 0, _T(".\\settings.ini"));
}

void initializeAhkFile()
{
	std::stringstream streamHide;
	streamHide << std::hex << cursorHideCode;
	std::string cursorHideCodeHex(streamHide.str());

	std::stringstream streamShow;
	streamShow << std::hex << cursorShowCode;
	std::string cursorShowCodeHex(streamShow.str());

	std::ofstream file("hidecursor.ahk");
	std::string text =
		"detectHiddenWindows, On\n"
		"gui +hwndgHwnd\n"
		"gui,show,hide w1 h1\n"
		"winset,transparent,1,ahk_id %gHwnd%\n"
		"gui +alwaysOnTop +toolWindow -caption +0x80000000\n"
		"return\n"
		"\n"
		"tog:=0\n"
		"\n"
		"VK" + cursorHideCodeHex + "::\n"
		"if (!tog)\n"
		"{\n"
		"    dllcall(\"ShowCursor\",\"uint\",0)\n"
		"    mousegetpos,mx,my,active\n"
		"    gui +Owner%active%\n"
		"    gui,show,x%mx% y%my% noactivate\n"
		"    tog:=1\n"
		"}\n"
		"return\n"
		"\n"
		"VK" + cursorShowCodeHex + "::\n"
		"if (tog)\n"
		"{\n"
		"    gui,cancel\n"
		"    dllcall(\"ShowCursor\",\"uint\",1)\n"
		"    tog:=0\n"
		"}\n"
		"return";

	file << text << std::endl;
	file.close();
}

void initializeAhkProcess()
{
	STARTUPINFOW process_startup_info{ 0 };
	process_startup_info.cb = sizeof(process_startup_info);

	PROCESS_INFORMATION process_info{ 0 };

	std::string cmdStr =
		"\""
		+ ahkPath
		+ "\""
		" \""
		".\\hidecursor.ahk"
		"\"";

	const char* msg = cmdStr.c_str();
	size_t size = strlen(msg) + 1;
	wchar_t* wmsg = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, wmsg, size, msg, size - 1);

	if (CreateProcessW(NULL, wmsg, NULL, NULL, TRUE, 0, NULL, NULL, &process_startup_info, &process_info))
	{
		//WaitForSingleObject(process_info.hProcess, INFINITE); // uncomment to wait till process finish
		ahkHandle = process_info.hProcess;
		//CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
	}
	else
	{
		std::cout << "Please check ahk exe path!" << std::endl;
	}

	delete[]wmsg;
}

void startAhkProcess()
{	
	initializeAhkFile();
	initializeAhkProcess();
}

BOOL WINAPI HandlerRoutine(DWORD eventCode)
{
	switch (eventCode)
	{
	case CTRL_CLOSE_EVENT:
		
		DWORD exitCode = 0;
		TerminateProcess(ahkHandle, GetExitCodeProcess(ahkHandle, &exitCode));

		return FALSE;
		break;
	}

	return TRUE;
}

int main()
{
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);

	readSettings();
	startAhkProcess();
	
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
			isL2Pressed = state.Gamepad.bLeftTrigger > thresholdL2;
			isR2Pressed = state.Gamepad.bRightTrigger > thresholdR2;
			isL2Released = state.Gamepad.bLeftTrigger == 0;
			isR2Released = state.Gamepad.bRightTrigger == 0;

			double RX = state.Gamepad.sThumbRX + 32768;
			double RY = state.Gamepad.sThumbRY + 32768;

			double LX = state.Gamepad.sThumbLX + 32768;
			double LY = state.Gamepad.sThumbLY + 32768;

			const bool isRightStickMoving =
				(RX <= 32768 - deadzoneR || RX >= 32768 + deadzoneR)
				|| (RY <= 32768 - deadzoneR || RY >= 32768 + deadzoneR);
			const bool isLeftStickMoving =
				(LX <= 32768 - deadzoneL || LX >= 32768 + deadzoneL)
				|| (LY <= 32768 - deadzoneL || LY >= 32768 + deadzoneL);

			if (isR2Pressed)
			{
				if (isL2Pressed)
				{
					sendRightMouse();
				}
				else
				{
					sendLeftMousePress();
				}
				canReleaseR2 = true;
			}
			else if (canReleaseR2 && isR2Released)
			{
				if (!isL2Pressed)
				{
					sendLeftMouseRelease();
				}
				canReleaseR2 = false;
			}
			if (isL2Pressed)
			{
				canReleaseL2 = true;
			}
			else if (canReleaseL2 && isL2Released)
			{
				canReleaseL2 = false;
			}
			if (isUpPressed)
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

				double currentRX = prevRX + (RX - 32768) * sensitivityR / 100;
				currentRX = currentRX < 0 ? 0 : currentRX;
				currentRX = currentRX > 65536 ? 65536 : currentRX;

				double currentRY = prevRY + (65536 - RY - 32768) * sensitivityR / 100;
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

				Sleep(1);
			}
			else if (isLeftStickMoving)
			{
				hideCursor();

				prevRX = initialLX;
				prevRY = initialLY;

				if (isMoving)
				{
					INPUT Inputs[1] = { 0 };
					Inputs[0].type = INPUT_MOUSE;
					Inputs[0].mi.dx = initialLX + (LX - 32768) / (movCoeffLX / 10);
					Inputs[0].mi.dy = initialLY + ((65536 - LY) - 32768) / (movCoeffLY / 10);
					Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					SendInput(1, Inputs, sizeof(INPUT));
				}
				else
				{
					INPUT Inputs[2] = { 0 };
					Inputs[0].type = INPUT_MOUSE;
					Inputs[0].mi.dx = initialLX + (LX - 32768) / (movCoeffLX / 10);
					Inputs[0].mi.dy = initialLY + ((65536 - LY) - 32768) / (movCoeffLY / 10);
					Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					Inputs[1].type = INPUT_MOUSE;
					Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

					SendInput(2, Inputs, sizeof(INPUT));

					isMoving = true;
				}

				Sleep(1);
			}
			else if (isMoving)
			{
				INPUT Inputs[2] = { 0 };

				Inputs[0].type = INPUT_MOUSE;
				Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

				Inputs[1].type = INPUT_MOUSE;
				Inputs[1].mi.dx = initialLX;
				Inputs[1].mi.dy = initialLY;
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