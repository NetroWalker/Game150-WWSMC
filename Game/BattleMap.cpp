#include "Battlemap.h"
#include <cmath>
#include <algorithm>

// BattleMap ������: �ʱ� ��ġ, ��� �ε�, ��ư ��ġ �� �ʱ�ȭ
BattleMap::BattleMap(int screenWidth, int screenHeight, int lineCount)
    : screenWidth(screenWidth),
    screenHeight(screenHeight),
    lineCount(lineCount)
{
    // �� ������ ���� ��� (ȭ�� ���̸� ���� ���� ����)
    lineHeight = screenHeight / lineCount;
    // ���� �� ���� UI �ڽ��� ũ�� ���
    boxWidth = screenWidth / 6;
    boxHeight = screenHeight / 3;

    // ���� ��ġ ������ �߽� ���: UI �ڽ� �߾� ��ǥ (���� ���� ��ġ�� ���� ����)
    float centerX = boxWidth / 2.0f;
    float centerY = boxHeight + boxHeight / 2.0f;
    leftAreaCenter = { centerX, centerY };

    // ���� ������� �ʱ� ��ġ�� �����ϰ� ����
    Vector2 pos1 = { centerX - 25, centerY - 25 };
    Vector2 pos2 = { centerX + 25, centerY - 25 };
    Vector2 pos3 = { centerX - 25, centerY + 25 };
    Vector2 pos4 = { centerX + 25, centerY + 25 };

    // �� ���縦 �����Ͽ� LeftBoxSoldiers ���Ϳ� �߰� (�� Ÿ�Կ� ���� Soldier �����ڿ��� �ʱⰪ ����)
    LeftBoxSoldiers.emplace_back(pos1, RANGED);
    LeftBoxSoldiers.emplace_back(pos2, TANK);
    LeftBoxSoldiers.emplace_back(pos3, MELEE);
    LeftBoxSoldiers.emplace_back(pos4, MELEE);
    // ���ġ �� ����ϱ� ���� �ʱ� ��ġ���� leftInitialPositions ���Ϳ� ����
    leftInitialPositions.push_back(pos1);
    leftInitialPositions.push_back(pos2);
    leftInitialPositions.push_back(pos3);
    leftInitialPositions.push_back(pos4);

    // ������ ���� ����: �� ���θ��� CAT Ÿ�� ���� ��ġ
    // ���� ��ġ�� ȭ�� ������, UI �ڽ� ������ ������ ������ x ��ǥ�� ����
    float catX = screenWidth - boxWidth - 20;
    for (int i = 0; i < lineCount; i++) {
        // �� ������ �߾� y ��ǥ ���
        float catY = i * lineHeight + lineHeight / 2.0f;
        // RightBoxSoldiers ���Ϳ� CAT ���� �߰�
        RightBoxSoldiers.emplace_back(Vector2{ catX, catY }, CAT);
    }

    // �� ������ ���� ���¸� �����ϴ� ���͸� �ʱ�ȭ (��� ������ �ʱ⿣ �������)
    lineOccupied = std::vector<bool>(lineCount, false);

    // ��� �ؽ�ó �ε�: "assets/Battlemap.png" ����
    backgroundTexture = LoadTexture("assets/Battlemap.png");

    // ���� ���� ��ư �ʱ�ȭ: ȭ�� �ϴ� �߾ӿ� ��ġ�ϸ� ũ��� 100x50
    battleStartButton = { float(screenWidth / 2 - 50), float(screenHeight - 70), 100, 50 };
}

// �Ҹ���: BattleMap�� �Ҹ�� �� ���� ���ҽ�(��� �ؽ�ó)�� ��ε�
BattleMap::~BattleMap() {
    UnloadTexture(backgroundTexture);
}

