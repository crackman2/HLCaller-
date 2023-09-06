/*
 * Player.cpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */

#include "Player.hpp"
#include <windows.h>
#include <iostream>
#include <sstream>

using namespace std;

Player::Player() {
	Init();
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

void Player::Init() {
	Initialized = false;
	ServerDLLBase = GetModuleHandleA("server.dll");
	EngineDLLBase = GetModuleHandleA("engine.dll");
	ClientDLLBase = GetModuleHandleA("client.dll");

	if ((ServerDLLBase != NULL) and (ClientDLLBase != NULL) and (EngineDLLBase != NULL)) {
		if (IsInGame()) {
			try {
				LocalPlayer = *(uint32_t*) ((uint32_t) ServerDLLBase + offset_LocalPlayer);
				pos = { 0, 0, 0 };
				vel = { 0, 0, 0 };
				cam = { 0, 0 };
				camHeight = (float*)((uint32_t)ClientDLLBase + offset_cameraHeight);
				maxVel = 0;

				OnGroundFuck = (*(uint32_t*) ((uint32_t) ServerDLLBase + offset_ongroundFUCK)) + offset_ongroundFUCK_bytes;

				Initialized = true;

				if (wep == nullptr)
					wep = std::make_unique<Weapon>(&ServerDLLBase, &Initialized);



			} catch (exception &e) {
				cout << "plyErr: access violation, initialization failed" << endl;
				cout << "E: " << e.what() << endl;
			}
		}

	} else {
		cout << "plyErr: failed to get handle to server.dll" << endl;
	}
}

void Player::GetPosition() {
	if (Initialized and !IsLoading()) {
		try {
			pos.x = *(float*) (LocalPlayer + offset_posx);
			pos.y = *(float*) (LocalPlayer + offset_posy);
			pos.z = *(float*) (LocalPlayer + offset_posz);
		} catch (const exception &e) {
			cout << "plyErr: access violation in: GetPosition. getting fresh pointers" << endl;
			cout << "E: " << e.what() << endl;
			Init();
		}
	} else {
		PrintPointerError();
	}
}

void Player::GetVelocity() {
	if (Initialized and !IsLoading()) {
		try {
			vel.x = *(float*) (LocalPlayer + offset_velx);
			vel.y = *(float*) (LocalPlayer + offset_vely);
			vel.z = *(float*) (LocalPlayer + offset_velz);
		} catch (const exception &e) {
			cout << "plyErr: access violation in: GetPosition. getting fresh pointers" << endl;
			cout << "E: " << e.what() << endl;
			Init();
		}
	} else {
		//cout << "plyErr: player not initialized. getting fresh pointers"
		//		<< endl;
		//Init();
		PrintPointerError();
	}
}

void Player::GetPlayerData(char *buffer1024) {
	GetPosition();
	GetVelocity();

	char strPlayerData[1024];

	if (Initialized and !IsLoading()) {
		sprintf(strPlayerData, " --- Player Data ---\n"
				"  addposx: %X\n"
				"  addposy: %X\n"
				"  addposz: %X\n"
				"  addvelx: %X\n"
				"  addvely: %X\n"
				"  addvelz: %X\n"
				" -------------------\n"
				"  posx: %.1f\n"
				"  posy: %.1f\n"
				"  posz: %.1f\n"
				"  velx: %.1f\n"
				"  vely: %.1f\n"
				"  velz: %.1f\n",
				(uint32_t) (LocalPlayer + offset_posx),
				(uint32_t) (LocalPlayer + offset_posy),
				(uint32_t) (LocalPlayer + offset_posz),
				(uint32_t) (LocalPlayer + offset_velx),
				(uint32_t) (LocalPlayer + offset_vely),
				(uint32_t) (LocalPlayer + offset_velz),
				pos.x, pos.y, pos.z,
				vel.x, vel.y, vel.z);

		strcpy(buffer1024, strPlayerData);
	} else {
		stringstream s;
		s << " -- something went terribly wrong, all hope is lost" << endl;
		strcpy(buffer1024, s.str().c_str());
		PrintPointerError();
	}
}

void Player::SetVelocity(uint8_t dir) {
	if ((mask_X bitand dir) != 0)
		*(float*) (LocalPlayer + offset_velx) = vel.x;
	if ((mask_Y bitand dir) != 0)
		*(float*) (LocalPlayer + offset_vely) = vel.y;
	if ((mask_Z bitand dir) != 0)
		*(float*) (LocalPlayer + offset_velz) = vel.z;
}

void Player::SetPosition(uint8_t axis) {
	if ((mask_X bitand axis) != 0)
		*(float*) (LocalPlayer + offset_posx) = pos.x;
	if ((mask_Y bitand axis) != 0)
		*(float*) (LocalPlayer + offset_posy) = pos.y;
	if ((mask_Z bitand axis) != 0)
		*(float*) (LocalPlayer + offset_posz) = pos.z;
}

/*
 * useless, mostly
 * prevent access violation
 */
void Player::GetInGameState() {
	uint8_t result = false;
	if (EngineDLLBase != 0) {
		result = *(uint8_t*) ((uint32_t) EngineDLLBase + offset_ingame);
	}

	if (result == 0) {
		Initialized = false;
	}
}

bool Player::IsOnGroundR() {
	if (Initialized and !IsLoading()) {
		onGround = (bool) (*(uint8_t*) ((uint32_t) ServerDLLBase + offset_ongroundR));
		return onGround;
	} else {
		PrintPointerError();
	}
	return false;
}

bool Player::IsOnGroundW() {
	if (Initialized and !IsLoading()) {
		return (bool) (*(uint8_t*) ((uint32_t) ServerDLLBase + offset_ongroundW));
	} else
		PrintPointerError();
	return false;
}

/*
 * more reliable way to test OnGround, preferred
 */
bool Player::IsOnGroundFuck() {
	if (Initialized and !IsLoading()) {
		uint8_t tmp = 0;
		tmp = *(uint8_t*) (OnGroundFuck);
		//cout << "OnGround: " << hex << OnGroundFuck << endl;
		//cout << "tmp: " << dec << tmp << endl;
		return (tmp != 0);
	} else
		PrintPointerError();

	return false;
}

/*
 * not really keys, but movement buttons (+forward, +duck, etc etc)
 */
bool Player::IsKeyPressed(uint16_t key) {
	if (Initialized and !IsLoading()) {
		jump = (((*(uint16_t*) ((uint32_t) ClientDLLBase + offset_keyswitch)) bitand key) != 0);
		return jump;
	} else {
		PrintPointerError();
	}
	return false;
}

/*
 * is actually also just the "+jump" key thing but with a differnt value
 */
bool Player::IsSpaceKeyPressed() {
	if (Initialized and !IsLoading()) {
		uint8_t result = *(uint8_t*) ((uint32_t) ClientDLLBase + offset_spacekey);
		return (result == 65);
	} else {
		PrintPointerError();
	}
	return false;
}

void Player::SetOnGround(uint8_t value) {
	if (Initialized and !IsLoading()) {
		*(uint8_t*) ((uint32_t) ServerDLLBase + offset_ongroundW) = value;
	} else {
		PrintPointerError();
	}
}

/*
 * just for research
 */
void Player::MaxVelUpdate() {
	if (Initialized and !IsLoading()) {
		if (vel.z > maxVel)
			maxVel = vel.z;
	} else {
		PrintPointerError();
	}
}

/*
 * just for research
 */
void Player::MaxVelReset() {
	if (Initialized and !IsLoading()) {
		maxVel = 0;
	} else {
		PrintPointerError();
	}
}

/*
 * just for research
 */
float Player::MaxVelReturn() {
	if (Initialized and !IsLoading()) {
		return maxVel;
	} else {
		PrintPointerError();
		return 0;
	}
}

void Player::SetJumpState(uint8_t value) {
	if (Initialized and !IsLoading()) {
		*(uint8_t*) ((uint32_t) ClientDLLBase + offset_jumpstate) = value;
	}
}

void Player::PollBhop() {
	if (Initialized and !IsLoading()) {
		if (IsSpaceKeyPressed() and !jumpThread) {
			jumpThread = true;
			SetJumpState(5); //jump
			DWORD threadid = 0;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) (&Player::BhopThread), this, 0, &threadid);
		}

	} else {
		PrintPointerError();
	}
}

