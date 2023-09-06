/*
 * ESP.hpp
 *
 *  Created on: 5 Sept 2023
 *      Author: Jackson Smackson
 */

#ifndef ESP_HPP_
#define ESP_HPP_
#include "Entity.hpp"
#include "Player.hpp"
#include <windows.h>
#include <cstdint>
#include <vector>

class ESP {
public:
	HDC hdc;
	Player *ply;
	std::vector<Entity> entityList;

	uint32_t currentEntityCount = 0;

	float *viewMatrix;


	ESP(HDC hdc, RECT *clientRect, Player *ply);
	virtual ~ESP();

	void Draw();
	void InitEntities();

private:
	void GetCurrentEntityCount();

	bool ProjectToScreen(const float* viewMatrix, const float* pointInCameraSpace, int screenWidth, int screenHeight, POINT& screenPoint);

	static const uint32_t maxEntites = 64;

	HFONT hFont;
	HPEN hPen;

	RECT *clientRect;

	/*
	 * server.dll+5C08B0 -> entity count
	 * server.dll+5C08B4 -> ptr to entity list
	 *
	 */
	uint32_t	ServerDLLBase;
	uint32_t    EngineDLLBase;
	uint32_t	ClientDLLBase;
	uint32_t    entityListBase;

	static const uint32_t offset_entityCount = 0x5C08B0;
	static const uint32_t offset_entityList  = 0x5C08B4;

	/*
	 * [engine.dll+586C1C] + 2D4 -> viewMatrix
	 */
	static const uint32_t offset_viewMatrix  = 0x586C1C;
	static const uint32_t bytes_viewMatrix = 0x2D4;

	void RectangleLines(HDC hdc, POINT center, float width, float height);
};

#endif /* ESP_HPP_ */
