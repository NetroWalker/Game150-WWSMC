#include "Material.h"
#include<cmath>

Stone::Stone() : stone_count(0), generation_timer(0.0)
{
}

void Stone::Update(double dt, int num_castles)
{
    if (num_castles <= 0) {
        return; // 성이 없으면 자원 생산 중단
    }

    // 성의 개수만큼 시간 가속
    generation_timer += dt * num_castles;

    if (generation_timer >= TIME_PER_RESOURCE) {
        // 타이머가 10초를 넘었으면, 넘은 만큼 자원을 생산합니다.
        int new_resources = static_cast<int>(floor(generation_timer / TIME_PER_RESOURCE));
        stone_count += new_resources;

        // 생산한 만큼 타이머에서 시간을 차감합니다.
        generation_timer -= new_resources * TIME_PER_RESOURCE;
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

