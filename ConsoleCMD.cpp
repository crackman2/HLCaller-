/*
 * ConsoleCMD.cpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */

#include "ConsoleCMD.hpp"
#include "Player.hpp"
#include "WinOverlay.hpp"
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>

using namespace std;

typedef void (*fMsg)(const char*, ...);

ConsoleCMD::ConsoleCMD(Player *ply, WinOverlay *win) {
	this->ply = ply;
	this->win = win;
}

ConsoleCMD::~ConsoleCMD() {
	// TODO Auto-generated destructor stub
}

void ConsoleCMD::Clear() {
	system("cls");
}

void ConsoleCMD::ShowHelp() {
	printboth(" ------------------------------- commands ------------------------------- \n"
			"   exit                        exits console\n"
			"   clear                       clear screen\n"
			"   msg \"<some message>\"        prints text to ingame console\n"
			"   velrecord                   displays the current highest velz\n"
			"   resetvelrecord              resets the current highest velz\n"
			"   plydata                     display player data\n"
			"   plyreinit                   reinitialize the player\n"
			"   update                      enables/disables update function (bhop)\n"
			"   speedlimit                  disables/enables speedlimit (breaks ABH)\n"
			"   air <value>                 set sv_airaccelerate, like the good old days\n"
			"   bhopboost <value>           default is 1 (no boost)\n"
			"   infammo                     enables infinite ammo\n"
			"   pistolspray                 pistol fires all ammo at once (lag)\n"
			"   bhop                        enables default bhop mode\n"
			"   overlay                     enables overlay (ESP)\n"
			"   tp <id>                     teleport to entity in entityList\n"
	);
}

void ConsoleCMD::Msg(const char *input) {
	std::string inputStr = input;
	size_t startPos = inputStr.find('"');
	size_t endPos = inputStr.find('"', startPos + 1);

	if (startPos != std::string::npos && endPos != std::string::npos) {
		std::string message = inputStr.substr(startPos + 1, endPos - startPos - 1);
		message += '\n';
		hl2Msg(message.c_str());
	}
}

void ConsoleCMD::PlayerData() {
	char strPlayerData[1024];
	ply->GetPlayerData(strPlayerData);
	printf(strPlayerData);
}

void ConsoleCMD::ReInitPlayer() {
	ply->Init();
	if (ply->Initialized)
		printboth(" -- player initialized successfully\n");
	else
		printboth(" -- player initialization failed\n");
}

void ConsoleCMD::PrintMaxVel() {
	stringstream s;
	s << "CMD: max velocity on Z axis: " << setprecision(2) << fixed << ply->maxVel << endl;
	printboth(s.str().c_str());
}

void ConsoleCMD::ResetMaxVel() {
	ply->maxVel = 0;
}

void ConsoleCMD::DefaultBhop() {
	ply->SetAirAccelerate(ply->defaultBhop_airaccelerate);
	ply->bhopBoost = ply->defaultBhop_boost;
	stringstream s;
	s << "] sv_airaccelerate " << ply->defaultBhop_airaccelerate << endl;
	hl2Msg(s.str().c_str());
	cout << " -- sv_airaccelerate set to " << ply->defaultBhop_airaccelerate << endl;
	cout << " -- default bhop enabled" << endl;
	hl2Msg(" -- default bhop enabled\n");
}

void ConsoleCMD::printboth(const char *msg) {
	cout << msg;
	stringstream extmsg;
	extmsg << " ..........::; ( HLCaller ) ;::........." << endl;
	extmsg << msg << "\0";
	hl2Msg((char*) (extmsg.str().c_str()));
}

void ConsoleCMD::hl2Msg(const char *Message) {
	HMODULE hModule = GetModuleHandleA("tier0.dll");

	if (hModule != NULL) {
		fMsg Msg = (fMsg) GetProcAddress(hModule, "Msg");

		if (Msg != NULL) {
			Msg(Message);
		} else {
			printf(" -- Msg function could not be found\n");
		}
	} else {
		printf(" -- tier0.dll could not be loaded\n");
	}
}

void ConsoleCMD::Exit() {
	ExitProcess(0);
}

void ConsoleCMD::Speedlimit() {
	if (ply->disableSpeedlimit) {
		ply->DisableSpeedLimit(false);
		printboth(" -- speedlimit enabled\n");
	} else {
		ply->DisableSpeedLimit(true);
		printboth(" -- speedlimit disabled\n");
	}
}

void ConsoleCMD::AirAccelerate(float value) {
	ply->SetAirAccelerate(value);
	stringstream s;
	s << "] sv_airaccelerate " << value << endl;
	hl2Msg(s.str().c_str());
	cout << " -- sv_airaccelerate set to " << value << endl;
}

void ConsoleCMD::BhopBoost(float value) {
	ply->bhopBoost = value;
	stringstream s;
	s << " -- bhopboost set to " << value << endl;
	printboth(s.str().c_str());
}

void ConsoleCMD::InfAmmo() {
	ply->wep->InfiniteAmmo(!(ply->wep->InfiniteAmmoEnabled));
	if (ply->wep->InfiniteAmmoEnabled) {
		printboth(" -- infinite ammo enabled\n");
	} else {
		printboth(" -- infinite ammo disabled\n");
	}
}

void ConsoleCMD::SprayShot() {
	ply->wep->SprayShot(!(ply->wep->SprayShotEnabled));
	if (ply->wep->SprayShotEnabled) {
		printboth(" -- pistolspray enabled\n");
	} else {
		printboth(" -- pistolspray disabled\n");
	}
}

void ConsoleCMD::Overlay() {
	if(win->enableOverlay){
		win->enableOverlay = false;
		printboth(" -- overlay disabled\n");
	} else {
		win->enableOverlay = true;
		printboth(" -- overlay enabled\n");
	}
}

void ConsoleCMD::Teleport(uint32_t id) {
	win->esp->InitEntities();
	win->esp->ply->pos.x = *win->esp->entityList[id].pos.x;
	win->esp->ply->pos.y = *win->esp->entityList[id].pos.y;
	win->esp->ply->pos.z = *win->esp->entityList[id].pos.z;
	win->esp->ply->SetPosition(win->esp->ply->mask_X | win->esp->ply->mask_Y | win->esp->ply->mask_Z);

	std::stringstream ss;

	ss << " -- teleported to id " << std::dec << id << " name: " << win->esp->entityList[id].name() << "\n";

	printboth(ss.str().c_str());
}
