#include "Easing.h"

namespace ST
{

    float EasingFunction::smoothStep(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3 - 2 * t);
    }

    float EasingFunction::easeInOutQuad(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        if (t < 0.5f)
            return 2.0f * t * t;
        else
            return -1.0f + (4.0f - 2.0f * t) * t;
    }

    float EasingFunction::easeInOutSine(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return -0.5f * (std::cos(std::numbers::pi * t) - 1.0f);
    }

    float EasingFunction::easeOutExpo(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t == 1.0f ? 1.0f : 1.0f * (-std::pow(2.0f, -10.0f * t) + 1.0f);
    }

    float EasingFunction::easeInOutExpo(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        if (t == 0.0f || t == 1.0f)
            return t;
        if (t < 0.5f)
            return 0.5f * std::pow(2.0f, (20.0f * t) - 10.0f);
        else
            return -0.5f * std::pow(2.0f, (-20.0f * t) + 10.0f) + 1.0f;
    }

    float EasingFunction::easeInOutCubic(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        if (t < 0.5f)
            return 4.0f * t * t * t;
        else
            return (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
    }

    float EasingFunction::easeOutCubic(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return 1.0f - std::pow(1.0f - t, 3);
    }

    float EasingFunction::easeOutQuint(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return 1.0f - std::pow(1.0f - t, 5);
    }

    float EasingFunction::easeOutQuad(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * (2.0f - t);
    }

    float EasingFunction::linear(float t)
    {
	    return t;
    }

    float EasingFunction::easeInOutQuint(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t < 0.5f ? 16.0f * std::pow(t, 5) : 1.0f - std::pow(-2.0f * t + 2.0f, 5) * 0.5f;

    }

}