/*
 * ESP.cpp
 *
 *  Created on: 5 Sept 2023
 *      Author: Jackson Smackson
 */

#include "ESP.hpp"
#include "Entity.hpp"
#include <windows.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cmath>

ESP::ESP(HDC hdc, RECT *clientRect, Player *ply) {
	this->hdc = hdc;
	this->ply = ply;
	this->clientRect = clientRect;
	ServerDLLBase = (uint32_t) GetModuleHandle("server.dll");
	EngineDLLBase = (uint32_t) GetModuleHandle("engine.dll");
	ClientDLLBase = (uint32_t) GetModuleHandle("client.dll");

	if (ServerDLLBase != 0) {
		entityListBase = *(uint32_t*) (ServerDLLBase + offset_entityList);
		for (int i = 0; i < maxEntites; i++) {
			//entityList.emplace_back(entityListBase + i * 4);
			entityList.emplace_back();
		}
	} else {
		std::cout << "ESP: init failed (server.dll)" << std::endl;
	}

	if (EngineDLLBase != 0) {
		viewMatrix = (float*) ((*(uint32_t*) (EngineDLLBase + offset_viewMatrix)) + bytes_viewMatrix);
	} else
		return;

	std::cout << "engine.dll + offset_Matrix: " << std::hex << EngineDLLBase + offset_viewMatrix << std::endl;
	std::cout << "are pointing to: " << std::hex << *(uint32_t*) (EngineDLLBase + offset_viewMatrix) << std::endl;
	std::cout << "adding offset to it: " << (*(uint32_t*) (EngineDLLBase + offset_viewMatrix)) + bytes_viewMatrix << std::endl;
	std::cout << "\nViewMatrix: " << std::hex << viewMatrix << std::endl;

	hFont = CreateFont(13,   // Height
			0,    // Width
			0,    // Escapement
			0,    // Orientation
			FW_NORMAL,        // Weight
			FALSE,            // Italic
			FALSE,            // Underline
			FALSE,            // Strikeout
			DEFAULT_CHARSET,  // Charset
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			NONANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Liberation Mono"          // Font name
			);

	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
}

ESP::~ESP() {
	// TODO Auto-generated destructor stub
}

void ESP::Draw() {
	if (!ply->IsLoading() and ply->IsInGame()) {
		SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 255, 0));

		std::stringstream ssz;
		std::string szname;

		ServerDLLBase = (uint32_t) GetModuleHandle("server.dll");
		EngineDLLBase = (uint32_t) GetModuleHandle("engine.dll");
		//ClientDLLBase = (uint32_t) GetModuleHandle("client.dll");

		if (ServerDLLBase != 0) {
			entityListBase = *(uint32_t*) (ServerDLLBase + offset_entityList);
		} else
			return;

		if (EngineDLLBase != 0) {
			viewMatrix = (float*) ((*(uint32_t*) (EngineDLLBase + offset_viewMatrix)) + bytes_viewMatrix);
		} else
			return;

		/*
		 for(int i = 0; i < currentEntityCount; i++){
		 ss << "i:" << std::hex <<
		 entityList[i].pos.x;
		 TextOutA(hdc,30,30+(20*i),ss.str().c_str(),ss.str().length());
		 ss.str("");

		 }
		 */
		int iLast = 0;

		//uint32_t entlistTmp = *(uint32_t*)(ServerDLLBase + offset_entityList);

		uint32_t entlistTmp = 0;

		GetCurrentEntityCount();
		int maxLoop = (currentEntityCount > maxEntites) ? maxEntites : currentEntityCount;

		for (int i = 0; i < maxLoop; i++) {
			InitEntities();

			ssz << std::left << std::setw(4) << "i:" << std::dec << std::setw(4) << i << " x:" << std::fixed << std::setprecision(0)
					<< std::setw(7) << *entityList[i].pos.x << " y:" << std::fixed << std::setprecision(0) << std::setw(7)
					<< *entityList[i].pos.y << " z:" << std::fixed << std::setprecision(0) << std::setw(7) << *entityList[i].pos.z << " hp:"
					<< std::setw(5) << *entityList[i].health << " name: " << entityList[i].name() << "\n";

			TextOutA(hdc, 8, 85 + (11 * i), ssz.str().c_str(), ssz.str().length());
			ssz.str("");
		}

		SelectObject(hdc, hPen);

		POINT p;
		float *t = new float[3];
		float dist = 0;
		float scaler = 10000;
		float fract = 0;
		//float *plypos = new float[3];

		for (int i = 0; i < maxLoop; i++) {
			if (strcmp(entityList[i].name(), "player") != 0) {
				ply->GetPosition();
				t[0] = *entityList[i].pos.x - ply->pos.x;
				t[1] = *entityList[i].pos.y - ply->pos.y;
				t[2] = *entityList[i].pos.z - *ply->camHeight;

				//plypos[0] = ply->pos.x;
				//plypos[1] = ply->pos.y;
				//plypos[2] = ply->pos.z;

				//t[0] = *entityList[i].pos.x;
				//t[1] = *entityList[i].pos.y;
				//t[2] = *entityList[i].pos.z;

				if (ProjectToScreen(viewMatrix, t, clientRect->right, clientRect->bottom, p)) {
					MoveToEx(hdc, clientRect->right / 2, clientRect->bottom, NULL);
					LineTo(hdc, p.x, p.y);

					dist = sqrtf(powf(t[0], 2) + powf(t[1], 2) + powf(t[2], 2));

					fract = ((scaler / dist) > 20) ? ((scaler / dist)) : 20;
					//Rectangle(hdc, p.x - fract, p.y - fract, p.x + fract, p.y + fract);
					RectangleLines(hdc, p, fract, fract);

					ssz << entityList[i].name();
					TextOutA(hdc, p.x + fract + 1, p.y - fract, ssz.str().c_str(), ssz.str().length());
					ssz.str("");

					ssz << std::fixed << std::setprecision(0) << *entityList[i].pos.x;
					TextOutA(hdc, p.x + fract + 1, p.y - fract + 11, ssz.str().c_str(), ssz.str().length());
					ssz.str("");

					ssz << std::fixed << std::setprecision(0) << *entityList[i].pos.y;
					TextOutA(hdc, p.x + fract + 1, p.y - fract + 22, ssz.str().c_str(), ssz.str().length());
					ssz.str("");

					ssz << std::fixed << std::setprecision(0) << *entityList[i].pos.z;
					TextOutA(hdc, p.x + fract + 1, p.y - fract + 33, ssz.str().c_str(), ssz.str().length());
					ssz.str("");

					ssz << std::dec << *entityList[i].health;
					TextOutA(hdc, p.x + fract + 1, p.y - fract + 44, ssz.str().c_str(), ssz.str().length());
					ssz.str("");
				}
			}
		}
		delete[] t;
		//delete[] plypos;
	}
}

