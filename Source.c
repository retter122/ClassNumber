#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "Neuro.h"
// WINDOW PARAMS
#define WinScaleX 256
#define WinScaleY 64
#define WindowSizeX BufferSizeX + WinScaleX
#define WindowSizeY BufferSizeY + WinScaleY
// WINDOW DATA
static const char WName[] = "Num class";

static WNDPROC WProc(HWND hWnd, UINT Mess, WPARAM w, LPARAM l);

static WNDCLASSA WClass = { CS_HREDRAW | CS_VREDRAW, WProc, 0, 0, 0, 0, 0, 0, 0, "Neuro" };
static MSG Mess;
static HDC Dc;

static HDC MDc;
static HBITMAP HBm;
// PIXEL BUFFER DATA
typedef struct { unsigned int left, top, right, bottom; } RCT;

static DWORD* PixBuffer;
static RCT PixRect;
static BITMAPINFOHEADER Bmi = { sizeof(BITMAPINFOHEADER), 0, 0, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };

static DWORD Buffer[BufferSizeX * BufferSizeY];
// INTERFACE PARAMS
#define IndexingFile 1
#define LearningModel 2
// INTERFACE DATA
static LOGFONTA Font = { 28, 12, 0, 0, 700, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 0, 0, "Arial" };
static HFONT HFont;

static const char HelpInterface[] = "0-9: Сохранить локально, Enter: Дообучить модель";
static const char Status[] = "Подождите, идет индексация файлов               ";
static const char Learning[] = "Подождите, идет обучение модели                 ";

static const char Indi[10][3] = { "0:", "1:", "2:", "3:", "4:", "5:", "6:", "7:", "8:", "9:" };

static unsigned char StatusCode = IndexingFile;
// GRAPHICS API
#define ToRGB(r, g, b) (r << 16) | (g << 8) | b

#define DrawRect(x, y, Sx, Sy, Color) DrawRectFunc((Sx << 16) | x, y, Sy, Color);
// GRAPHICS FUCTIONS
static void __fastcall DrawFromBuffer();
static void __fastcall ClearBuffer();
static void __fastcall BufferDraw(int x, int y);
static void __fastcall DrawToText(const char* Path);
static void __fastcall DrawFromSvertka();

