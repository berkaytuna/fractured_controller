#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <string>
#include <thread>
#include "wtypes.h"

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
int autorunCode = 0;

int hotkeyXPosInit = 0;
int hotkeyXPosIncr = 0;
int hotkeyYPos = 0;

int upHotKeyPosition = 0;
int downHotKeyPosition = 0;
int leftHotKeyPosition = 0;
int rightHotKeyPosition = 0;
int aHotKeyPosition0 = 0;
int bHotKeyPosition0 = 0;
int xHotKeyPosition0 = 0;
int yHotKeyPosition0 = 0;
int l1HotKeyPosition0 = 0;
int r1HotKeyPosition0 = 0;
int aHotKeyPosition1 = 0;
int bHotKeyPosition1 = 0;
int xHotKeyPosition1 = 0;
int yHotKeyPosition1 = 0;
int l1HotKeyPosition1 = 0;
int r1HotKeyPosition1 = 0;
int l3HotKeyPosition = 0;
int r3HotKeyPosition = 0;
int backHotKeyPosition = 0;
int startHotKeyPosition = 0;
int autorunHotKeyPosition = 0;

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

bool isTarget(/*POINT pos, int& inRed, int& inGreen, int& inBlue*/ )
{
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, 256, 256);
	HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	GetCursorInfo(&ci);
	DrawIcon(hdcMem, 0, 0, ci.hCursor);
	COLORREF clr = GetPixel(hdcMem, 0, 0);

	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmCanvas);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	double const avgRed = GetRValue(clr);
	double const avgGreen = GetGValue(clr);
	double const avgBlue = GetBValue(clr);

	//const bool cond1 = inRed == avgRed;
	//const bool cond2 = inGreen == avgGreen;
	//const bool cond3 = inBlue == avgBlue;
	//const bool result = !cond1 || !cond2 || !cond3;
	const bool cond1 = avgRed == 65;
	const bool cond2 = avgGreen == 60;
	const bool cond3 = avgBlue == 56;
	const bool result = cond1 && cond2 && cond3;
	std::cout
	//	<< "pos: " << pos.x
	//	<< " " << pos.y
		<< ", RGB: " << avgRed
		<< " " << avgGreen
		<< " " << avgBlue
		<< " " << (result ? "true" : "false")
		<< endl;

	//inRed = avgRed;
	//inGreen = avgGreen;
	//inBlue = avgBlue;

	return result;
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

void sendRightMousePress()
{
	INPUT Inputs[1] = { 0 };

	//Inputs[0].type = INPUT_MOUSE;
	//Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, Inputs, sizeof(INPUT));

	//if (isMoving)
	//{
	//	isMoving = false;
	//	Sleep(500);
	//}
}

void sendRightMouseRelease()
{
	INPUT Inputs[1] = { 0 };
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, Inputs, sizeof(INPUT));
}

