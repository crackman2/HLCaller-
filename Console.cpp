/*
 * Console.cpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */

#include "Console.hpp"
#include "ConsoleCMD.hpp"
#include "Player.hpp"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <map>
#include <functional>
#include <stdio.h>

using namespace std;

Console::Console(const char *Title, Player *ply, ConsoleCMD *CMD) {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	char WinTitle[512];
	sprintf(WinTitle, "title %s", Title);
	system(WinTitle);

	this->CMD = CMD;
	this->CMD->Msg("msg \"ConsoleInit finished\"");

	this->ply = ply;
	cout << "* Console Init *" << endl;
}

Console::~Console() {
	FreeConsole();
}

bool Console::InputLoop() {
    std::string input;

    std::cout << "> ";
    std::getline(std::cin, input);

    //std::cout << "cmd: " << input << std::endl;

    // Remove newline character from input
    if (!input.empty() && input.back() == '\n') {
        input.pop_back();
    }

    // Tokenize input
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    if (tokens.empty()) {
        return true; // No input, continue loop
    }

    std::string command = tokens[0]; // First token is the command

    // Command lookup map using lambda functions
    std::map<std::string, std::function<void()>> commandMap = {
        { "exit", [this]() {
            CMD->Exit();
        } },
        { "clear", [this]() {
            CMD->Clear();
        } },
        { "help", [this]() {
            CMD->ShowHelp();
        } },
        { "velrecord", [this]() {
            CMD->PrintMaxVel();
        } },
        { "resetvelrecord", [this]() {
            CMD->ResetMaxVel();
        } },
        { "plydata", [this]() {
            CMD->PlayerData();
        } },
        { "plyreinit", [this]() {
            CMD->ReInitPlayer();
        } },
        { "speedlimit", [this]() {
            CMD->Speedlimit();
        } },
        { "update", [this]() {
            ActivateUpdateThread();
        } },
        { "air", [this, &input]() {
            // Find the position of the space character after the command name
            size_t spacePos = input.find(" ");
            if (spacePos != std::string::npos) {
                // Extract the argument string after the space
                std::string argStr = input.substr(spacePos + 1);

                // Convert the argument string to a float
                float argValue = std::stof(argStr);

                // Call the AirAccelerate(float) function from CMD
                CMD->AirAccelerate(argValue);
            } else {
                std::cout << " -- invalid argument for air" << std::endl;
            }
        } },
        { "bhopboost", [this, &input]() {
            // Find the position of the space character after the command name
            size_t spacePos = input.find(" ");
            if (spacePos != std::string::npos) {
                // Extract the argument string after the space
                std::string argStr = input.substr(spacePos + 1);

                // Convert the argument string to a float
                float argValue = std::stof(argStr);

                // Call the AirAccelerate(float) function from CMD
                CMD->BhopBoost(argValue);
            } else {
                std::cout << " -- invalid argument for bhopboost" << std::endl;
            }
        } }
        // Add more commands here
    };

    // Search for the command in the map
    auto commandIt = commandMap.find(command);
    if (commandIt != commandMap.end()) {
        commandIt->second(); // Call the associated function
    } else if (input.size() > 5 && input.substr(0, 5) == "msg \"" && input.back() == '"') {
        CMD->Msg(input.c_str());
    } else {
        std::cout << " -- unknown command " << command << std::endl;
    }

    return true;
}


void Console::ActivateUpdateThread() {
	if (UpdateThreadEnabled) {
		TerminateThread(UpdateThreadHandle, 0);
		CloseHandle(UpdateThreadHandle);
		UpdateThreadHandle = 0;
		UpdateThreadEnabled = false;
		cout << " -- update thread terminated" << endl;
	} else {
		if (UpdateThreadHandle != 0) {
			TerminateThread(UpdateThreadHandle, 0);
			CloseHandle(UpdateThreadHandle);
			UpdateThreadHandle = 0;
			UpdateThreadEnabled = false;
		}

		DWORD threadid = 0;
		UpdateThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) (&Console::UpdateThreadFunction), this, 0,
				&threadid);
		UpdateThreadEnabled = true;
		cout << " -- update thread started (bhop)" << endl;
	}
}

void Console::UpdateThreadFunction(void *lpParameter) {
	Console *consoleInstance = static_cast<Console*>(lpParameter);
	bool ongroundc = consoleInstance->ply->onground;
	bool jumppressed = consoleInstance->ply->jump;

	while (true) {
		consoleInstance->ply->GetInGameState();
		if (consoleInstance->ply->Initialized and !consoleInstance->ply->IsLoading()) {
			consoleInstance->ply->GetVelocity();
			consoleInstance->ply->MaxVelUpdate();
			consoleInstance->ply->PollBhop();

			if (ongroundc != consoleInstance->ply->onground) {
				if (ongroundc) {
					CMD->hl2Msg("OnGround 0\n");
				} else {
					CMD->hl2Msg("OnGround 1\n");
				}
				ongroundc = consoleInstance->ply->onground;
			}

			if (jumppressed != consoleInstance->ply->jump) {
				if (jumppressed) {
					CMD->hl2Msg("Jump     0\n");
				} else {
					CMD->hl2Msg("Jump     1\n");
				}
				jumppressed = consoleInstance->ply->jump;
			}
		} else {
			Sleep(200);
		}

		if(!consoleInstance->ply->IsLoading() and consoleInstance->ply->loading){
			consoleInstance->ply->Init();
			if(consoleInstance->ply->Initialized)
				consoleInstance->ply->loading = false;
		}
	}
}
