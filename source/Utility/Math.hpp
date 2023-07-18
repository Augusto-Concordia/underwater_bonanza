#pragma once

struct Math {
    constexpr static float PI = 3.1415f;

    static float Map(float value, float inMin, float inMax, float outMin, float outMax)
    {
        // Check for division by zero
        if (inMin == inMax)
            return outMin;

        // Perform the mapping
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
};