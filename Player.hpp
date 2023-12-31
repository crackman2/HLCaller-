/*
 * Player.hpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */

#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include "Weapon.hpp"
#include <cstdint>
#include <memory>
#include <windows.h>

typedef struct {
	float x,y,z;
}vec3f;

typedef struct{
	float x,y;
}vec2f;


class Player {
public:
	vec3f pos;
	vec3f vel;
	vec2f cam;
	float* camHeight;


	float maxVel;
	float bhopBoost = 1;

	float defaultBhop_boost = 1.1;
	float defaultBhop_airaccelerate = 9999.0;

	bool  Initialized;

	bool  onGround;
	bool  jump;
	bool  jumpThread;
	bool  disableSpeedlimit = false;
	bool  loading = false;

	bool enableOverlay = false;

	std::unique_ptr<Weapon> wep;

	/*
	 * SetVelocity directions
	 */
	const uint8_t mask_X               =            0x1;
	const uint8_t mask_Y               =            0x2;
	const uint8_t mask_Z               =            0x4;

	Player();
	virtual ~Player();

	void  Init();

	void  GetPosition();
	void  GetVelocity();
	void  GetInGameState();
	void  GetPlayerData(char* buffer1024);


	void  SetVelocity(uint8_t dir);
	void  SetPosition(uint8_t axis);
	void  SetOnGround(uint8_t value);
	void  SetJumpState(uint8_t value);
	void  SetResetJumpKey();
	void  SetAirAccelerate(float value);


	bool  IsInGame();
	bool  IsKeyPressed(uint16_t key);
	bool  IsSpaceKeyPressed();
	bool  IsOnGroundR();
	bool  IsOnGroundW();
	bool  IsOnGroundFuck();
	bool  IsLoading();


	void  PollBhop();
	void  DisableSpeedLimit(bool state);
	void  BhopBoost();

	/*
	 * Functions for research
	 */
	void  MaxVelUpdate();
	void  MaxVelReset();
	float MaxVelReturn();

private:
	HANDLE   ServerDLLBase             =              0;
	HANDLE   EngineDLLBase             =              0;
	HANDLE   ClientDLLBase             =              0;

	uint32_t LocalPlayer               =              0;
	static const uint32_t offset_LocalPlayer  =       0x68B8DC;

	uint32_t OnGroundFuck              =              0;


	/*
	 * Localplayer + <offset for something> -> <value of that something>
	 */
	static const uint32_t offset_posx         =          0x27C;
	static const uint32_t offset_posy         =          0x280;
	static const uint32_t offset_posz         =          0x284;

	static const uint32_t offset_velx	       =          0x214;
	static const uint32_t offset_vely	       =          0x218;
	static const uint32_t offset_velz	       =          0x21C; //jump is 146.5

	/*
	 * engine.dll+3BA204 -> 1 if ingame 0 if disconnected
	 */
	static const uint32_t offset_ingame       =       0x3BA204;

	/*
	 * client.dll+45F48C -> keyswitch uint16_t 1
	 * client.dll+4B0FB0 -> keyswitch uint16_t 2
	 * client.dll+4B4D24 -> jumpstate -> 4 stand 5 jump uint8_t
	 * client.dll+4B4D1C -> is 65 when space is pressed (?) uint8_t
	 * client.dll+45928F -> is non-zero when loading
	 * client.dll+4BD730 -> camera height, float
	 */
	static const uint32_t offset_keyswitch    =       0x45F48C;
	static const uint32_t offset_keyswitch2   =       0x4B0FB0;
	static const uint32_t offset_jumpstate    =       0x4B4D24;
	static const uint32_t offset_spacekey     =       0x4B4D1C;
	static const uint32_t offset_loading1     =       0x45928F;
	static const uint32_t offset_cameraHeight =       0x4BD730;


	/*
	 * server.dll+650EE8 -> onground 1 or 0 uint8_t
	 * server.dll+5F7D14 -> server keyswitch uint16_t 1
	 * server.dll+5F7D18 -> server keyswitch uint16_t 2
	 * server.dll+635334 -> offset(F2D)-> onground byte is non zero when true
	 *
	 * disable speedlimit (breaks ABH)
	 * server.dll+182B88 - F3 0F11 40 40         - movss [eax+40],xmm0  X-AXIS
	 * server.dll+182B96 - F3 0F11 40 44         - movss [eax+44],xmm0  Y-AXIS
	 *
	 */
	static const uint32_t offset_ongroundW    =       0x650EE8;
	static const uint32_t offset_ongroundR    =       0x650F2C;
    static const uint32_t offset_keyswitchsrv =       0x5F7D14;
    static const uint32_t offset_keyswitchsrv2=       0x5F7D18;
    static const uint32_t offset_ongroundFUCK =       0x635334;
    static const uint32_t offset_ongroundFUCK_bytes =    0xF2D;
    static const uint32_t offset_speedlimitX  =       0x182B88;
    static const uint32_t offset_speedlimitY  =       0x182B96;
    static const uint32_t offset_cvar_airacc  =       0x64F03C;


	/*
	 * keyswitch keys
	 */
	static const uint16_t key_Fire            =            0x1;
	static const uint16_t key_Jump            =            0x2;
	static const uint16_t key_Duck            =            0x4;
	static const uint16_t key_W               =            0x8;
	static const uint16_t key_S               =           0x10;
	static const uint16_t key_Use             =           0x20;
	static const uint16_t key_A               =          0x200;
	static const uint16_t key_D               =          0x400;
	static const uint16_t key_AltFire         =          0x800;

	/*
	 * byte arrays
	 */
	const uint8_t bytes_speedlimitX[5] = {0xF3,0x0F,0x11,0x40,0x40};
	const uint8_t bytes_speedlimitY[5] = {0xF3,0x0F,0x11,0x40,0x44};



	/*
	 * private/internal functions /  other variables
	 */


	void  PrintPointerError();
	void  BhopThread(void *lpParameter);
};

#endif /* PLAYER_HPP_ */
