#pragma once
#include "map.h"
#include "General.h"
#include "Texture.h"

class Battlestart {
public:
	Battlestart();
	~Battlestart();
	void Load();
	void Draw();
	void Update();
	void Unload();

private:
	bool Battlstarted = false;
	General genral;
	Texture texture;
};