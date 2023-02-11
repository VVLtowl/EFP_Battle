#include "main.h"
#include "Collision.h"

namespace Collision
{
#pragma region ========== line ==========
	Line::Line(const D3DXVECTOR3& start, const D3DXVECTOR3& end) :
		Start(start),
		End(end)
	{
	}

	float Line::Length() const
	{
		D3DXVECTOR3 ab = (End - Start);
		return D3DXVec3Length(&ab);
	}

	D3DXVECTOR3 Line::PointOnLine(float t) const
	{
		return Start + (End - Start) * t;
	}

	float Line::MinDist(const D3DXVECTOR3& point, float* outVerticalDist, D3DXVECTOR3* outPoint) const
	{
		// Construct vectors
		D3DXVECTOR3 ab = End - Start;
		D3DXVECTOR3 ba = -1.0f * ab;
		D3DXVECTOR3 ac = point - Start;
		D3DXVECTOR3 bc = point - End;

		float scale = D3DXVec3Dot(&ac, &ab) / D3DXVec3Dot(&ab, &ab);
		D3DXVECTOR3 verticalDistVec = ac - scale * ab;
		*outVerticalDist = D3DXVec3Length(&verticalDistVec);

		// Case 1: C projects prior to A
		if (D3DXVec3Dot(&ab, &ac) < 0.0f)
		{
			*outPoint = Start;
			return D3DXVec3Length(&ac);
		}
		// Case 2: C projects after B
		else if (D3DXVec3Dot(&ba, &bc) < 0.0f)
		{
			*outPoint = End;
			return D3DXVec3Length(&bc);
		}
		// Case 3: C projects onto line
		else
		{
			// Compute p
			float scale = D3DXVec3Dot(&ac, &ab) / D3DXVec3Dot(&ab, &ab);
			D3DXVECTOR3 shortestDistVec = ac - scale * ab;
			*outPoint = Start + scale * ab;
			// Compute length squared of ac - p
			return D3DXVec3Length(&shortestDistVec);
		}
	}

#pragma endregion

#pragma region ========== Plane ==========

	Plane::Plane(const D3DXVECTOR3& normal, float d) :
		Normal(normal),
		Dist(d)
	{
	}

	Plane::Plane(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c)
	{
		// Compute vectors from a to b and a to c
		D3DXVECTOR3 ab = b - a;
		D3DXVECTOR3 ac = c - a;
		// Cross product and normalize to get normal
		D3DXVec3Cross(&Normal, &ab, &ac);
		D3DXVec3Normalize(&Normal, &Normal);
		// d = P dot n
		Dist = D3DXVec3Dot(&a, &Normal);
	}

	float Plane::SignedDist(const D3DXVECTOR3& point) const
	{
		return D3DXVec3Dot(&point, &Normal) - Dist;
	}

#pragma endregion

#pragma region ========== sphere ==========

	Sphere::Sphere(const D3DXVECTOR3& center, float radius) :
		Center(center),
		Radius(radius)
	{
	}

	bool Sphere::Contains(const D3DXVECTOR3& point) const
	{
		// need world matrix

		// Get distance squared between center and point
		D3DXVECTOR3 centerPt = Center - point;
		float distSq = D3DXVec3LengthSq(&centerPt);
		return distSq <= (Radius * Radius);
	}


#pragma endregion

#pragma region ========== AABB ==========

	AABB::AABB(const D3DXVECTOR3& min, const D3DXVECTOR3& max) :
		Min(min),
		Max(max)
	{
	}

	bool AABB::Contains(const D3DXVECTOR3& point) const
	{
		// need world matrix 

		bool outside = point.x < Min.x ||
			point.y < Min.y ||
			point.z < Min.z ||
			point.x > Max.x ||
			point.y > Max.y ||
			point.z > Max.z;
		// If none of these are true, the point is inside the box
		return !outside;
	}

	float AABB::MinDistSq(const D3DXVECTOR3& point) const
	{
		// need world matrix 

		// Compute differences for each axis
		float dx = fmaxf(Min.x - point.x, 0.0f);
		dx = fmaxf(dx, point.x - Max.x);

		float dy = fmaxf(Min.y - point.y, 0.0f);
		dy = fmaxf(dy, point.y - Max.y);

		float dz = fmaxf(Min.z - point.z, 0.0f);
		dz = fmaxf(dy, point.z - Max.z);

		// Distance squared formula
		return dx * dx + dy * dy + dz * dz;
	}

#pragma endregion




#pragma region ========== intersect ==========

	bool Intersect(const Line& l, const Sphere& s,
		float* outDist, D3DXVECTOR3* outPoint)
	{
		*outDist = FLT_MAX;

		D3DXVECTOR3 minDistPoint;
		float verticalDist;
		float minDist = l.MinDist(s.Center, &verticalDist, &minDistPoint);
		float radius = s.Radius;

		if (minDist < radius)
		{
			D3DXVECTOR3 ab = l.End - l.Start;
			float lengthAB = D3DXVec3Length(&ab);
			float lengthSlabPtToVerticalPt = powf(radius * radius - verticalDist * verticalDist, 0.5f);
			float lengthAToCenter = Line(s.Center, l.Start).Length();
			float lengthAToVerticalPt = powf(lengthAToCenter * lengthAToCenter - verticalDist * verticalDist, 0.5f);
			*outDist = fabsf(lengthAToVerticalPt - lengthSlabPtToVerticalPt);
			D3DXVECTOR3 abDir;
			D3DXVec3Normalize(&abDir, &ab);
			*outPoint = l.Start + (*outDist) * abDir;

			return true;
		}
		else
		{
			return false;
		}
	}

