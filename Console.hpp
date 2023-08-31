/*
 * Console.hpp
 *
 *  Created on: 30 Aug 2023
 *      Author: Jackson Smackson
 */

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_
#include "ConsoleCMD.hpp"
#include "Player.hpp"

class Console {
public:
	Console(const char *Title, Player *ply, ConsoleCMD *CMD);
	virtual ~Console();

	bool InputLoop();
	void ActivateUpdateThread();

private:
	ConsoleCMD *CMD;
	Player *ply;

	bool UpdateThreadEnabled = false;
	HANDLE UpdateThreadHandle = 0;

	void UpdateThreadFunction(void* lpParameter);

};

#endif /* CONSOLE_HPP_ */
