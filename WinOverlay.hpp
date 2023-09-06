/*
 * WinOverlay.hpp
 *
 *  Created on: 3 Sept 2023
 *      Author: Jackson Smackson
 */

#ifndef WINOVERLAY_HPP_
#define WINOVERLAY_HPP_
#include <windows.h>
#include <memory>
#include "Player.hpp"
#include "ESP.hpp"


static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WinOverlay{
public:
	bool enableOverlay = false;

	HWND hWndOverlay = 0;

	Player *ply;

	std::unique_ptr<ESP> esp;


	RECT clientRect;



    WinOverlay(Player *ply);
    ~WinOverlay();

private:
    HANDLE hCreationThread;
    HANDLE hRendererThread;
    static DWORD WINAPI overlayCreationThread(LPVOID lpParam);
    static DWORD WINAPI overlayRendererThreadGDI(LPVOID lpParam);
    static DWORD WINAPI overlayRendererThreadDirectDraw(LPVOID lpParam);

};

#endif /* WINOVERLAY_HPP_ */
