#include "Battlemap.h"
#include <cmath>
#include <algorithm>

// BattleMap 생성자: 초기 배치, 배경 로드, 버튼 위치 등 초기화
BattleMap::BattleMap(int screenWidth, int screenHeight, int lineCount)
    : screenWidth(screenWidth),
    screenHeight(screenHeight),
    lineCount(lineCount)
{
    // 각 라인의 높이 계산 (화면 높이를 라인 수로 나눔)
    lineHeight = screenHeight / lineCount;
    // 좌측 및 우측 UI 박스의 크기 계산
    boxWidth = screenWidth / 6;
    boxHeight = screenHeight / 3;

    // 왼쪽 배치 영역의 중심 계산: UI 박스 중앙 좌표 (왼쪽 병사 배치를 위한 기준)
    float centerX = boxWidth / 2.0f;
    float centerY = boxHeight + boxHeight / 2.0f;
    leftAreaCenter = { centerX, centerY };

    // 왼쪽 병사들의 초기 위치를 생성하고 저장
    Vector2 pos1 = { centerX - 25, centerY - 25 };
    Vector2 pos2 = { centerX + 25, centerY - 25 };
    Vector2 pos3 = { centerX - 25, centerY + 25 };
    Vector2 pos4 = { centerX + 25, centerY + 25 };

    // 각 병사를 생성하여 LeftBoxSoldiers 벡터에 추가 (각 타입에 따라 Soldier 생성자에서 초기값 설정)
    LeftBoxSoldiers.emplace_back(pos1, RANGED);
    LeftBoxSoldiers.emplace_back(pos2, TANK);
    LeftBoxSoldiers.emplace_back(pos3, MELEE);
    LeftBoxSoldiers.emplace_back(pos4, MELEE);
    // 재배치 시 사용하기 위해 초기 위치들을 leftInitialPositions 벡터에 저장
    leftInitialPositions.push_back(pos1);
    leftInitialPositions.push_back(pos2);
    leftInitialPositions.push_back(pos3);
    leftInitialPositions.push_back(pos4);

    // 오른쪽 병사 생성: 각 라인마다 CAT 타입 병사 배치
    // 우측 배치는 화면 오른쪽, UI 박스 영역을 제외한 영역의 x 좌표로 설정
    float catX = screenWidth - boxWidth - 20;
    for (int i = 0; i < lineCount; i++) {
        // 각 라인의 중앙 y 좌표 계산
        float catY = i * lineHeight + lineHeight / 2.0f;
        // RightBoxSoldiers 벡터에 CAT 병사 추가
        RightBoxSoldiers.emplace_back(Vector2{ catX, catY }, CAT);
    }

    // 각 라인의 점유 상태를 저장하는 벡터를 초기화 (모든 라인은 초기엔 비어있음)
    lineOccupied = std::vector<bool>(lineCount, false);

    // 배경 텍스처 로드: "assets/Battlemap.png" 파일
    backgroundTexture = LoadTexture("assets/Battlemap.png");

    // 전투 시작 버튼 초기화: 화면 하단 중앙에 위치하며 크기는 100x50
    battleStartButton = { float(screenWidth / 2 - 50), float(screenHeight - 70), 100, 50 };
}

// 소멸자: BattleMap이 소멸될 때 동적 리소스(배경 텍스처)를 언로드
BattleMap::~BattleMap() {
    UnloadTexture(backgroundTexture);
}