void sendNormalKeyPress(int code)
{
	//hideCursor();

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

bool sendTargetKeyPressTo(int const code, double const unitX, double const unitY)
{
	const double unitCoeff = targetKeyDistance * (1 + pow((1 + unitY), 2) / 2);
	const double incrX = unitCoeff * unitX;
	const double incrY = unitCoeff * unitY;

	POINT pos({ charPos.x, charPos.y });

	//int prevRed = -1;
	//int prevGreen = -1;
	//int prevBlue = -1;
	for (int j = 0; j < targetKeyRepeat; j++)
	{
		pos.x += incrX;
		pos.y += incrY;

		SetCursorPos(pos.x, pos.y);
		Sleep(1);
		bool const doesHitTarget = isTarget();
		if (doesHitTarget)
		{
			sendNormalKeyPressDirect(code);
			std::cout << "keykeykey" << std::endl;
			return true;
		}
	}

	return false;

	//INPUT Inputs[3] = { 0 };
	//
	//Inputs[0].type = INPUT_MOUSE;
	//Inputs[0].mi.dx = initialLX;
	//Inputs[0].mi.dy = initialLY;
	//Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	//
	//Inputs[1].type = INPUT_KEYBOARD;
	//Inputs[1].ki.wScan = 0;
	//Inputs[1].ki.time = 0;
	//Inputs[1].ki.dwExtraInfo = 0;
	//Inputs[1].ki.wVk = code;
	//Inputs[1].ki.dwFlags = 0;
	//
	//Inputs[2].type = INPUT_KEYBOARD;
	//Inputs[2].ki.wScan = 0;
	//Inputs[2].ki.time = 0;
	//Inputs[2].ki.dwExtraInfo = 0;
	//Inputs[2].ki.wVk = code;
	//Inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
	//
	//SendInput(3, Inputs, sizeof(INPUT));

	//hideCursor();
	//Sleep(200);
}

void sendTargetKeyPress(int code)
{
	//showCursor();

	double unitX = 0.0;
	double unitY = 0.0;
	getUnitVec(unitX, unitY);
	if (sendTargetKeyPressTo(code, unitX, unitY))
	{
		Sleep(200);
		return;
	}

	//double const interval = 0.0873;
	//double const rad = std::asin(unitY);
	//double const radPlus = rad + interval;
	//double const radMinus = rad - interval;
	//double const cosPlus = std::cos(radPlus);
	//double const plusUnitX = unitX < 0 ? -1 * cosPlus : cosPlus;
	//double const plusUnitY = std::sin(radPlus);
	//if (sendTargetKeyPressTo(code, plusUnitX, plusUnitY))
	//{
	//	Sleep(200);
	//	return;
	//}
	//
	//double const cosMinus = std::cos(radMinus);
	//double const minusUnitX = unitX < 0 ? -1 * cosMinus : cosMinus;
	//double const minusUnitY = std::sin(radMinus);
	//if (sendTargetKeyPressTo(code, minusUnitX, minusUnitY))
	//{
	//	Sleep(200);
	//	return;
	//}
	//
	//std::cout << unitX << " " << plusUnitX << " " << minusUnitX << std::endl;
	//std::cout << unitY << " " << plusUnitY << " " << minusUnitY << std::endl;
}

void sendAoEKeyPress(int code)
{
	double unitX = 0.0;
	double unitY = 0.0;
	getUnitVec(unitX, unitY);
	//std::cout << unitX << " " << unitY << std::endl;
	const double unitCoeff = aoeKeyDistance * (1 + pow((1 + unitY), 2) / 2);
	const double incrX = unitCoeff * unitX;
	const double incrY = unitCoeff * unitY;

	INPUT Inputs[1] = { 0 };
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = (charPos.x + incrX) * (65536 / screenRes.x);
	Inputs[0].mi.dy = (charPos.y + incrY) * (65536 / screenRes.y);
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1, Inputs, sizeof(INPUT));
	Sleep(100);

	INPUT InputsA[2] = { 0 };
	InputsA[0].type = INPUT_KEYBOARD;
	InputsA[0].ki.wScan = 0;
	InputsA[0].ki.time = 0;
	InputsA[0].ki.dwExtraInfo = 0;
	InputsA[0].ki.wVk = code;
	InputsA[0].ki.dwFlags = 0;

	InputsA[1].type = INPUT_KEYBOARD;
	InputsA[1].ki.wScan = 0;
	InputsA[1].ki.time = 0;
	InputsA[1].ki.dwExtraInfo = 0;
	InputsA[1].ki.wVk = code;
	InputsA[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, InputsA, sizeof(INPUT));

	Sleep(200);
}

int getHotKeyX(int const hotkeyPos)
{
	return hotkeyXPosInit + ((hotkeyPos - 1) *  hotkeyXPosIncr);
}

void sendAutorunIfSkillFired(HDC dc, COLORREF prevColor, int const hotkeyXPos)
{
	int const prevColorR = (int)GetRValue(prevColor);
	int const prevColorG = (int)GetGValue(prevColor);
	int const prevColorB = (int)GetBValue(prevColor);

	cout
		<< (int)GetRValue(prevColor)
		<< " "
		<< (int)GetGValue(prevColor)
		<< " "
		<< (int)GetBValue(prevColor)
		<< endl;

	COLORREF color = GetPixel(dc, hotkeyXPos, hotkeyYPos);
	int const colorR = (int)GetRValue(color);
	int const colorG = (int)GetGValue(color);
	int const colorB = (int)GetBValue(color);

	bool const cond =
		colorR == prevColorR
		&& colorG == prevColorG
		&& colorB == prevColorB;
	if (!cond)
	{
		cout << "abc" << endl;
		sendNormalKeyPressDirect(autorunCode);
	}

	cout
		<< (int)GetRValue(GetPixel(dc, hotkeyXPos, hotkeyYPos))
		<< " "
		<< (int)GetGValue(GetPixel(dc, hotkeyXPos, hotkeyYPos))
		<< " "
		<< (int)GetBValue(GetPixel(dc, hotkeyXPos, hotkeyYPos))
		<< endl;
}