// Update �Լ�: ���� ���� ������Ʈ (��ġ ���¿� ���� ���¿� ���� �б�)
void BattleMap::Update() {
    float dt = GetFrameTime();  // ������ �� ����ð�(��)
    Vector2 mousePos = GetMousePosition(); // ���� ���콺 ��ġ

    // ��ġ ���� (���� ���� ��)
    if (!battleStarted) {
        // ���콺 ���� ��ư Ŭ�� �̺�Ʈ ó��
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // ���� ���� ��ư Ŭ�� �� ��ġ ���¿��� ���� �������� ��ȯ
            if (CheckCollisionPointRec(mousePos, battleStartButton)) {
                battleStarted = true;
            }
            else {
                // ��ġ ����: ���� ���� ���� �� ��ġ �̵� ó��
                if (SelectedSoldier == nullptr) {
                    // ���� ���õ� ���簡 ������, ���콺 ��ġ ��ó�� �ִ� ���縦 Ž���Ͽ� ������
                    for (auto& soldier : LeftBoxSoldiers) {
                        Vector2 pos = soldier.GetPosition();
                        float dx = mousePos.x - pos.x;
                        float dy = mousePos.y - pos.y;
                        float distance = sqrtf(dx * dx + dy * dy);
                        if (distance <= 20.0f) {  // ���� �Ӱ谪: 20�ȼ�
                            SelectedSoldier = &soldier;
                            soldier.SetSelected(true);
                        }
                        else {
                            soldier.SetSelected(false);
                        }
                    }
                }
                else {
                    // �̹� ���õ� ���簡 ������, ���콺�� y��ǥ�� �ش��ϴ� ������ �߾����� �̵���Ŵ
                    for (int i = 0; i < lineCount; ++i) {
                        int y = i * lineHeight;
                        if ((int)mousePos.y >= y && (int)mousePos.y < y + lineHeight) {
                            Vector2 newPos = { float(boxWidth + 20), y + lineHeight / 2.0f };
                            bool isOccupied = false;
                            // �̹� �ٸ� ���簡 �� ��ġ�� �ִ��� �˻�
                            for (auto& soldier : LeftBoxSoldiers) {
                                if (&soldier != SelectedSoldier &&
                                    GetDistance(soldier.GetPosition(), newPos) < 10.0f)
                                {
                                    isOccupied = true;
                                    // ���� �̹� �����Ǿ� ������, ���� ����� ���õ� ������ ��ġ�� ��ȯ
                                    Vector2 tempPos = soldier.GetPosition();
                                    soldier.SetPosition(SelectedSoldier->GetPosition());
                                    SelectedSoldier->SetPosition(newPos);
                                    break;
                                }
                            }
                            if (!isOccupied) {
                                // �ش� ������ ��������� ���õ� ���縦 �� ��ġ�� �̵���Ű��, �� ������ ���� ó��
                                SelectedSoldier->SetPosition(newPos);
                                lineOccupied[i] = true;
                            }
                            // ���� ���� �� ���� ����
                            SelectedSoldier->SetSelected(false);
                            SelectedSoldier = nullptr;
                            break;
                        }
                    }
                }
            }
        }
    }
    else { // ���� ���� ���� ����
        // �� ���� �� ������ ��ġ�� ���� (�浹 �߻� �� ���� ��ġ�� �����ϱ� ����)
        std::vector<Vector2> oldLeftPositions, oldRightPositions;
        for (auto& soldier : LeftBoxSoldiers) {
            oldLeftPositions.push_back(soldier.GetPosition());
        }
        for (auto& soldier : RightBoxSoldiers) {
            oldRightPositions.push_back(soldier.GetPosition());
        }

        // �� �� ���� �̵�: ���� ����� ����������, ������ ����� �������� �̵�
        for (auto& soldier : LeftBoxSoldiers) {
            Vector2 pos = soldier.GetPosition();
            pos.x += soldier.GetSpeed() * dt * 50; // �̵� �ӵ��� �ð��� ����ġ(50)�� ���Ͽ� �̵��� ����
            soldier.SetPosition(pos);
        }
        for (auto& soldier : RightBoxSoldiers) {
            Vector2 pos = soldier.GetPosition();
            pos.x -= soldier.GetSpeed() * dt * 50;
            soldier.SetPosition(pos);
        }

        // �� �̵� �� �浹 ����: ���� ������ �߽� �� �Ÿ��� �� ������ ������ �պ��� ������ �浹�� ����
        bool collided = false;
        for (size_t i = 0; i < LeftBoxSoldiers.size(); i++) {
            for (size_t j = 0; j < RightBoxSoldiers.size(); j++) {
                if (GetDistance(LeftBoxSoldiers[i].GetPosition(), RightBoxSoldiers[j].GetPosition()) <
                    (LeftBoxSoldiers[i].GetRadius() + RightBoxSoldiers[j].GetRadius()))
                {
                    collided = true;
                    break;
                }
            }
            if (collided) break;
        }
        if (collided) {
            // �� �浹�� �߻��ϸ� �̵� ������Ʈ�� ���: ���� ����� ��ġ�� �����Ͽ� ���ֵ��� ���߰� ��
            for (size_t i = 0; i < LeftBoxSoldiers.size(); i++) {
                LeftBoxSoldiers[i].SetPosition(oldLeftPositions[i]);
            }
            for (size_t j = 0; j < RightBoxSoldiers.size(); j++) {
                RightBoxSoldiers[j].SetPosition(oldRightPositions[j]);
            }
            // �� ���� Ÿ�̸� ������Ʈ: �浹 ���¿����� 2�ʸ��� ���� ���簡 1:1 �������� ����
            attackTimer += dt;
            if (attackTimer >= 2.0f) {
                int count = std::min(LeftBoxSoldiers.size(), RightBoxSoldiers.size());
                for (int i = 0; i < count; i++) {
                    // ���� ����: �� ����� ��� ������ ���ݷ�(GetAtk())��ŭ HP�� ����
                    LeftBoxSoldiers[i].ReceiveAttack(RightBoxSoldiers[i].GetAtk());
                    RightBoxSoldiers[i].ReceiveAttack(LeftBoxSoldiers[i].GetAtk());
                }
                // ���� �� Ÿ�̸� �ʱ�ȭ
                attackTimer = 0.0f;
            }
        }
        else {
            // �浹�� ������ ���� Ÿ�̸� �ʱ�ȭ (���� ���� ȿ�� �� �߰� ����)
            attackTimer = 0.0f;
        }

        // �� HP�� 0 ������ ���� ���ġ �� ü�� ����
        // ���� ���� ���ġ: HP�� 0 �����̸�, �ʱ� ��ġ�� �¿� ���� ��ġ�� �̵�
        for (size_t i = 0; i < LeftBoxSoldiers.size(); i++) {
            if (LeftBoxSoldiers[i].GetHP() <= 0) {
                // �¿� ����: leftAreaCenter�� �������� �ʱ� ��ġ�� ��Ī ��ġ ���
                Vector2 initPos = leftInitialPositions[i];
                Vector2 newPos = { 2 * leftAreaCenter.x - initPos.x, initPos.y };
                LeftBoxSoldiers[i].SetPosition(newPos);
                LeftBoxSoldiers[i].ResetHP();  // ü�� �ʱⰪ���� ����
            }
        }
        // ������ ���� ���ġ: HP�� 0 �����̸�, ���� ���ֵ��� �ʱ� ��ġ�� ���ġ
        for (size_t i = 0; i < RightBoxSoldiers.size(); i++) {
            if (RightBoxSoldiers[i].GetHP() <= 0) {
                // ���ġ�� �� �ε��� ��ȯ ���� (���� ������ ���� ���)
                size_t index = i % leftInitialPositions.size();
                Vector2 newPos = leftInitialPositions[index];
                RightBoxSoldiers[i].SetPosition(newPos);
                RightBoxSoldiers[i].ResetHP();
            }
        }
    }
}

