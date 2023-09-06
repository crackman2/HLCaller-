/*
 * WinOverlay.cpp
 *
 *  Created on: 3 Sept 2023
 *      Author: Jackson Smackson
 */

#include "WinOverlay.hpp"
#include "Player.hpp"
#include "ESP.hpp"
#include <windows.h>
#include <iostream>
#include <cmath>
#include <memory>

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	WinOverlay *overlayInstance = reinterpret_cast<WinOverlay*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (overlayInstance) {
		switch (msg) {

		case WM_ERASEBKGND: {
			// Create a brush with the desired background color (magenta)
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255)); // Magenta background color

			// Fill the background with the brush
			RECT rect;
			GetClientRect(hwnd, &rect);
			FillRect((HDC) wParam, &rect, hBrush);

			// Cleanup the brush
			DeleteObject(hBrush);

			//std::cout << "BKGND PAINT" << std::endl;
			return 1; // Tell Windows we've handled erasing the background
		}

		case WM_TIMER: {
			// Handle the timer event.
			// You can use wParam to identify the timer (e.g., if you have multiple timers).
			// Perform your periodic task here.
			// wParam contains the timer ID.
			if (wParam == 1) {
				// Your code here.
				HWND hl2HWND = FindWindowA("Valve001", 0);
				if (hl2HWND != 0) {
					RECT hl2ClientRect;
					GetClientRect(hl2HWND, &hl2ClientRect);
					overlayInstance->clientRect = hl2ClientRect;

					POINT screenPos = { hl2ClientRect.left, hl2ClientRect.top };
					ClientToScreen(hl2HWND, &screenPos);
					int width = hl2ClientRect.right;
					int height = hl2ClientRect.bottom;

					HWND hwndForeground = GetForegroundWindow();

					if (hwndForeground == hl2HWND) {
						SetWindowPos(hwnd, HWND_TOPMOST, screenPos.x, screenPos.y, width, height, 0);
						SetWindowLongA(hwnd, 0, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | SWP_NOMOVE | WS_EX_TOPMOST);
					} else {
						SetWindowPos(hwnd, HWND_BOTTOM, screenPos.x, screenPos.y, width, height, 0);
						SetWindowLongA(hwnd, 0, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | SWP_NOMOVE);
					}

				}
			}
		}
			break;
			// Handle other messages as needed...
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WinOverlay::overlayCreationThread(LPVOID lpParam) {
	WinOverlay *overlayInstance = static_cast<WinOverlay*>(lpParam);
	// Register the window class
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	//wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "WinOverlayClass";
	RegisterClass(&wc);

	// Create the window
	HWND hWnd = CreateWindowEx(
	WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | SWP_NOMOVE | WS_EX_TOPMOST, "WinOverlayClass", "Overlay Window",
	WS_POPUP,            //WS_OVERLAPPEDWINDOW  ,
			0, 0, 400, 300,  // Adjust width and height as needed
			NULL, NULL, GetModuleHandle(NULL), NULL);

	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lpParam));

	SetTimer(hWnd, 1, 300, NULL);

	// Make the window transparent
	SetLayeredWindowAttributes(hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	// Set the window position
//    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,0);

	// Show the window
	ShowWindow(hWnd, SW_SHOW);

	overlayInstance->hWndOverlay = hWnd;

	//MessageBoxA(0,(LPCSTR)"window created",(LPCSTR)"this totally worked",0);
	// Enter the message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(hWnd, 1);

	return 0;
}

WinOverlay::WinOverlay(Player *ply) {
	// Create a new thread for overlay creation
	hCreationThread = CreateThread(NULL, 0, overlayCreationThread, this, 0, NULL);
	hRendererThread = CreateThread(NULL, 0, overlayRendererThreadGDI, this, 0, NULL);

	this->ply = ply;

	if (hCreationThread == NULL) {
		// Handle thread creation error
	}

	if (hRendererThread == NULL) {
		// Handle thread creation error
	}
}

WinOverlay::~WinOverlay() {
	if (hCreationThread != NULL) {
		// Close the thread handle (important to avoid resource leaks)
		CloseHandle(hCreationThread);
	}

	if (hRendererThread != NULL) {
		// Close the thread handle (important to avoid resource leaks)
		CloseHandle(hRendererThread);
	}
}

DWORD WINAPI WinOverlay::overlayRendererThreadGDI(LPVOID lpParam) {
	WinOverlay *overlayInstance = static_cast<WinOverlay*>(lpParam);

	//std::cout << "WinOverlay: renderer thread waiting for hwnd to be defined" << std::endl;
	while (!(overlayInstance->hWndOverlay)) {
		Sleep(1000);
	}

	//std::cout << "WinOverlay: hwnd found" << std::endl;


	long int *width = &overlayInstance->clientRect.right;
	long int *height = &overlayInstance->clientRect.bottom;

	HDC hdc = GetDC(overlayInstance->hWndOverlay);
	HDC hdcBackBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, *width, *height);

	SelectObject(hdcBackBuffer, hBitmap);

	float angle = 0.0; // Initialize the angle
	float increment = 0.1; // Angle increment
	int length = 100; // Length of the line
	int centerX = *width  / 2; // Replace 'width' and 'height' with your window dimensions
	int centerY = *height / 2;

	int endX = 0;
	int endY = 0;

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0)); // Green pen
	HBRUSH hClearBrush = CreateSolidBrush(RGB(255, 0, 255));

	RECT clearRect = { 0, 0, *width, *height };


	overlayInstance->esp = std::make_unique<ESP>(hdcBackBuffer, &overlayInstance->clientRect, overlayInstance->ply);

	//std::cout << "WinOverlay: drawing loop" << std::endl;
	while (true) {
		SelectObject(hdcBackBuffer, hClearBrush);
		FillRect(hdcBackBuffer, &clearRect, hClearBrush);
		if(overlayInstance->enableOverlay){
			overlayInstance->ply->GetVelocity();
			length = sqrtf(
					powf(overlayInstance->ply->vel.x,2) +
					powf(overlayInstance->ply->vel.y,2)
			)/10;
			centerX = *width  / 2; // Replace 'width' and 'height' with your window dimensions
			centerY = *height / 2;
			angle += increment; // Update the angle
			endX = centerX + static_cast<int>(length * cos(angle));
			endY = centerY + static_cast<int>(length * sin(angle));


			SelectObject(hdcBackBuffer, hPen);
			MoveToEx(hdcBackBuffer, centerX, centerY, NULL);
			LineTo(hdcBackBuffer, endX, endY);



			overlayInstance->esp->Draw();
		}
		BitBlt(hdc, 0, 0, *width, *height, hdcBackBuffer, 0, 0, SRCCOPY);
		Sleep(5);
	}

	DeleteObject(hPen);
	ReleaseDC(overlayInstance->hWndOverlay, hdc);
	return 0;
}