	bool Intersect(const Line& l, const AABB& aabb, const D3DXMATRIX& boxWorldMtx,
		float* outDist, D3DXVECTOR3* outPoint, D3DXVECTOR3* outNormal)
	{
		if (outDist)
		{
			*outDist = FLT_MAX;
		}
		if (outPoint)
		{
			*outPoint = { FLT_MAX,FLT_MAX,FLT_MAX };
		}
		if (outNormal)
		{
			*outNormal = { 0,0,0 };
		}

		// 線を境界ボックスの空間へ座標変換
		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv, 0, &boxWorldMtx);
		D3DXVECTOR3 startPt = l.Start;
		D3DXVECTOR3 lineDir = l.End - l.Start;
		D3DXVec3Normalize(&lineDir, &lineDir);
		//線の始点の変換
		D3DXVec3TransformCoord(&startPt, &startPt, &inv);
		//線の方向の変換（移動は要らない）
		inv._41 = 0.0f;
		inv._42 = 0.0f;
		inv._43 = 0.0f;
		D3DXVec3TransformCoord(&lineDir, &lineDir, &inv);

		// 交差判定
		float pt[3], dir[3], min[3], max[3];
		memcpy(pt, &startPt, sizeof(D3DXVECTOR3));
		memcpy(dir, &lineDir, sizeof(D3DXVECTOR3));
		memcpy(min, aabb.Min, sizeof(D3DXVECTOR3));
		memcpy(max, aabb.Max, sizeof(D3DXVECTOR3));

		float distNear = -FLT_MAX;
		float distFar = FLT_MAX;

		//nearPlane[3][2];//nearPlane [ xyzAxis ][ -+Signed ]
		int nearAxis = -1;//x:0, y:1, z:2
		int farAxis = -1;//x:0, y:1, z:2

		for (int axis = 0; axis < 3; ++axis)
		{
			if (fabsf(dir[axis]) < FLT_EPSILON)
			{
				//この軸方向と垂直とみなして、
				//=========================================================
				//　　|			|		|		|		|
				//----|---------|-------|-------|-------|------->軸方向
				//    |			|		|		|		|
				//	  |		   min		|	   max      |
				//  line1			  line2			  line3
				//  交差ない		 交差可能 　　　　交差ない
				//=========================================================
				if (pt[axis] < min[axis] || pt[axis] > max[axis])
				{
					return false; // 交差していない
				}
			}
			else
			{
				// スラブとの距離を算出
				// t1が近スラブ、t2が遠スラブとの距離
				float odd = 1.0f / dir[axis];
				float dist1 = (min[axis] - pt[axis]) * odd;
				float dist2 = (max[axis] - pt[axis]) * odd;
				if (dist1 > dist2)
				{
					float tmp = dist1;
					dist1 = dist2;
					dist2 = tmp;
				}

				if (dist1 > distNear)
				{
					distNear = dist1;
					nearAxis = axis;
				}
				if (dist2 < distFar)
				{
					distFar = dist2;
					farAxis = axis;
				}

				// スラブ交差チェック（スラブ交差ない場合、線とボックス衝突しない）
				//=========================================================
				//                    　		 | 	      far|  ／
				//                    　		 | 	         ■ 	
				//                    　		 | 	      ／ |	
				//                    　		 | 　  ／	 |	　 ／
				//                    　		 |  ／	  far|  ／	
				//        スラブ交差ない -> near ■ 	     ■   	
				//                    -> far  ／ | 	 far  ／ |	
				//-------------------------□--- | ----□----|---------- axis2.max
				//                      ／       | 	／   	 |		   
				//                   ／		near ■ 		 |		   
				//				  ／		  ／ | 			 |		   
				//        near ／     near ／    |           |		   
				//----------□----------□------ | ----------|---------- axis2.min
				//       ／          ／  		 | 			 |	
				//     line1      ／          axis1.min     axis1.max  
				//               line2
				//=========================================================
				if (distNear >= distFar)
				{
					return false;
				}
			}
		}



		// 交差している
		if (outDist)
		{
			if (distNear > 0)
			{
				*outDist = distNear;
			}
			else//始点はボックスの中にある場合
			{
				*outDist = distFar;
			}
		}

		if (outPoint)
		{
			D3DXVECTOR3 lineDir = l.End - l.Start;
			D3DXVec3Normalize(&lineDir, &lineDir);
			*outPoint = l.Start + lineDir * (*outDist);
		}

		if (outNormal)
		{
			D3DXVECTOR3 normal;
			D3DXMATRIX noTranslate = boxWorldMtx;
			noTranslate._41 = 0;
			noTranslate._42 = 0;
			noTranslate._43 = 0;
			int axis = -1;
			if (distNear > 0)
			{
				axis = nearAxis;
			}
			else
			{
				axis = farAxis;
			}
			if (axis == 0)
			{
				D3DXVec3TransformCoord(&normal, &V3_AXIS_X, &noTranslate);
			}
			else if (axis == 1)
			{
				D3DXVec3TransformCoord(&normal, &V3_AXIS_Y, &noTranslate);
			}
			else if (axis == 2)
			{
				D3DXVec3TransformCoord(&normal, &V3_AXIS_Z, &noTranslate);
			}



			if (distNear > 0)
			{
				if (dir[nearAxis] < 0)
				{
					*outNormal = normal;
				}
				else
				{
					*outNormal = -normal;
				}
			}
			else//始点はボックスの中にある場合
			{
				if (dir[farAxis] < 0)
				{
					*outNormal = normal;
				}
				else
				{
					*outNormal = -normal;
				}
			}
		}

		return true;
	}


#pragma endregion
}