// Draw �Լ�: ���, UI, ���� ���� ��ư �� ��� ������ �׸�
void BattleMap::Draw() {
    // ��� �ؽ�ó �׸���
    Rectangle src = { 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height };
    Rectangle dst = { 0, 0, (float)screenWidth, (float)screenHeight };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(backgroundTexture, src, dst, origin, 0.0f, WHITE);

    // ���� ���� ���� ���̵� UI �׸���
    DrawLines();
    DrawSideUI();

    // ��ġ �����̸� ���� ���� ��ư ǥ��
    if (!battleStarted) {
        DrawRectangleRec(battleStartButton, LIGHTGRAY);
        DrawText("Battle Start", battleStartButton.x + 5, battleStartButton.y + 15, 20, BLACK);
    }

    // ��� ���� �� ������ ���� �׸���
    for (const auto& soldier : LeftBoxSoldiers) {
        soldier.Draw();
    }
    for (const auto& soldier : RightBoxSoldiers) {
        soldier.Draw();
    }
}

// DrawLines �Լ�: ���� ������ �����ϴ� ������ �׸� (���� UI �ڽ��� ���� UI �ڽ� ���� ����)
void BattleMap::DrawLines() {
    int lineWidth = screenWidth - boxWidth * 2;
    for (int i = 0; i < lineCount; ++i) {
        int y = i * lineHeight;
        DrawRectangleLines(boxWidth, y, lineWidth, lineHeight, GRAY);
    }
}

// DrawSideUI �Լ�: �¿� ���̵� UI �ڽ��� �׸�
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

// GetDistance �Լ�: �� Vector2 ������ ��Ŭ���� �Ÿ��� ����Ͽ� ��ȯ
float BattleMap::GetDistance(Vector2 a, Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}