// Update 함수: 게임 로직 업데이트 (배치 상태와 전투 상태에 따라 분기)
void BattleMap::Update() {
    float dt = GetFrameTime();  // 프레임 당 경과시간(초)
    Vector2 mousePos = GetMousePosition(); // 현재 마우스 위치

    // 배치 상태 (전투 시작 전)
    if (!battleStarted) {
        // 마우스 왼쪽 버튼 클릭 이벤트 처리
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // 전투 시작 버튼 클릭 시 배치 상태에서 전투 시작으로 전환
            if (CheckCollisionPointRec(mousePos, battleStartButton)) {
                battleStarted = true;
            }
            else {
                // 배치 상태: 왼쪽 병사 선택 및 위치 이동 처리
                if (SelectedSoldier == nullptr) {
                    // 아직 선택된 병사가 없으면, 마우스 위치 근처에 있는 병사를 탐색하여 선택함
                    for (auto& soldier : LeftBoxSoldiers) {
                        Vector2 pos = soldier.GetPosition();
                        float dx = mousePos.x - pos.x;
                        float dy = mousePos.y - pos.y;
                        float distance = sqrtf(dx * dx + dy * dy);
                        if (distance <= 20.0f) {  // 선택 임계값: 20픽셀
                            SelectedSoldier = &soldier;
                            soldier.SetSelected(true);
                        }
                        else {
                            soldier.SetSelected(false);
                        }
                    }
                }
                else {
                    // 이미 선택된 병사가 있으면, 마우스의 y좌표에 해당하는 라인의 중앙으로 이동시킴
                    for (int i = 0; i < lineCount; ++i) {
                        int y = i * lineHeight;
                        if ((int)mousePos.y >= y && (int)mousePos.y < y + lineHeight) {
                            Vector2 newPos = { float(boxWidth + 20), y + lineHeight / 2.0f };
                            bool isOccupied = false;
                            // 이미 다른 병사가 그 위치에 있는지 검사
                            for (auto& soldier : LeftBoxSoldiers) {
                                if (&soldier != SelectedSoldier &&
                                    GetDistance(soldier.GetPosition(), newPos) < 10.0f)
                                {
                                    isOccupied = true;
                                    // 만약 이미 점유되어 있으면, 기존 병사와 선택된 병사의 위치를 교환
                                    Vector2 tempPos = soldier.GetPosition();
                                    soldier.SetPosition(SelectedSoldier->GetPosition());
                                    SelectedSoldier->SetPosition(newPos);
                                    break;
                                }
                            }
                            if (!isOccupied) {
                                // 해당 라인이 비어있으면 선택된 병사를 새 위치로 이동시키고, 그 라인을 점유 처리
                                SelectedSoldier->SetPosition(newPos);
                                lineOccupied[i] = true;
                            }
                            // 선택 해제 후 루프 종료
                            SelectedSoldier->SetSelected(false);
                            SelectedSoldier = nullptr;
                            break;
                        }
                    }
                }
            }
        }
    }
    else { // 전투 시작 후의 로직
        // ① 현재 각 병사의 위치를 저장 (충돌 발생 시 이전 위치로 복원하기 위해)
        std::vector<Vector2> oldLeftPositions, oldRightPositions;
        for (auto& soldier : LeftBoxSoldiers) {
            oldLeftPositions.push_back(soldier.GetPosition());
        }
        for (auto& soldier : RightBoxSoldiers) {
            oldRightPositions.push_back(soldier.GetPosition());
        }

        // ② 각 병사 이동: 왼쪽 병사는 오른쪽으로, 오른쪽 병사는 왼쪽으로 이동
        for (auto& soldier : LeftBoxSoldiers) {
            Vector2 pos = soldier.GetPosition();
            pos.x += soldier.GetSpeed() * dt * 50; // 이동 속도에 시간과 가중치(50)를 곱하여 이동량 결정
            soldier.SetPosition(pos);
        }
        for (auto& soldier : RightBoxSoldiers) {
            Vector2 pos = soldier.GetPosition();
            pos.x -= soldier.GetSpeed() * dt * 50;
            soldier.SetPosition(pos);
        }

        // ③ 이동 후 충돌 감지: 양쪽 병사의 중심 간 거리가 두 병사의 반지름 합보다 작으면 충돌로 간주
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
            // ④ 충돌이 발생하면 이동 업데이트를 취소: 이전 저장된 위치로 복원하여 유닛들이 멈추게 함
            for (size_t i = 0; i < LeftBoxSoldiers.size(); i++) {
                LeftBoxSoldiers[i].SetPosition(oldLeftPositions[i]);
            }
            for (size_t j = 0; j < RightBoxSoldiers.size(); j++) {
                RightBoxSoldiers[j].SetPosition(oldRightPositions[j]);
            }
            // ⑤ 공격 타이머 업데이트: 충돌 상태에서는 2초마다 양쪽 병사가 1:1 대응으로 공격
            attackTimer += dt;
            if (attackTimer >= 2.0f) {
                int count = std::min(LeftBoxSoldiers.size(), RightBoxSoldiers.size());
                for (int i = 0; i < count; i++) {
                    // 서로 공격: 각 병사는 상대 병사의 공격력(GetAtk())만큼 HP를 깎음
                    LeftBoxSoldiers[i].ReceiveAttack(RightBoxSoldiers[i].GetAtk());
                    RightBoxSoldiers[i].ReceiveAttack(LeftBoxSoldiers[i].GetAtk());
                }
                // 공격 후 타이머 초기화
                attackTimer = 0.0f;
            }
        }
        else {
            // 충돌이 없으면 공격 타이머 초기화 (추후 가속 효과 등 추가 가능)
            attackTimer = 0.0f;
        }

        // ⑥ HP가 0 이하인 유닛 재배치 및 체력 복구
        // 왼쪽 유닛 재배치: HP가 0 이하이면, 초기 위치의 좌우 반전 위치로 이동
        for (size_t i = 0; i < LeftBoxSoldiers.size(); i++) {
            if (LeftBoxSoldiers[i].GetHP() <= 0) {
                // 좌우 반전: leftAreaCenter를 기준으로 초기 위치의 대칭 위치 계산
                Vector2 initPos = leftInitialPositions[i];
                Vector2 newPos = { 2 * leftAreaCenter.x - initPos.x, initPos.y };
                LeftBoxSoldiers[i].SetPosition(newPos);
                LeftBoxSoldiers[i].ResetHP();  // 체력 초기값으로 복구
            }
        }
        // 오른쪽 유닛 재배치: HP가 0 이하이면, 왼쪽 유닛들의 초기 위치로 재배치
        for (size_t i = 0; i < RightBoxSoldiers.size(); i++) {
            if (RightBoxSoldiers[i].GetHP() <= 0) {
                // 재배치할 때 인덱스 순환 적용 (여러 유닛이 있을 경우)
                size_t index = i % leftInitialPositions.size();
                Vector2 newPos = leftInitialPositions[index];
                RightBoxSoldiers[i].SetPosition(newPos);
                RightBoxSoldiers[i].ResetHP();
            }
        }
    }
}

