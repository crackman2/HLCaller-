/*
 * mainF.cpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */
#include "mainF.hpp"
#include "Player.hpp"
#include "Console.hpp"
#include "WinOverlay.hpp"
#include <windows.h>


DWORD WINAPI fmcConsoleMainThread(LPVOID lpParam) {
	Player ply;
	WinOverlay win(&ply);
	ConsoleCMD CMD(&ply,&win);
	Console con("HLCaller++", &ply, &CMD);



	while (con.InputLoop()) {}

	con.~Console();

	return 0;
}

