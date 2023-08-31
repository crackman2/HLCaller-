/*
 * mainF.cpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */
#include "mainF.hpp"
#include "Player.hpp"
#include "Console.hpp"
#include <windows.h>
#include <stdio.h>
#include <stdint.h>


DWORD WINAPI fmcConsoleMainThread(LPVOID lpParam) {
	Player ply;
	ConsoleCMD CMD(&ply);
	Console con("HLCaller++", &ply, &CMD);


	while (con.InputLoop()) {}

	con.~Console();

	return 0;
}

