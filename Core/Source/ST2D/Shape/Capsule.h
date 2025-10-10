#pragma once

#include "Shape.h"

namespace ST
{
    class ST_API Capsule : public Shape
    {
    public:
        Capsule(real halfLength = 0.0f, real radius = 0.0f);
        bool contains(const Vector2& point, const real& epsilon) override;
        void scale(const real& factor) override;
        Vector2 center() const override;
        void set(real halfLength, real radius);
        void setRadius(real radius);
        void setHalfLength(real halfLength);

        real halfLength()const;
        real radius()const;

        Vector2 topLeft()const;
        Vector2 bottomLeft()const;
        Vector2 topRight()const;
        Vector2 bottomRight()const;
        Vector2 anchorTop()const;
        Vector2 anchorBottom()const;
        std::array<Vector2, 4> boxVertices()const;
    private:
        real m_halfLength;
        real m_radius;
    };
}
