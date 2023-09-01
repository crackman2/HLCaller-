/*
 * Weapon.h
 *
 *  Created on: 1 Sept 2023
 *      Author: Jackson Smackson
 */

#ifndef WEAPON_HPP_
#define WEAPON_HPP_
#include <windows.h>
#include <cstdint>

class Weapon {
public:
	bool *Initialized = nullptr;

	bool InfiniteAmmoEnabled = false;


	HANDLE *ServerDLLBase;

	Weapon(HANDLE *ServerDLLBase, bool *Initialized);

	void InfiniteAmmo(bool state);

	virtual ~Weapon();
private:
	/**
	 * Infinite Ammo
	 *
	 * server.dll+E73FA - 89 3E                  - mov [esi],edi			most Guns
	 * server.dll+F0BC4 - 89 1E                  - mov [esi],ebx 			most AltFire (Handgrenade, RPG too)
	 * server.dll+3C0349 - 89 1E                 - mov [esi],ebx        	Magnum, XBow
	 * server.dll+3D9D6B - 89 83 AC040000        - mov [ebx+000004AC],eax	Shotgun
	 * server.dll+3DA331 - 89 87 AC040000        - mov [edi+000004AC],eax   Shotgun Alt
	 * server.dll+F353A - 89 3E                  - mov [esi],edi			XBow Reload
	 *
	 */
	const uint32_t offset_infAmmo_mostPrimary              =                              0xE73FA;
	const uint8_t  opcode_infAmmo_mostPrimary[2]           =                       { 0x89, 0x3E };

	const uint32_t offset_infAmmo_mostAltfire              =                              0xF0BC4;
	const uint8_t  opcode_infAmmo_mostAltfire[2]           =                       { 0x89, 0x1E };

	const uint32_t offset_infAmmo_specialGuns              =                             0x3C0349;
	const uint8_t  opcode_infAmmo_specialGuns[2]           =                       { 0x89, 0x1E };

	const uint32_t offset_infAmmo_shotgun                  =                             0x3D9D6B;
	const uint8_t  opcode_infAmmo_shotgun[6]               = { 0x89, 0x83, 0xAC, 0x04, 0x0, 0x0 };
	const uint32_t offset_infAmmo_shotgunAlt               =                             0x3DA331;
	const uint8_t  opcode_infAmmo_shotgunAlt[6]            = { 0x89, 0x87, 0xAC, 0x04, 0x0, 0x0 };

	const uint32_t offset_infAmmo_xbowReload               =                              0xF353A;
	const uint8_t  opcode_infAmmo_xbowReload[2]            =                       { 0x89, 0x3E };

	void KillOpCodes(const uint32_t offset,const uint8_t *opcodes, size_t size, bool NOP);
};

#endif /* WEAPON_HPP_ */
