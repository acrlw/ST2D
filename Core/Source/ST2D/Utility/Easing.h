#pragma once

#include "ST2D/Log.h"
#include "ST2D/Math/Vector2.h"
#include "ST2D/Math/Vector3.h"
#include "ST2D/Math/Vector4.h"
#include "ST2D/Math/Matrix2x2.h"
#include "ST2D/Math/Matrix3x3.h"
#include "ST2D/Math/Matrix4x4.h"
#include "ST2D/Math/Quaternion.h"

namespace ST
{
    namespace EasingFunction
    {
        ST_API float smoothStep(float t);

        ST_API float easeInOutQuad(float t);

        ST_API float easeInOutSine(float t);

        ST_API float easeInOutQuint(float t);

        ST_API float easeOutExpo(float t);

        ST_API float easeInOutExpo(float t);

        ST_API float easeInOutCubic(float t);

        ST_API float easeOutCubic(float t);

        ST_API float easeOutQuint(float t);

        ST_API float easeOutQuad(float t);
    }

    template<typename T>
    struct LerpTraits {};

    template<>
    struct LerpTraits<float>
    {
        static float lerp(float startValue, float endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<Vector2>
    {
        static Vector2 lerp(const Vector2& startValue, const Vector2& endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<Vector3>
    {
        static Vector3 lerp(const Vector3& startValue, const Vector3& endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<Vector4>
    {
        static Vector4 lerp(const Vector4& startValue, const Vector4& endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<glm::quat>
    {
        static glm::quat lerp(const glm::quat& startValue, const glm::quat& endValue, float t)
        {
            return glm::slerp(startValue, endValue, t);
        }
    };

    template<>
    struct LerpTraits<Matrix4x4>
    {
        static Matrix4x4 lerp(const Matrix4x4& startValue, const Matrix4x4& endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<Matrix3x3>
    {
        static Matrix3x3 lerp(const Matrix3x3& startValue, const Matrix3x3& endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<Matrix2x2>
    {
        static Matrix2x2 lerp(const Matrix2x2& startValue, const Matrix2x2& endValue, float t)
        {
            return startValue * (1.0f - t) + endValue * t;
        }
    };

    template<>
    struct LerpTraits<int>
    {
        static int lerp(int startValue, int endValue, float t)
        {
            return static_cast<int>(startValue * (1.0f - t) + endValue * t);
        }
    };

    template<>
    struct LerpTraits<unsigned int>
    {
        static unsigned int lerp(unsigned int startValue, unsigned int endValue, float t)
        {
            return static_cast<unsigned int>(startValue * (1.0f - t) + endValue * t);
        }
    };

    template<>
    struct LerpTraits<char>
    {
        static char lerp(char startValue, char endValue, float t)
        {
            float start = static_cast<float>(startValue);
            float end = static_cast<float>(endValue);
            float current = start * (1.0f - t) + end * t;
            //let current to the ceiling

            current = start < end ? std::ceil(current) : std::floor(current);

            if (start > end)
                std::swap(start, end);

            current = std::clamp(current, start, end);

            char result = static_cast<char>(current);

            return result;
        }
    };

    template<>
    struct LerpTraits<std::string>
    {
        static std::string lerp(const std::string& startValue, const std::string& endValue, float t)
        {
            assert(startValue.size() == endValue.size());
            std::string result;
            result.reserve(startValue.size());
            for (size_t i = 0; i < startValue.size(); i++)
                result.push_back(LerpTraits<char>::lerp(startValue[i], endValue[i], t));

            return result;
        }
    };


    template<typename T>
    class EasingObject
    {
    public:
        explicit EasingObject(const T& value);

        EasingObject& operator=(const T& value);

        EasingObject& operator=(const EasingObject& other);

        void setEasingFunction(std::function<float(float)> easingFunction);

        void update(float deltaTime);

        void restart(const T& startValue, const T& endValue, float duration);

        void continueTo(const T& endValue, float duration);

        void finish();

        T startValue()const;

        T endValue()const;

        T value()const;

        bool isFinished() const;

        bool enableEasing() const;

        void setEnableEasing(bool enable);

    private:
        bool m_enableEasing = true;
        T m_startValue;
        T m_endValue;
        float m_duration = 0.5f;
        float m_timeAccumulator = 0.0f;

        std::function<float(float)> m_easingFunction;
    };

    template<typename T>
    EasingObject<T>::EasingObject(const T& value) :
        m_startValue(value), m_endValue(value)
    {
        m_easingFunction = EasingFunction::smoothStep;
    }

    template<typename T>
    EasingObject<T>& EasingObject<T>::operator=(const T& value)
    {
        m_startValue = value;
        m_endValue = value;
        return *this;
    }

    template<typename T>
    EasingObject<T>& EasingObject<T>::operator=(const EasingObject& other)
    {
        m_startValue = other.m_startValue;
        m_endValue = other.m_endValue;
        m_duration = other.m_duration;
        m_timeAccumulator = other.m_timeAccumulator;
        m_easingFunction = other.m_easingFunction;
        return *this;
    }

    template<typename T>
    void EasingObject<T>::setEasingFunction(std::function<float(float)> easingFunction)
    {
        m_easingFunction = std::move(easingFunction);
    }

    template<typename T>
    void EasingObject<T>::update(float deltaTime)
    {

        m_timeAccumulator += deltaTime;

        if (m_duration <= 0.0f || !m_enableEasing) {
            m_timeAccumulator = m_duration;
            return;
        }

        if (m_timeAccumulator >= m_duration) {
            m_timeAccumulator = m_duration;
        }
    }

    template<typename T>
    void EasingObject<T>::restart(const T& startValue, const T& endValue, float duration)
    {
        //duration cannot be zero
        CORE_ASSERT(duration > 0.0f, "Duration cannot be zero")


            if (startValue == endValue)
            {
                m_timeAccumulator = m_duration;
                return;
            }

        m_timeAccumulator = 0.0f;
        m_startValue = startValue;
        m_endValue = endValue;
        m_duration = duration;
    }

    template<typename T>
    void EasingObject<T>::continueTo(const T& endValue, float duration)
    {
        restart(value(), endValue, duration);
    }

    template <typename T>
    void EasingObject<T>::finish()
    {
        m_timeAccumulator = m_duration;
    }

    template<typename T>
    T EasingObject<T>::startValue() const
    {
        return m_startValue;
    }

    template<typename T>
    T EasingObject<T>::endValue()const
    {
        return m_endValue;
    }

    template<typename T>
    T EasingObject<T>::value() const
    {
        CORE_ASSERT(m_duration > 0.0f, "Duration cannot be zero")

            float t = m_easingFunction(m_timeAccumulator / m_duration);
        return LerpTraits<T>::lerp(m_startValue, m_endValue, t);
    }

    template<typename T>
    bool EasingObject<T>::isFinished() const
    {
        return m_timeAccumulator >= m_duration && m_duration > 0.0f;
    }

    template<typename T>
    bool EasingObject<T>::enableEasing() const
    {
        return m_enableEasing;
    }

    template<typename T>
    void EasingObject<T>::setEnableEasing(bool enable)
    {
        m_enableEasing = enable;
    }
	
}