#pragma once

#include "Shape.h"
namespace ST
{
    class ST_API Ellipse : public Shape
    {

    public:
        Ellipse(const real& width = 0, const real& height = 0);
        void set(const Vector2& leftTop, const Vector2& rightBottom);
        void set(const real& width, const real& height);

        real width()const;
        void setWidth(const real& width);

        real height()const;
        void setHeight(const real& height);

        void scale(const real& factor) override;
        bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) override;
        Vector2 center()const override;
        real A()const;
        real B()const;
        real C()const;
    private:
        real m_width;
        real m_height;
    };
}
