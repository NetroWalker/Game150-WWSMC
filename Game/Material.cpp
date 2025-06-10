#include "Material.h"
#include<cmath>

Stone::Stone() : stone_count(0)
{
}

void Stone::AddResources(int amount) {
    if (amount > 0) {
        stone_count += amount;
    }
}

int Stone::GetStoneCount() const
{
	return stone_count;
}

bool Stone::SpendResources(int amount)
{
    if(stone_count >= amount) {
        stone_count -= amount;
        return true; // 사용 성공
    }
    return false; // 자원 부족
}

