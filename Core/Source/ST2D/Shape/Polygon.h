#pragma once

#include "Shape.h"
namespace ST
{

    constexpr size_t MaxPolygonVertices = 32;
    class ST_API Polygon : public Shape
    {

    public:
        Polygon();
        const std::array<Vector2, MaxPolygonVertices>& vertices() const;
        void set(const std::initializer_list<Vector2>& vertices);
		void set(const std::array<Vector2, MaxPolygonVertices>& vertices, const uint32_t& count);
		void set(Vector2* vertices, const uint32_t& count);
        Vector2 center()const override;
        void scale(const real& factor) override;
        bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) override;
        size_t count()const;
    protected:
        std::array<Vector2, MaxPolygonVertices> m_vertices;
        size_t m_count = 0;
        void updateVertices();
    };
}
