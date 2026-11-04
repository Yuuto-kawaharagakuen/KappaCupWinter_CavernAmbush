#pragma once

namespace HitUtil
{
	inline float Clamp01(float value)
	{
		if (value < 0.0f) {
			return 0.0f;
		}
		if (value > 1.0f) {
			return 1.0f;
		}
		return value;
	}

	inline float ClosestSegmentSegment(
		const Vector3& p1, const Vector3& q1,
		const Vector3& p2, const Vector3& q2,
		float& s, float& t)
	{
		const float epsilon = 0.0001f;
		Vector3 d1 = q1 - p1;
		Vector3 d2 = q2 - p2;
		Vector3 r = p1 - p2;
		float a = d1.Dot(d1);
		float e = d2.Dot(d2);
		float f = d2.Dot(r);

		if (a <= epsilon && e <= epsilon) {
			s = t = 0.0f;
			return r.Length();
		}
		if (a <= epsilon) {
			s = 0.0f;
			t = Clamp01(f / e);
		}
		else {
			float c = d1.Dot(r);
			if (e <= epsilon) {
				t = 0.0f;
				s = Clamp01(-c / a);
			}
			else {
				float b = d1.Dot(d2);
				float denom = a * e - b * b;
				s = denom > epsilon ? Clamp01((b * f - c * e) / denom) : 0.0f;
				t = (b * s + f) / e;
				if (t < 0.0f) {
					t = 0.0f;
					s = Clamp01(-c / a);
				}
				else if (t > 1.0f) {
					t = 1.0f;
					s = Clamp01((b - c) / a);
				}
			}
		}

		Vector3 diff = (p1 + d1 * s) - (p2 + d2 * t);
		return diff.Length();
	}

	inline bool SegmentCapsule(
		const Vector3& start, const Vector3& end, float radius,
		const Vector3& footPosition, float capsuleRadius, float capsuleHeight,
		float& hitRate)
	{
		Vector3 bottom = footPosition;
		bottom.y += capsuleRadius;
		Vector3 top = bottom;
		top.y += capsuleHeight;

		float s = 0.0f;
		float t = 0.0f;
		float distance = ClosestSegmentSegment(start, end, bottom, top, s, t);
		if (distance > capsuleRadius + radius) {
			return false;
		}
		hitRate = s;
		return true;
	}
}
