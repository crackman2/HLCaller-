/*
 * Entity.cpp
 *
 *  Created on: 5 Sept 2023
 *      Author: Jackson Smackson
 */

#include "Entity.hpp"
#include <iostream>

Entity::Entity() {
}

Entity::~Entity() {
	// TODO Auto-generated destructor stub
}

void Entity::Init(uint32_t entityBase){
	pos.x  = (float*)(*(uint32_t*)(entityBase) + offset_posx);
	pos.y  = (float*)(*(uint32_t*)(entityBase) + offset_posy);
	pos.z  = (float*)(*(uint32_t*)(entityBase) + offset_posz);

	health = (int*)(*(uint32_t*)(entityBase) + offset_hp);
	nameptr   = (uint32_t*)(*(uint32_t*)(entityBase) + offset_nameStr);
}

char* Entity::name() {
	return (char*)(*nameptr);
}
