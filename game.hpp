#pragma once
#include "VecBase.hpp"
#include <vector>

enum Player {
	NONE,P1,P2
};

struct Planet {
	Player owner;
	float size;
	float population;
	Vec3 pos;
};
struct Craft {
	Vec3 pos;
	Player owner;
	Planet* dest;

	Craft(Vec3 pos, Player owner, Planet& dest): pos(pos), owner(owner), dest(&dest) {}
};
extern std::vector<Planet> planets;
extern std::vector<Craft> crafts;
