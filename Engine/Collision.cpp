#include "Collision.h"
#include "Matrix.h"
#include "GameObject.h"
#include "Engine.h"

CS230::RectCollision::RectCollision(Math::irect boundary, GameObject* object) :
    boundary(boundary),
    object(object)
{
}

Math::rect CS230::RectCollision::WorldBoundary() {
    return {
        object->GetMatrix() * static_cast<Math::vec2>(boundary.point_1),
        object->GetMatrix() * static_cast<Math::vec2>(boundary.point_2)
    };
}

Rectangle CS230::RectCollision::ToRaylibScreenRect(const Math::TransformationMatrix& camera_matrix) {
    // 1. 월드 좌표계의 사각형 경계를 가져옵니다.
    Math::rect world_box = WorldBoundary();

    // 2. 사각형의 두 꼭짓점을 카메라 행렬로 변환하여 화면 좌표(Y축이 위로 향하는)로 만듭니다.
    Math::vec2 screen_p1 = camera_matrix * world_box.point_1;
    Math::vec2 screen_p2 = camera_matrix * world_box.point_2;

    // 3. 변환된 두 점을 기준으로 화면 사각형의 좌/우/위/아래를 계산합니다.
    float left = static_cast<float>(std::min(screen_p1.x, screen_p2.x));
    float right = static_cast<float>(std::max(screen_p1.x, screen_p2.x));
    float top_engine = static_cast<float>(std::max(screen_p1.y, screen_p2.y)); // 엔진 좌표계 (Y-up)
    float bottom_engine = static_cast<float>(std::min(screen_p1.y, screen_p2.y)); // 엔진 좌표계 (Y-up)

    // 4. Raylib의 Rectangle 구조체(x, y, width, height) 형식으로 변환합니다.
    const float screen_height = static_cast<float>(Engine::GetWindow().GetSize().y);

    // Raylib의 화면 좌표계 (Y-down)에 맞게 y 좌표를 변환합니다.
    float screen_rect_x = left;
    float screen_rect_y = screen_height - top_engine; // 화면의 Y 좌표는 위에서부터 시작
    float screen_rect_width = right - left;
    float screen_rect_height = top_engine - bottom_engine;

    return { screen_rect_x, screen_rect_y, screen_rect_width, screen_rect_height };
}

void CS230::RectCollision::Draw(Math::TransformationMatrix display_matrix) {
    const double render_height = rlGetFramebufferHeight();

    Math::rect world_boundary = WorldBoundary();

    Math::vec2 bottom_left = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Bottom() };
    Math::vec2 bottom_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Bottom() };
    Math::vec2 top_left = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Top() };
    Math::vec2 top_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Top() };

    bottom_left.y = bottom_left.y * -1 + render_height;
    bottom_right.y = bottom_right.y * -1 + render_height;
    top_left.y = top_left.y * -1 + render_height;
    top_right.y = top_right.y * -1 + render_height;

    DrawLine(int(top_left.x), int(top_left.y), int(top_right.x), int(top_right.y), WHITE);
    DrawLine(int(bottom_right.x), int(bottom_right.y), int(top_right.x), int(top_right.y), WHITE);
    DrawLine(int(bottom_right.x), int(bottom_right.y), int(bottom_left.x), int(bottom_left.y), WHITE);
    DrawLine(int(top_left.x), int(top_left.y), int(bottom_left.x), int(bottom_left.y), WHITE);
}

bool CS230::RectCollision::IsCollidingWith(GameObject* other_object) {
    Collision* other_collider = other_object->GetGOComponent<Collision>();


    if (other_collider == nullptr) {
        Engine::GetLogger().LogError("No collision component found");
        return false;
    }


    if (other_collider->Shape() != CollisionShape::Rect) {
        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }

    Math::rect rectangle_1 = WorldBoundary();
    Math::rect rectangle_2 = dynamic_cast<RectCollision*>(other_collider)->WorldBoundary();

    if (rectangle_1.Right() <= rectangle_2.Left() ||
        rectangle_1.Left() >= rectangle_2.Right() ||
        rectangle_1.Top() <= rectangle_2.Bottom() ||
        rectangle_1.Bottom() >= rectangle_2.Top()   
        ) {
        return false;
    }
    return true;
}


CS230::CircleCollision::CircleCollision(double radius, GameObject* object):
    radius(radius),
    object(object)
{
}

void CS230::CircleCollision::Draw(Math::TransformationMatrix display_matrix) {
    const double render_height = rlGetFramebufferHeight();
    Math::vec2 transformed_position = display_matrix * object->GetPosition();
    transformed_position.y = transformed_position.y * -1 + render_height;
    const int num_segments = 36;
    Math::vec2 previous_vertex;
    for (int i = 0; i <= num_segments + 1; i++) {
        double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(num_segments);
        Math::vec2 vertex = {
            transformed_position.x + GetRadius() * std::cos(theta),
            transformed_position.y + GetRadius() * std::sin(theta)
        };
        if (i > 0) {
            DrawLine(int(vertex.x), int(vertex.y), int(previous_vertex.x), int(previous_vertex.y), WHITE);
        }
        previous_vertex = vertex;
    }
}
bool CS230::CircleCollision::IsCollidingWith(GameObject* other_object)
{
    Collision* other_collider = other_object->GetGOComponent<Collision>();
    if (other_collider == nullptr) {
        Engine::GetLogger().LogError("No collision component found");
        return false;
    }

    if (other_collider->Shape() != CollisionShape::Circle) {
        Engine::GetLogger().LogError("Circle vs unsupported type");
        return false;
    }

    double radius1 = GetRadius();
    double radius2 = dynamic_cast<CircleCollision*>(other_collider)->GetRadius();
    double dx = object->GetPosition().x - other_object->GetPosition().x;
    double dy = object->GetPosition().y - other_object->GetPosition().y;
    double distance_squared = (dx * dx) + (dy * dy);
    double radi_sq = (radius1 + radius2) * (radius1 + radius2);
    if (distance_squared <= radi_sq) {
        return true;
    }
    return false;
}

double CS230::CircleCollision::GetRadius()
{
    return radius;
}
