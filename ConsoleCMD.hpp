/*
 * ConsoleCMD.hpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */

#ifndef CONSOLECMD_HPP_
#define CONSOLECMD_HPP_
#include "Player.hpp"
#include "WinOverlay.hpp"

class ConsoleCMD {
public:
	Player *ply;
	WinOverlay *win;

	ConsoleCMD(Player *ply, WinOverlay *win);
	virtual ~ConsoleCMD();

	void Clear();
	void ShowHelp();
	void Msg(const char *input);
	void PlayerData();
	void ReInitPlayer();
	void PrintMaxVel();
	void ResetMaxVel();
	void Speedlimit();
	void AirAccelerate(float value);
	void Exit();
	void BhopBoost(float value);
	void InfAmmo();
	void SprayShot();
	void DefaultBhop();
	void Overlay();
	void Teleport(uint32_t id);

	void hl2Msg(const char *Message);


private:
	void printboth(const char* msg);

};

#endif /* CONSOLECMD_HPP_ */
