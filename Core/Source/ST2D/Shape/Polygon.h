#pragma once

#include "Shape.h"
namespace ST
{
    class ST_API Polygon : public Shape
    {

    public:
        Polygon();

        const std::vector<Vector2>& vertices() const;
        void set(const std::initializer_list<Vector2>& vertices);
		void set(std::vector<Vector2>& vertices);
		void set(Vector2* vertices, const uint32_t& count);
        Vector2 center()const override;
        void scale(const real& factor) override;
        bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) override;
    protected:
        std::vector<Vector2> m_vertices;
        void updateVertices();
    };
}
