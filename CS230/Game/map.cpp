#include "Map.h"
#include <cmath>
#include <algorithm>

// 생성자: 타일 텍스처 로드, 파라미터 초기화
Map::Map(Vector2 center, float radiusX, float radiusY)
    : center(center),
    radiusX(radiusX),
    radiusY(radiusY),
    squashFactor(0.5f)
{
    tileTexture = LoadTexture("assets/150map.png");
}

// 소멸자: 텍스처 언로드
Map::~Map() {
    UnloadTexture(tileTexture);
}

// 매 프레임 호출: 마우휠·드래그 처리
void Map::Update() {
    HandleMouseWheelInput();
    UpdateMapPosition();
}

// 타일 계산 및 그리기
void Map::Draw() {
    tiles.clear();

    float root3 = sqrtf(3.0f);
    float xOffset = radiusX * 1.5f;
    float yOffset = radiusY * root3;

    for (int x = 0; x < mapW; ++x) {
        for (int y = 0; y < mapH; ++y) {
            float hexX = center.x + x * xOffset;
            float hexY = center.y + y * yOffset + ((x % 2) * (yOffset * 0.5f));
            float squashedY = hexY * squashFactor;

            HexTile tile{ x, y, { hexX, squashedY } };
            tiles.push_back(tile);

            // 꼭짓점 계산
            for (int i = 0; i < 6; ++i) {
                float angle = PI / 3 * i;
                points[i] = {
                    hexX + radiusX * cosf(angle),
                    (hexY + radiusY * sinf(angle)) * squashFactor
                };
            }

            // 텍스처 그리기
            Rectangle src{ 0,0, (float)tileTexture.width, (float)tileTexture.height };
            Rectangle dst{ hexX, squashedY,
                           radiusX * 2.0f,
                           radiusY * 2.0f * squashFactor };
            Vector2 origin{ radiusX, radiusY * squashFactor };
            DrawTexturePro(tileTexture, src, dst, origin, 0.0f, WHITE);

            // 선 그리기
            DrawHexagon(DARKGREEN);
        }
    }
}

// 마우스 포인터가 타일 내부에 있는지 검사
bool Map::IsMouseOver() {
    return IsPointInHexagon(GetMousePosition());
}

// 유틸: HEX의 꼭짓점 선 그리기
void Map::DrawHexagon(Color color) {
    for (int i = 0; i < 6; ++i) {
        DrawLineV(points[i], points[(i + 1) % 6], color);
    }
}

// 포인트 인-폴리곤 테스트
bool Map::IsPointInHexagon(Vector2 pt) const {
    int cnt = 0;
    for (int i = 0; i < 6; ++i) {
        Vector2 a = points[i];
        Vector2 b = points[(i + 1) % 6];
        if ((a.y > pt.y) != (b.y > pt.y)) {
            float t = (pt.y - a.y) / (b.y - a.y);
            if (pt.x < a.x + t * (b.x - a.x)) ++cnt;
        }
    }
    return (cnt % 2) == 1;
}

// 마우스 드래그로 맵 이동 (필요 시 활성화)
void Map::UpdateMapPosition() {
    // (주석 처리된 드래그 로직 생략)
}

// 마우스 휠로 squashFactor 조정 (스크롤 줌용)
void Map::HandleMouseWheelInput() {
    float w = GetMouseWheelMove();
    if (w != 0.0f) {
        Vector2 screenC{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        float oldY = center.y + (screenC.y - center.y) * squashFactor;
        squashFactor = std::clamp(squashFactor - w * 0.05f, 0.5f, 1.0f);
        float newY = center.y + (screenC.y - center.y) * squashFactor;
        center.y += (oldY - newY);
    }
}

// 단순 조회 메서드들
HexTile* Map::GetTileAt(int x, int y) {
    for (auto& t : tiles)
        if (t.x == x && t.y == y) return &t;
    return nullptr;
}
HexTile* Map::GetTileAtPosition(Vector2 pos) {
    for (auto& t : tiles) {
        if (Vector2Distance(pos, t.center) <= radiusX * 0.9f)
            return &t;
    }
    return nullptr;
}

// ------------------------------
// 이동 가능한 타일 계산 멤버 함수
std::vector<HexTile> Map::GetMovableTiles(HexTile* from) const {
    std::vector<HexTile> result;
    if (!from) return result;

    int x = from->x, y = from->y;
    int dx[6], dy[6];
    if (x % 2) {
        int odx[] = { 0,1,-1,-1,0,1 };
        int ody[] = { -1,0,0,1,1,1 };
        std::copy(std::begin(odx), std::end(odx), dx);
        std::copy(std::begin(ody), std::end(ody), dy);
    }
    else {
        int edx[] = { 0,0,1,-1,-1,1 };
        int edy[] = { -1,1,0,0,-1,-1 };
        std::copy(std::begin(edx), std::end(edx), dx);
        std::copy(std::begin(edy), std::end(edy), dy);
    }
    for (int i = 0; i < 6; ++i) {
        if (auto t = GetTileAt(x + dx[i], y + dy[i]))
            result.push_back(*t);
    }
    return result;
}