void Player::SetResetJumpKey() {
	uint16_t keyswitch = *(uint16_t*) ((uint32_t) ServerDLLBase + offset_keyswitchsrv);
	uint16_t mask = ~(1 << 1); //11111111 11111101
	uint16_t result = keyswitch bitand mask;
	*(uint16_t*) ((uint32_t) ServerDLLBase + offset_keyswitchsrv) = result;
	*(uint16_t*) ((uint32_t) ServerDLLBase + offset_keyswitchsrv2) = result;
	*(uint16_t*) ((uint32_t) ClientDLLBase + offset_keyswitch) = result;
	*(uint16_t*) ((uint32_t) ClientDLLBase + offset_keyswitch2) = result;
}


/*
 * prevent access violation
 */
bool Player::IsInGame() {
	uint8_t result = 0;
	if (EngineDLLBase != 0) {
		result = *(uint8_t*) ((uint32_t) EngineDLLBase + offset_ingame);
	}

	return (bool) result;
}

void Player::DisableSpeedLimit(bool state) {
	if (Initialized and !IsLoading()) {
		if (state) {
			DWORD old = 0;
			VirtualProtect((LPVOID) ((uint32_t) ServerDLLBase + offset_speedlimitX), sizeof(offset_speedlimitX),
			PAGE_EXECUTE_READWRITE, &old);
			for (uint32_t i = 0; i < sizeof(bytes_speedlimitX); i++) {
				*(uint8_t*) ((uint32_t) ServerDLLBase + offset_speedlimitX + i) = 0x90;
				*(uint8_t*) ((uint32_t) ServerDLLBase + offset_speedlimitY + i) = 0x90;
			}
			disableSpeedlimit = true;
		} else {
			for (uint32_t i = 0; i < sizeof(bytes_speedlimitX); i++) {
				*(uint8_t*) ((uint32_t) ServerDLLBase + offset_speedlimitX + i) = bytes_speedlimitX[i];
				*(uint8_t*) ((uint32_t) ServerDLLBase + offset_speedlimitY + i) = bytes_speedlimitY[i];
			}
			disableSpeedlimit = false;
		}
	} else {
		PrintPointerError();
	}
}

