#include "Battlemap.h"

BattleMap::BattleMap(int screenWidth, int screenHeight, int lineCount)
	: screenWidth(screenWidth),
	screenHeight(screenHeight),
	lineCount(lineCount)
{
	lineHeight = screenHeight / lineCount;	// 라인 당 너비
	boxWidth = screenWidth / 6;
	boxHeight = screenHeight / 3;

	float centerX = boxWidth / 2.0f;
	float centerY = boxHeight + boxHeight / 2.0f;
	float offset = 50.0f;	// 병사 간격

	for (int i = 0; i < 4; ++i) {
		float x = centerX + (i % 2) * offset - offset / 2.0f;	// 병사 x 좌표
		float y = centerY + (i / 2) * offset - offset / 2.0f;	// 병사 y 좌표
		LeftBoxSoldiers.emplace_back(Vector2{ x, y });
	}

	lineOccupied = std::vector<bool>(lineCount, false);
}

void BattleMap::Update() {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mousePos = GetMousePosition(); // 마우스 위치 가져오기

		if (SelectedSoldier == nullptr) {
			// 병사 선택
			for (auto& soldier : LeftBoxSoldiers) {
				Vector2 pos = soldier.GetPosition();

				// 마우스랑 병사 사이 거리
				float dx = mousePos.x - pos.x;
				float dy = mousePos.y - pos.y;
				float distance = sqrtf(dx * dx + dy * dy);

				// 병사 선택
				if (distance <= 20.0f) {
					SelectedSoldier = &soldier;
					soldier.SetSelected(true);	// 선택 병사 색 변경 (Green -> Orange)
				}
				else {
					soldier.SetSelected(false);	// 나머지 그대로
				}
			}
		}
		else {
			// 병사 배치
			for (int i = 0; i < lineCount; ++i) {
				int y = i * lineHeight;

				// 라인 안에 마우스 있는지 확인 
				if ((int)mousePos.y >= y && (int)mousePos.y < y + lineHeight) {
					// 라인 안에 병사가 있으면 선택 해제
					if (lineOccupied[i]) {
						SelectedSoldier->SetSelected(false);
						SelectedSoldier = nullptr;
						break;
					}

					// 병사 없으면 라인 젤 앞에 병사 배치
					Vector2 newPos = { (float)(boxWidth + 20), y + lineHeight / 2.0f };
					SelectedSoldier->SetPosition(newPos);	// 병사 이동
					lineOccupied[i] = true;					// 라인 점유 true

					SelectedSoldier->SetSelected(false);	// 색깔 복구
					SelectedSoldier = nullptr;				// 상태 초기화
					break;
				}
			}
		}
	}
}

void BattleMap::Draw() {
	DrawLines();
	DrawSideUI();
	for (const auto& soldier : LeftBoxSoldiers) {
		soldier.Draw();
	}
}

void BattleMap::DrawLines() {
	int lineWidth = screenWidth - boxWidth * 2;
	for (int i = 0; i < lineCount; ++i) {
		int y = i * lineHeight;
		DrawRectangleLines(boxWidth, y, lineWidth, lineHeight, GRAY);
	}
}

void BattleMap::DrawSideUI() {
	boxWidth = screenWidth / 6;
	boxHeight = screenHeight / 3;

	for (int i = 0; i < 3; ++i) {
		DrawRectangleLines(0, i * boxHeight, boxWidth, boxHeight, BLACK);
	}

	for (int i = 0; i < 3; ++i) {
		DrawRectangleLines(screenWidth - boxWidth, i * boxHeight, boxWidth, boxHeight, BLACK);
	}
}

float BattleMap::GetDistance(Vector2 a, Vector2 b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrtf(dx * dx + dy * dy);
}