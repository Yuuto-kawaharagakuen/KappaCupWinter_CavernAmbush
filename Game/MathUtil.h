#pragma once

namespace MathUtil
{
	inline float Clamp(float value, float minValue, float maxValue)
	{
		if (value < minValue) {
			return minValue;
		}
		if (value > maxValue) {
			return maxValue;
		}
		return value;
	}

	inline float Clamp01(float value)
	{
		return Clamp(value, 0.0f, 1.0f);
	}

	inline float SmoothStep(float t)
	{
		return t * t * (3.0f - 2.0f * t);
	}

	inline float EaseOutBack(float t)
	{
		const float overshoot = 1.70158f;
		float u = t - 1.0f;
		return 1.0f + (overshoot + 1.0f) * u * u * u + overshoot * u * u;
	}

	inline float Damp(float current, float target, float speed, float deltaTime)
	{
		return target + (current - target) * expf(-speed * deltaTime);
	}

	inline float YawFromDirection(const Vector3& direction)
	{
		return Math::RadToDeg(atan2f(direction.x, direction.z));
	}
}