// Draw 함수: 배경, UI, 전투 시작 버튼 및 모든 유닛을 그림
void BattleMap::Draw() {
    // 배경 텍스처 그리기
    Rectangle src = { 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height };
    Rectangle dst = { 0, 0, (float)screenWidth, (float)screenHeight };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(backgroundTexture, src, dst, origin, 0.0f, WHITE);

    // 전투 구역 선과 사이드 UI 그리기
    DrawLines();
    DrawSideUI();

    // 배치 상태이면 전투 시작 버튼 표시
    if (!battleStarted) {
        DrawRectangleRec(battleStartButton, LIGHTGRAY);
        DrawText("Battle Start", battleStartButton.x + 5, battleStartButton.y + 15, 20, BLACK);
    }

    // 모든 왼쪽 및 오른쪽 병사 그리기
    for (const auto& soldier : LeftBoxSoldiers) {
        soldier.Draw();
    }
    for (const auto& soldier : RightBoxSoldiers) {
        soldier.Draw();
    }
}

// DrawLines 함수: 전투 구역을 구분하는 선들을 그림 (좌측 UI 박스와 우측 UI 박스 사이 영역)
void BattleMap::DrawLines() {
    int lineWidth = screenWidth - boxWidth * 2;
    for (int i = 0; i < lineCount; ++i) {
        int y = i * lineHeight;
        DrawRectangleLines(boxWidth, y, lineWidth, lineHeight, GRAY);
    }
}

// DrawSideUI 함수: 좌우 사이드 UI 박스를 그림
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

// GetDistance 함수: 두 Vector2 사이의 유클리드 거리를 계산하여 반환
float BattleMap::GetDistance(Vector2 a, Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}
