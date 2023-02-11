#pragma once
#include <vector>
#include <functional>

namespace Collision
{
	/*********************************************************
	* @brief	è’ìÀëŒè€
	********************************************************/
	struct Line
	{
		D3DXVECTOR3 Start;
		D3DXVECTOR3 End;

		Line(const D3DXVECTOR3& start = V3_ZERO, const D3DXVECTOR3& end = V3_ZERO);

		float Length() const;
		// Get point along segment where 0 <= t <= 1
		D3DXVECTOR3 PointOnLine(float t) const;
		// Get minimum distance squared between point and line segment
		float MinDist(const D3DXVECTOR3& point, float* outVerticalDist, D3DXVECTOR3* outPoint) const;
	};

	struct Plane
	{
		D3DXVECTOR3 Normal;

		// @brief	ç¿ïWå¥ì_Ç©ÇÁïΩñ Ç‹Ç≈ÇÃãóó£
		float Dist;

		Plane(const D3DXVECTOR3& normal, float d);
		// Construct plane from three points
		//ab -> ac angle < 180
		Plane(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c);
		// Get the signed distance between the point and the plane
		float SignedDist(const D3DXVECTOR3& point) const;
	};

	struct Sphere
	{
		D3DXVECTOR3 Center;
		float Radius;
		Sphere(const D3DXVECTOR3& center, float radius);
		bool Contains(const D3DXVECTOR3& point) const;
	};

	struct AABB
	{
		D3DXVECTOR3 Min;
		D3DXVECTOR3 Max;

		AABB(const D3DXVECTOR3& min = -V3_ONE, const D3DXVECTOR3& max = V3_ONE);
		bool Contains(const D3DXVECTOR3& point) const;
		float MinDistSq(const D3DXVECTOR3& point) const;
	};


	/*********************************************************
	* @brief	äeéÌóﬁÇÃè’ìÀîªíË
	********************************************************/
	bool Intersect(const Line& l, const Sphere& s,
		float* outDist = nullptr, D3DXVECTOR3* outPoint = nullptr);
	//bool Intersect(const Line& l, const Plane& p, float& outT);
	bool Intersect(const Line& l, const AABB& aabb, const D3DXMATRIX& boxWorldMtx,
		float* outDist = nullptr, D3DXVECTOR3* outPoint = nullptr, D3DXVECTOR3* outNormal = nullptr);

}