#include "Rect.h"
#include "Component.h"

namespace Math {
    class TransformationMatrix;
}

namespace CS230 {
    class GameObject;
    class Collision : public Component {
    public:
        enum class CollisionShape {
            Rect,
            Circle
        };
        virtual CollisionShape Shape() = 0;
        virtual bool IsCollidingWith(GameObject* other_object) = 0;
        virtual void Draw(Math::TransformationMatrix display_matrix) = 0;
    };

    class RectCollision : public Collision {
    public:
        RectCollision(Math::irect boundary, GameObject* object);
        CollisionShape Shape()override {
            return CollisionShape::Rect;
        }
        void Draw(Math::TransformationMatrix display_matrix)override;
        bool IsCollidingWith(GameObject* other_object)override;
        Math::rect WorldBoundary();
    private:
        GameObject* object;
        Math::irect boundary;
    };
    class CircleCollision : public Collision {
    public:
        CircleCollision(double radius, GameObject* object);
        CollisionShape Shape() override {
            return CollisionShape::Circle;
        }
        void Draw(Math::TransformationMatrix display_matrix);
        bool IsCollidingWith(GameObject* other_object)override;
        double GetRadius();
    private:
        GameObject* object;
        double radius;
    };

}