void sendKeyPress(int type, int code, int hotkeyPos)
{
	//hideCursor();

	HDC dc = GetDC(NULL);
	int const hotkeyXPos = getHotKeyX(hotkeyPos);
	COLORREF prevColor = GetPixel(dc, hotkeyXPos, hotkeyYPos);

	if (type == 0)
	{
		sendNormalKeyPress(code);
		//sendNormalKeyPressDirect(autorunCode);
		//sendTargetKeyPress(code);
	}
	else if (type == 1)
	{
		sendTargetKeyPress(code);
		//sendNormalKeyPressDirect(autorunCode);
	}
	else if (type == 2)
	{
		sendAoEKeyPress(code);
		//sendNormalKeyPressDirect(autorunCode);
	}
	if (type == 3)
	{
		sendNormalKeyPress(code);
	}

	ReleaseDC(NULL,dc);
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
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
	autorunCode = GetPrivateProfileInt(_T("button"), _T("autorun"), 0, _T(".\\settings.ini"));
	
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
	l3Type = GetPrivateProfileInt(_T("type"), _T("l3"), 0, _T(".\\settings.ini"));
	r3Type = GetPrivateProfileInt(_T("type"), _T("r3"), 0, _T(".\\settings.ini"));
	backType = GetPrivateProfileInt(_T("type"), _T("back"), 0, _T(".\\settings.ini"));
	startType = GetPrivateProfileInt(_T("type"), _T("start"), 0, _T(".\\settings.ini"));

	hotkeyXPosInit = GetPrivateProfileInt(_T("hotkey"), _T("x_pos_init"), 0, _T(".\\settings.ini"));
	hotkeyXPosIncr = GetPrivateProfileInt(_T("hotkey"), _T("x_pos_incr"), 0, _T(".\\settings.ini"));
	hotkeyYPos = GetPrivateProfileInt(_T("hotkey"), _T("y_pos"), 0, _T(".\\settings.ini"));

	upHotKeyPosition = GetPrivateProfileInt(_T("hotkey"), _T("up"), 0, _T(".\\settings.ini"));
	downHotKeyPosition = GetPrivateProfileInt(_T("hotkey"), _T("down"), 0, _T(".\\settings.ini"));
	leftHotKeyPosition = GetPrivateProfileInt(_T("hotkey"), _T("left"), 0, _T(".\\settings.ini"));
	rightHotKeyPosition = GetPrivateProfileInt(_T("hotkey"), _T("right"), 0, _T(".\\settings.ini"));
	aHotKeyPosition0 = GetPrivateProfileInt(_T("hotkey"), _T("a"), 0, _T(".\\settings.ini"));
	bHotKeyPosition0 = GetPrivateProfileInt(_T("hotkey"), _T("b"), 0, _T(".\\settings.ini"));
	xHotKeyPosition0 = GetPrivateProfileInt(_T("hotkey"), _T("x"), 0, _T(".\\settings.ini"));
	yHotKeyPosition0 = GetPrivateProfileInt(_T("hotkey"), _T("y"), 0, _T(".\\settings.ini"));
	l1HotKeyPosition0 = GetPrivateProfileInt(_T("hotkey"), _T("l1"), 0, _T(".\\settings.ini"));
	r1HotKeyPosition0 = GetPrivateProfileInt(_T("hotkey"), _T("r1"), 0, _T(".\\settings.ini"));
	aHotKeyPosition1 = GetPrivateProfileInt(_T("hotkey"), _T("a_2"), 0, _T(".\\settings.ini"));
	bHotKeyPosition1 = GetPrivateProfileInt(_T("hotkey"), _T("b_2"), 0, _T(".\\settings.ini"));
	xHotKeyPosition1 = GetPrivateProfileInt(_T("hotkey"), _T("x_2"), 0, _T(".\\settings.ini"));
	yHotKeyPosition1 = GetPrivateProfileInt(_T("hotkey"), _T("y_2"), 0, _T(".\\settings.ini"));
	l1HotKeyPosition1 = GetPrivateProfileInt(_T("hotkey"), _T("l1_2"), 0, _T(".\\settings.ini"));
	r1HotKeyPosition1 = GetPrivateProfileInt(_T("hotkey"), _T("r1_2"), 0, _T(".\\settings.ini"));
	backHotKeyPosition = GetPrivateProfileInt(_T("hotkey"), _T("back"), 0, _T(".\\settings.ini"));
	startHotKeyPosition = GetPrivateProfileInt(_T("hotkey"), _T("start"), 0, _T(".\\settings.ini"));

	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);
	//cout << horizontal << '\n' << vertical << '\n';
	//screenRes.x = GetPrivateProfileInt(_T("screen"), _T("res_x"), 0, _T(".\\settings.ini"));
	//screenRes.y = GetPrivateProfileInt(_T("screen"), _T("res_y"), 0, _T(".\\settings.ini"));
	screenRes.x = (LONG)horizontal;
	screenRes.y = (LONG)vertical;

	//charPos.x = GetPrivateProfileInt(_T("character"), _T("pos_x"), 0, _T(".\\settings.ini"));
	//charPos.y = GetPrivateProfileInt(_T("character"), _T("pos_y"), 0, _T(".\\settings.ini"));
	charPos.x = screenRes.x * 0.50;
	charPos.y = screenRes.y * 0.44;

	//aoeKeyDistance = GetPrivateProfileInt(_T("ability"), _T("aoe_key_distance"), 0, _T(".\\settings.ini"));
	aoeKeyDistance = charPos.y * 0.80;
	targetKeyRepeat = GetPrivateProfileInt(_T("ability"), _T("target_key_repeat"), 0, _T(".\\settings.ini"));
	//targetKeyDistance = GetPrivateProfileInt(_T("ability"), _T("target_key_distance"), 0, _T(".\\settings.ini"));
	targetKeyDistance = aoeKeyDistance / targetKeyRepeat;

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

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD dwResult = XInputGetState(0, &state);
	while (dwResult != ERROR_SUCCESS)
	{
		dwResult = XInputGetState(0, &state);
		cout << "XInput Controller not found with error code: " << dwResult << endl;
		Sleep(1000);
		//break;
	};

	double prevRX = initialLX;
	double prevRY = initialLY;
	while (1)
	{
		//char ch = _getch();
		//if (ch == 'k')
		//{
		//	sendTargetKeyPress(69);
		//}
		//Sleep(5000);
		//continue;

		XInputGetState(0, &state);

		WORD buttons = state.Gamepad.wButtons;
		bool const isAPressed = buttons == XINPUT_GAMEPAD_A;
		bool const isBPressed = buttons == XINPUT_GAMEPAD_B;
		bool const isYPressed = buttons == XINPUT_GAMEPAD_Y;
		bool const isXPressed = buttons == XINPUT_GAMEPAD_X;
		bool const isL1Pressed = buttons == XINPUT_GAMEPAD_LEFT_SHOULDER;
		bool const isL3Pressed = buttons == XINPUT_GAMEPAD_LEFT_THUMB;
		bool const isR1Pressed = buttons == XINPUT_GAMEPAD_RIGHT_SHOULDER;
		bool const isR3Pressed = buttons == XINPUT_GAMEPAD_RIGHT_THUMB;
		bool const isUpPressed = buttons == XINPUT_GAMEPAD_DPAD_UP;
		bool const isDownPressed = buttons == XINPUT_GAMEPAD_DPAD_DOWN;
		bool const isLeftPressed = buttons == XINPUT_GAMEPAD_DPAD_LEFT;
		bool const isRightPressed = buttons == XINPUT_GAMEPAD_DPAD_RIGHT;
		bool const isBackPressed = buttons == XINPUT_GAMEPAD_BACK;
		bool const isStartPressed = buttons == XINPUT_GAMEPAD_START;
		bool const isL2Pressed = state.Gamepad.bLeftTrigger > thresholdL2;
		bool const isR2Pressed = state.Gamepad.bRightTrigger > thresholdR2;
		bool const isL2Released = state.Gamepad.bLeftTrigger == 0;
		bool const isR2Released = state.Gamepad.bRightTrigger == 0;

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
				sendRightMousePress();
			}
			else
			{
				sendLeftMousePress();
			}
			canReleaseR2 = true;
		}
		else if (canReleaseR2 && isR2Released)
		{
			if (isL2Pressed)
			{
				sendRightMouseRelease();
			}
			else
			{
				sendLeftMouseRelease();
			}
			canReleaseR2 = false;
		}

		//if (isL2Pressed)
		//{
		//	canReleaseL2 = true;
		//}
		//else if (canReleaseL2 && isL2Released)
		//{
		//	canReleaseL2 = false;
		//}

		if (isRightStickMoving)
		{
			//showCursor();

			double currentRX = prevRX + (RX - 32768) * sensitivityR / 1000;
			currentRX = currentRX < 0 ? 0 : currentRX;
			currentRX = currentRX > 65536 ? 65536 : currentRX;

			double currentRY = prevRY + (65536 - RY - 32768) * sensitivityR / 1000;
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
			//hideCursor();

			//prevRX = initialLX;
			//prevRY = initialLY;

			//LX -= 32768;
			//LY -= 32768;
			//double const H = 32768;
			//double const h = std::sqrt(std::pow(LX, 2) + std::pow(LY, 2));
			//LX = 32768 + H * LX / h;
			//LY = 32768 + H * LY / h;

			//std::cout << LX << " " << LY << std::endl;

			const double nextLX = (initialLX + (LX - 32768) / (movCoeffLX / 10));
			const double nextLY = (initialLY + ((65536 - LY) - 32768) / (movCoeffLY / 10));

			if (isMoving)
			{
				INPUT Inputs[1] = { 0 };
				Inputs[0].type = INPUT_MOUSE;
				Inputs[0].mi.dx = nextLX;
				Inputs[0].mi.dy = nextLY;
				Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
				SendInput(1, Inputs, sizeof(INPUT));
			}
			else
			{
				isMoving = true;

				INPUT Inputs[1] = { 0 };
				Inputs[0].type = INPUT_MOUSE;
				Inputs[0].mi.dx = nextLX;
				Inputs[0].mi.dy = nextLY;
				Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
				SendInput(1, Inputs, sizeof(INPUT));

				//sendNormalKeyPressDirect(autorunCode);
				sendLeftMousePress();
			}

			prevRX = nextLX;
			prevRY = nextLY;
		}
		else if (isMoving)
		{
			INPUT Inputs[1] = { 0 };

			Inputs[0].type = INPUT_MOUSE;
			Inputs[0].mi.dx = initialLX;
			Inputs[0].mi.dy = initialLY;
			Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

			//sendNormalKeyPressDirect(autorunCode);
			sendLeftMouseRelease();

			isMoving = false;
		}

		if (isUpPressed)
		{
			sendKeyPress(upType, upCode, upHotKeyPosition);
		}
		else if (isDownPressed)
		{
			sendKeyPress(downType, downCode, downHotKeyPosition);
		}
		else if (isLeftPressed)
		{
			sendKeyPress(leftType, leftCode, leftHotKeyPosition);
		}
		else if (isRightPressed)
		{
			sendKeyPress(rightType, rightCode, rightHotKeyPosition);
		}
		else if (isAPressed)
		{
			sendKeyPress(
				isL2Pressed ? aType1 : aType0,
				isL2Pressed ? aCode1 : aCode0,
				isL2Pressed ? aHotKeyPosition1 : aHotKeyPosition0);
		}
		else if (isBPressed)
		{
			sendKeyPress(
				isL2Pressed ? bType1 : bType0,
				isL2Pressed ? bCode1 : bCode0,
				isL2Pressed ? bHotKeyPosition1 : bHotKeyPosition0);
		}
		else if (isXPressed)
		{
			sendKeyPress(
				isL2Pressed ? xType1 : xType0,
				isL2Pressed ? xCode1 : xCode0,
				isL2Pressed ? xHotKeyPosition1 : xHotKeyPosition0);
		}
		else if (isYPressed)
		{
			sendKeyPress(
				isL2Pressed ? yType1 : yType0,
				isL2Pressed ? yCode1 : yCode0,
				isL2Pressed ? yHotKeyPosition1 : yHotKeyPosition0);
		}
		else if (isL1Pressed)
		{
			sendKeyPress(
				isL2Pressed ? l1Type1 : l1Type0,
				isL2Pressed ? l1Code1 : l1Code0,
				isL2Pressed ? l1HotKeyPosition1 : l1HotKeyPosition0);
		}
		else if (isR1Pressed)
		{
			sendKeyPress(
				isL2Pressed ? r1Type1 : r1Type0,
				isL2Pressed ? r1Code1 : r1Code0,
				isL2Pressed ? r1HotKeyPosition1 : r1HotKeyPosition0);
		}
		else if (isL3Pressed)
		{
			sendKeyPress(l3Type, l3Code, l3HotKeyPosition);
		}
		else if (isR3Pressed)
		{
			sendKeyPress(r3Type, r3Code, r3HotKeyPosition);
		}
		else if (isBackPressed)
		{
			sendKeyPress(backType, backCode, backHotKeyPosition);
		}
		else if (isStartPressed)
		{
			sendKeyPress(startType, startCode, startHotKeyPosition);
		}

		Sleep(1);
	}

	return 0;
}