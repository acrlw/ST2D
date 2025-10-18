#pragma once

#include "AbstractShape.h"
namespace ST
{

    class ST_API Polygon : public AbstractShape
    {

    public:
        Polygon();
        const std::array<Vector2, Constant::MaxPolygonVertices>& vertices() const;
        void set(const std::initializer_list<Vector2>& vertices);
		void set(const std::array<Vector2, Constant::MaxPolygonVertices>& vertices, const uint32_t& count);
		void set(Vector2* vertices, const uint32_t& count);
        Vector2 center()const override;
        void scale(const real& factor) override;
        bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) override;
        size_t count()const;
        const std::array<uint32_t, Constant::MaxSupportFieldSize>& supportField() const;
    protected:
        std::array<Vector2, Constant::MaxPolygonVertices> m_vertices;
        std::array<uint32_t, Constant::MaxSupportFieldSize> m_supportField;
        size_t m_count = 0;
        void updateVertices();
        void buildSupportField();
    };
}
