//Material.h
#pragma once
#include"../Engine/Component.h"

class Stone : public CS230::Component{
public:
	Stone();
	void Update(double dt, int num_castles);

	int GetStoneCount() const;
	bool SpendResources(int amount);
private: 
	static constexpr double TIME_PER_RESOURCE = 2.0;
	int stone_count;
	double generation_timer;
};