void Player::SetAirAccelerate(float value) {
	if (Initialized and !IsLoading()) {
		*(float*) ((uint32_t) ServerDLLBase + offset_cvar_airacc) = value;
	} else {
		PrintPointerError();
	}
}

void Player::BhopBoost() {
	if (Initialized and !IsLoading()) {
		GetVelocity();
		vel.x *= bhopBoost;
		vel.y *= bhopBoost;
		SetVelocity(mask_X bitor mask_Y);
	} else {
		PrintPointerError();
	}
}

/*
 * prevent access violation
 */
bool Player::IsLoading() {
	uint8_t result = 0;
	if (ClientDLLBase != 0) {
		result = *(uint8_t*) ((uint32_t) ClientDLLBase + offset_loading1);
	}
	bool tmp = (bool) result;
	if (tmp) {
		Initialized = false;
		loading = true;
	}

	return tmp;
}

void Player::PrintPointerError() {
	if (!IsLoading()) {
		cout << "plyErr: pointer invalid. getting fresh pointers" << endl;
		Init();
	}
}

void Player::BhopThread(void *lpParameter) {
	Player *playerInstance = static_cast<Player*>(lpParameter);
	Sleep(20);
	playerInstance->BhopBoost();
	while (!playerInstance->IsOnGroundFuck()) {
		playerInstance->SetJumpState(4);
	}
	playerInstance->SetJumpState(4); //stand
	playerInstance->jumpThread = false;
}