void ESP::InitEntities() {
	GetCurrentEntityCount();
	int maxLoop = (currentEntityCount > maxEntites) ? maxEntites : currentEntityCount;
	for (int i = 0; i < maxLoop; i++)
		entityList[i].Init(entityListBase + i * 4);
}

/* private */

void ESP::GetCurrentEntityCount() {
	currentEntityCount = *(uint32_t*) (ServerDLLBase + offset_entityCount);
}

bool ESP::ProjectToScreen(const float *viewMatrix, const float *pointInCameraSpace, int screenWidth, int screenHeight, POINT &screenPoint) {
	// Apply the view matrix to the point
	float pointInHomogeneous[4];
	for (int i = 0; i < 4; i++) {
		pointInHomogeneous[i] = 0.0f;
		for (int j = 0; j < 4; j++) {
			pointInHomogeneous[i] += viewMatrix[i * 4 + j] * pointInCameraSpace[j];
		}
	}

	// Check if the point is behind the camera (z-coordinate should be positive)
	if (pointInHomogeneous[2] <= 0.0f)
		return false;

	// Normalize the point
	for (int i = 0; i < 4; i++) {
		pointInHomogeneous[i] /= pointInHomogeneous[3];
	}
	POINT tmpP;
	// Calculate screen coordinates
	tmpP.x = static_cast<LONG>((pointInHomogeneous[0] + 1.0f) * 0.5f * static_cast<float>(screenWidth));
	tmpP.y = static_cast<LONG>((1.0f - pointInHomogeneous[1]) * 0.5f * static_cast<float>(screenHeight));

	if ((tmpP.x > screenWidth) or (tmpP.x < 0) or (tmpP.y > screenHeight) or (tmpP.y < 0))
		return false;

	screenPoint = tmpP;

	return true;
}
/* private */

void ESP::RectangleLines(HDC hdc, POINT center, float width, float height) {
	MoveToEx(hdc,center.x - width, center.y - height, NULL);
	LineTo(hdc,center.x + width, center.y - height);

	MoveToEx(hdc,center.x + width, center.y - height, NULL);
	LineTo(hdc,center.x + width, center.y + height);

	MoveToEx(hdc,center.x + width, center.y + height, NULL);
	LineTo(hdc,center.x - width, center.y + height);

	MoveToEx(hdc,center.x - width, center.y + height, NULL);
	LineTo(hdc,center.x - width, center.y - height);
}
