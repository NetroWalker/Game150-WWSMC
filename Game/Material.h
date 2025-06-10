//Material.h
#pragma once
#include"../Engine/Component.h"

class Stone : public CS230::Component{
public:
	Stone();
	//void Update(double dt, int num_castles);
	void AddResources(int amount);
	int GetStoneCount() const;
	bool SpendResources(int amount);
private: 
	int stone_count;
};