static void __fastcall DrawBackground();
static void __fastcall DrawRectFunc(unsigned int XSx, unsigned int y, unsigned int sy, DWORD Color);
// FILESYSTEM FUNCTION
static void __fastcall IndexFiles();
// MAIN FUNCTION
static int WinMain() {
// APP INIT
	ModelInit();

	HFont = CreateFontIndirectA(&Font);
// INIT AND SHOW WINDOW
	WClass.hCursor = LoadCursorA(0, IDC_ARROW);
	if (!RegisterClassA(&WClass)) ExitProcess(1);
	ShowWindow(Dc = CreateWindowExA(0, WClass.lpszClassName, WName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, 0, 0, WindowSizeX, WindowSizeY, 0, 0, 0, 0), SW_SHOWNORMAL);
// PIXBUFFER INIT
	GetClientRect(Dc, &PixRect);
	PixBuffer = malloc((PixRect.right * PixRect.bottom) << 2);
	Bmi.biWidth = PixRect.right;
	Bmi.biHeight = PixRect.bottom;
// INIFITY CYCLE
	Dc = GetDC(Dc);
	CYC: {
		if (PeekMessageA(&Mess, 0, 0, 0, PM_REMOVE)) DispatchMessageA(&Mess);
	} goto CYC;

	return 0;
}
// WINDOW PROCEDURE
static WNDPROC WProc(HWND hWnd, UINT Mess, WPARAM w, LPARAM l) {
	switch (Mess) {
// WINDOW DESTROY MESSAGE
		case (WM_DESTROY):
			ExitProcess(0);
			return 0;
// MOUSE DOWN EVENT
		case (WM_MOUSEMOVE):
			if (w == MK_LBUTTON && ((l & 0x0000FFFF) < 512) && ((l >> 16) < 512)) {
				BufferDraw(l & 0x0000FFFF, (l >> 16));
				SolveModel(Buffer);
			}
			return 0;
// KEYDOWN EVENT
		case (WM_KEYUP):
			if (w >= 0x30 && w <= 0x39) {
				sprintf(RealPath, FileNames[w - 0x30], NumberImages[w - 0x30]);
				DrawToText(RealPath);
				++NumberImages[w - 0x30];
			}
			else if (w == VK_RETURN) {
				StatusCode = LearningModel;
				return 0;
			}
			
			ClearBuffer();	
			for (unsigned int i = 0; i < 10; ++i) Layer2Container[i] = 0.f;
			return 0;
// WINDOW PAINT EVENT
		case (WM_PAINT):
			MDc = CreateCompatibleDC(Dc);
			HBm = CreateCompatibleBitmap(Dc, Bmi.biWidth, Bmi.biHeight);
			SelectObject(MDc, HBm);
			SelectObject(MDc, HFont);

			DrawBackground();
			DrawFromBuffer();

			for (unsigned int i = 0; i < 10; ++i) DrawRect(550, (Bmi.biHeight - i * 28 - 28), (unsigned int)(200.f * Layer2Container[i]), 28, ToRGB(100, 255, 100));

			SetDIBitsToDevice(MDc, 0, 0, PixRect.right, PixRect.bottom, 0, 0, 0, PixRect.bottom, PixBuffer, &Bmi, BI_RGB);

			if (StatusCode == IndexingFile) TextOutA(MDc, 0, 511, Status, sizeof(Status));
			else if (StatusCode == LearningModel) TextOutA(MDc, 0, 511, Learning, sizeof(Learning));
			else TextOutA(MDc, 0, 511, HelpInterface, sizeof(HelpInterface));

			for (unsigned int i = 0; i < 10; ++i) TextOutA(MDc, 518, i * 28, Indi[i], 3);

			BitBlt(Dc, 0, 0, Bmi.biWidth, Bmi.biHeight, MDc, 0, 0, SRCCOPY);
			DeleteDC(MDc);
			DeleteObject(HBm);

			if (StatusCode == LearningModel) {
				LearnEpocha();

				SolveModel(Buffer);

				StatusCode = 0;
			} else if (StatusCode == IndexingFile) {
				IndexFiles();

				StatusCode = 0;
			}

			return 0;
// OTHER MESSAGES
		default: 
			return DefWindowProcA(hWnd, Mess, w, l);
	}
}
// DRAW FROR BUFFER FUNCTION
static void __fastcall DrawFromBuffer() {
	for (unsigned int i = 0; i < min(BufferSizeY, Bmi.biHeight); ++i) {
		for (unsigned int j = 0; j < min(BufferSizeX, Bmi.biWidth); ++j) {
			PixBuffer[(Bmi.biHeight - i) * Bmi.biWidth + j] = Buffer[i * BufferSizeX + j] ? 0x00FFFFFF : 0;
		}
	}
}
// CLEAR BUFFER FUNCTION
static void __fastcall ClearBuffer() {
	for (unsigned int i = 0; i < BufferSizeX * BufferSizeY; ++i) Buffer[i] = 0;
	for (unsigned int i = 0; i < Svrtk3SizeX * Svrtk3SizeY; ++i) Svrtk3[i] = 0.f;
}
// DRAW ON BUFFER FUNCTION
static void __fastcall BufferDraw(int x,  int y) {
	for (int i = max(y - 10, 0); i < min(y + 10, 511); ++i) {
		int rad = sqrtf(100.f - (float)(y - i) * (float)(y - i));
		for (int j = max(x - rad, 0); j < min(x + rad, 511); ++j) {
			Buffer[i * BufferSizeX + j] = 1;
		}
	}
}
// SAVE BIFFER TO FILE FUNCTION
static void __fastcall DrawToText(const char* Path) {
	File = fopen(Path, "w");
	for (unsigned int i = 0; i < BufferSizeX * BufferSizeY; ++i) {
		fputc(Buffer[i] ? '1' : '0', File);
	}
	fclose(File);
}
// DRAW BACKGROUND FUNCTION
static void __fastcall DrawBackground() {
	for (unsigned int i = 0; i < Bmi.biHeight; ++i) {
		for (unsigned int j = BufferSizeX; j < Bmi.biWidth; ++j) PixBuffer[i * Bmi.biWidth + j] = 0x00FFFFFF;
	}
}
// DRAW RECT FUNCTION
static void __fastcall DrawRectFunc(unsigned int XSx, unsigned int y, unsigned int sy, DWORD Color) {
	unsigned int Sx = XSx >> 16;
	XSx &= 0x0000FFFF;

	for (unsigned int i = y; i < min(y + sy, Bmi.biHeight - 1); ++i) {
		for (unsigned int j = XSx; j < min(XSx + Sx, Bmi.biWidth - 1); ++j) PixBuffer[i * Bmi.biWidth + j] = Color;
	}
}
// INDEX FILES FUNCTION
static void __fastcall IndexFiles() {
	CreateDirectoryA(DirectoryName, NULL);
	for (unsigned int i = 0; i < 10; ++i) {
		CreateDirectoryA(Dir1Names[i], NULL);

		sprintf(RealPath, FileNames[i], 0);
		while (File = fopen(RealPath, "r")) {
			NumberImages[i] += 10;
			fclose(File);
			sprintf(RealPath, FileNames[i], NumberImages[i]);
		}
		NumberImages[i] -= 10;
		sprintf(RealPath, FileNames[i], NumberImages[i]);
		while (File = fopen(RealPath, "r")) {
			++NumberImages[i];
			fclose(File);
			sprintf(RealPath, FileNames[i], NumberImages[i]);
		}
	}
}