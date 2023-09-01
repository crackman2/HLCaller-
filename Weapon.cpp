/*
 * Weapon.cpp
 *
 *  Created on: 1 Sept 2023
 *      Author: Jackson Smackson
 */

#include "Weapon.hpp"
#include <windows.h>

Weapon::Weapon(HANDLE *ServerDLLBase, bool *Initialized) {
	if(Initialized != nullptr){
		this->Initialized = Initialized;
	}

	if(ServerDLLBase != nullptr){
		this->ServerDLLBase = ServerDLLBase;
	}
}



Weapon::~Weapon() {
	// TODO Auto-generated destructor stub
}


void Weapon::InfiniteAmmo(bool state) {
	if(*Initialized){
		if(state){
			KillOpCodes(offset_infAmmo_mostPrimary,opcode_infAmmo_mostPrimary,sizeof(opcode_infAmmo_mostPrimary),true);
			KillOpCodes(offset_infAmmo_mostAltfire,opcode_infAmmo_mostAltfire,sizeof(opcode_infAmmo_mostAltfire),true);
			KillOpCodes(offset_infAmmo_specialGuns,opcode_infAmmo_specialGuns,sizeof(opcode_infAmmo_specialGuns),true);
			KillOpCodes(offset_infAmmo_shotgun,    opcode_infAmmo_shotgun,    sizeof(opcode_infAmmo_shotgun),    true);
			KillOpCodes(offset_infAmmo_shotgunAlt, opcode_infAmmo_shotgunAlt, sizeof(opcode_infAmmo_shotgunAlt), true);
			KillOpCodes(offset_infAmmo_xbowReload, opcode_infAmmo_xbowReload, sizeof(opcode_infAmmo_xbowReload), true);
			InfiniteAmmoEnabled = true;

		} else {
			KillOpCodes(offset_infAmmo_mostPrimary,opcode_infAmmo_mostPrimary,sizeof(opcode_infAmmo_mostPrimary),false);
			KillOpCodes(offset_infAmmo_mostAltfire,opcode_infAmmo_mostAltfire,sizeof(opcode_infAmmo_mostAltfire),false);
			KillOpCodes(offset_infAmmo_specialGuns,opcode_infAmmo_specialGuns,sizeof(opcode_infAmmo_specialGuns),false);
			KillOpCodes(offset_infAmmo_shotgun,    opcode_infAmmo_shotgun,    sizeof(opcode_infAmmo_shotgun),    false);
			KillOpCodes(offset_infAmmo_shotgunAlt, opcode_infAmmo_shotgunAlt, sizeof(opcode_infAmmo_shotgunAlt), false);
			KillOpCodes(offset_infAmmo_xbowReload, opcode_infAmmo_xbowReload, sizeof(opcode_infAmmo_xbowReload), false);
			InfiniteAmmoEnabled = false;
		}
	}
}


/**
 *  PRIVATE
 */

void Weapon::KillOpCodes(const uint32_t offset,const uint8_t *opcodes, size_t size, bool NOP) {
	DWORD old;
	VirtualProtect((LPVOID)((uint32_t)(*ServerDLLBase) + offset),size,PAGE_EXECUTE_READWRITE,&old);
	if(NOP or (opcodes == nullptr)){
		for(uint32_t i = 0; i < size; i++){
			*(uint8_t*)((uint32_t)(*ServerDLLBase) + offset + i) = 0x90;
		}
	} else {
		for(uint32_t i = 0; i < size; i++){
			*(uint8_t*)((uint32_t)(*ServerDLLBase) + offset + i) = opcodes[i];
		}
	}
}
