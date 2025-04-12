#include "Battlemap.h"

BattleMap::BattleMap(int screenWidth, int screenHeight, int lineCount)
	: screenWidth(screenWidth),
	screenHeight(screenHeight),
	lineCount(lineCount)
{
	lineHeight = screenHeight / lineCount;	// ���� �� �ʺ�
	boxWidth = screenWidth / 6;
	boxHeight = screenHeight / 3;

	float centerX = boxWidth / 2.0f;
	float centerY = boxHeight + boxHeight / 2.0f;
	float offset = 50.0f;	// ���� ����

	for (int i = 0; i < 4; ++i) {
		float x = centerX + (i % 2) * offset - offset / 2.0f;	// ���� x ��ǥ
		float y = centerY + (i / 2) * offset - offset / 2.0f;	// ���� y ��ǥ
		LeftBoxSoldiers.emplace_back(Vector2{ x, y });
	}

	lineOccupied = std::vector<bool>(lineCount, false);
}

void BattleMap::Update() {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mousePos = GetMousePosition(); // ���콺 ��ġ ��������

		if (SelectedSoldier == nullptr) {
			// ���� ����
			for (auto& soldier : LeftBoxSoldiers) {
				Vector2 pos = soldier.GetPosition();

				// ���콺�� ���� ���� �Ÿ�
				float dx = mousePos.x - pos.x;
				float dy = mousePos.y - pos.y;
				float distance = sqrtf(dx * dx + dy * dy);

				// ���� ����
				if (distance <= 20.0f) {
					SelectedSoldier = &soldier;
					soldier.SetSelected(true);	// ���� ���� �� ���� (Green -> Orange)
				}
				else {
					soldier.SetSelected(false);	// ������ �״��
				}
			}
		}
		else {
			// ���� ��ġ
			for (int i = 0; i < lineCount; ++i) {
				int y = i * lineHeight;

				// ���� �ȿ� ���콺 �ִ��� Ȯ�� 
				if ((int)mousePos.y >= y && (int)mousePos.y < y + lineHeight) {
					// ���� �ȿ� ���簡 ������ ���� ����
					if (lineOccupied[i]) {
						SelectedSoldier->SetSelected(false);
						SelectedSoldier = nullptr;
						break;
					}

					// ���� ������ ���� �� �տ� ���� ��ġ
					Vector2 newPos = { (float)(boxWidth + 20), y + lineHeight / 2.0f };
					SelectedSoldier->SetPosition(newPos);	// ���� �̵�
					lineOccupied[i] = true;					// ���� ���� true

					SelectedSoldier->SetSelected(false);	// ���� ����
					SelectedSoldier = nullptr;				// ���� �ʱ�ȭ
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