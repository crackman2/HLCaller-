/*
 * Entity.hpp
 *
 *  Created on: 5 Sept 2023
 *      Author: Jackson Smackson
 */

#ifndef ENTITY_HPP_
#define ENTITY_HPP_
#include <string>
#include <cstdint>

typedef struct {
	float *x, *y, *z;
} pvec3f;

typedef struct {
	float *x, *y;
} pvec2f;

class Entity {
public:
	pvec3f  pos;
	int   *health;
	//char   *name;
	uint32_t* nameptr;

	Entity();
	virtual ~Entity();

	char* name();

	void Init(uint32_t entityBase);
private:

	static const uint32_t offset_nameStr = 0x5C; /* c-string */

	static const uint32_t offset_posx = 0x28; /* float */
	static const uint32_t offset_posy = 0x2C; /* float */
	static const uint32_t offset_posz = 0x30; /* float */

	static const uint32_t offset_hp = 0xE0; /* signed int */
};

/**
 * server.dll+5C08B0 -> number of entities (including player), right next to it (4 bytes+ offset) is a pointer to the entity list
 */

#endif /* ENTITY_HPP_ */
