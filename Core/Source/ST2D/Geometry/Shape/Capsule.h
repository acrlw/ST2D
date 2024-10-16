#pragma once

#include "Shape.h"

namespace ST
{
    class ST_API Capsule : public Shape
    {
    public:
        Capsule(real width = 0.0f, real height = 0.0f);
        bool contains(const Vector2& point, const real& epsilon) override;
        void scale(const real& factor) override;
        Vector2 center() const override;
        void set(real width, real height);
        void setWidth(real width);
        void setHeight(real height);
        real width()const;
        real height()const;
        real halfWidth()const;
        real halfHeight()const;
        Vector2 topLeft()const;
        Vector2 bottomLeft()const;
        Vector2 topRight()const;
        Vector2 bottomRight()const;
        std::vector<Vector2> boxVertices()const;
    private:

        real m_halfWidth;
        real m_halfHeight;
    };
}
