#include "main.h"
#include "TransformAnime.h"

// @brief	slow start slow end
auto g = [&](float x)
{
	float y = 1 - powf((fabsf(x) / (PI / 2.0f)), 0.4f);
	return y;
};
auto h = [&](float x)
{
	float sign = 0;
	if (x > 0)
	{
		sign = 1;
	}
	else if (x < 0)
	{
		sign = -1;
	}
	else if (x == 0)
	{
		sign = 0;
	}

	float y = sign * powf(fabsf(sinf(x)), g(x));
	return y;
};

#pragma region ========== usual anime ==========


void ComputeFrameTransformDataFunc::InitData(TransformAnime* ani)
{
	StartPosition = ani->m_AnimePositions[1];
	StartRotation = ani->m_AnimeRotations[1];
	StartScale = ani->m_AnimeScales[1];

	EndPosition = ani->m_AnimePositions[0];
	EndRotation = ani->m_AnimeRotations[0];
	EndScale = ani->m_AnimeScales[0];

	NowPosition = StartPosition;
	NowRotation = StartRotation;
	NowScale = StartScale;
}

void UniformSpeedAnime::operator()(TransformAnime* ani)
{
	InitData(ani);
	float ratio = 0;
	float frameMax = ani->m_Duration;
	for (float frameID = 0; frameID < frameMax; frameID += FRAME_STRIDE)
	{
		ratio = frameID / frameMax;

		D3DXVec3Lerp(&NowPosition, &StartPosition, &EndPosition, ratio);
		D3DXQuaternionSlerp(&NowRotation, &StartRotation, &EndRotation, ratio);
		D3DXVec3Lerp(&NowScale, &StartScale, &EndScale, ratio);

		ani->m_AnimePositions.emplace_back(NowPosition);
		ani->m_AnimeRotations.emplace_back(NowRotation);
		ani->m_AnimeScales.emplace_back(NowScale);
	}
}


void SlowLerpAnime::operator()(TransformAnime* ani)
{
	InitData(ani);
	float ratio = 0;
	float frameMax = ani->m_Duration;
	for (float frameID = 0; frameID < frameMax; frameID += FRAME_STRIDE)
	{
		ratio = frameID / frameMax;

		D3DXVec3Lerp(&NowPosition, &NowPosition, &EndPosition, ratio);
		D3DXQuaternionSlerp(&NowRotation, &NowRotation, &EndRotation, ratio);
		D3DXVec3Lerp(&NowScale, &NowScale, &EndScale, ratio);

		ani->m_AnimePositions.emplace_back(NowPosition);
		ani->m_AnimeRotations.emplace_back(NowRotation);
		ani->m_AnimeScales.emplace_back(NowScale);
	}
}

HermiteCurveAnime::HermiteCurveAnime(
	const D3DXVECTOR3& startTang,
	const D3DXVECTOR3& endTang) :
	m_StartTangent(startTang),
	m_EndTangent(endTang)
{
}

void HermiteCurveAnime::operator()(TransformAnime* ani)
{
	//compute frame data
	{
		float H00 = 0, H01 = 0, H10 = 0, H11 = 0;
		float t = 0, x = 0;

		InitData(ani);
		float frameMax = ani->m_Duration;
		for (float frameID = 0; frameID < frameMax; frameID += FRAME_STRIDE)
		{
			// sin(x),[-pi/2,pi/2]
			x = frameID / frameMax * PI - PI * 0.5f;
			t = (h(x) + 1.0f) / 2.0f;
			//t = (sinf(x) + 1.0f) / 2.0f;
			//t = frameID / ani->m_Duration;
			//t = powf(t, 0.25f);

			H00 = 2 * t * t * t - 3 * t * t + 1;
			H01 = -2 * t * t * t + 3 * t * t;
			H10 = t * t * t - 2 * t * t + t;
			H11 = t * t * t - t * t;

			// “_‚ÌÀ•W‚ð‹‚ßŠi”[
			NowPosition =
				H00 * StartPosition +
				H01 * EndPosition +
				H10 * m_StartTangent +
				H11 * m_EndTangent;


			ani->m_AnimePositions.emplace_back(NowPosition);
			ani->m_AnimeRotations.emplace_back(NowRotation);
			ani->m_AnimeScales.emplace_back(NowScale);
		}
	}
}

SineMoveAnime::SineMoveAnime(
	const D3DXVECTOR3& minPos,
	const D3DXVECTOR3& maxPos,
	float min,
	float max) :
	m_MinPosition(minPos),
	m_MaxPosition(maxPos),
	m_XMin(min),
	m_XMax(max)
{
}

void SineMoveAnime::operator()(TransformAnime* ani)
{
	InitData(ani);
	ani->m_AnimePositions[0] = m_MinPosition;

	float xDist = m_XMax - m_XMin;
	float x = 0;
	float ratio = 0;
	float frameMax = ani->m_Duration;
	for (float frameID = 0; frameID < frameMax; frameID += FRAME_STRIDE)
	{
		x = m_XMin + frameID / frameMax * xDist;
		ratio = (sinf(x) + 1.0f) / 2.0f;

		D3DXVec3Lerp(&NowPosition, &m_MinPosition, &m_MaxPosition, ratio);
		D3DXQuaternionSlerp(&NowRotation, &StartRotation, &EndRotation, ratio);
		D3DXVec3Lerp(&NowScale, &StartScale, &EndScale, ratio);

		ani->m_AnimePositions.emplace_back(NowPosition);
		ani->m_AnimeRotations.emplace_back(NowRotation);
		ani->m_AnimeScales.emplace_back(NowScale);
	}
}

RotateMoveAnime::RotateMoveAnime(
	const D3DXVECTOR3& center,
	float radius,
	float startRadian,
	float endRadian) :
	m_Center(center),
	m_Radius(radius),
	m_StartRadian(startRadian),
	m_EndRadian(endRadian)
{
}

void RotateMoveAnime::operator()(TransformAnime* ani)
{
	float radian = m_EndRadian;
	float posX = cosf(radian) * m_Radius;
	float posY = sinf(radian) * m_Radius;

	InitData(ani);
	ani->m_AnimePositions[0] = { posX,posY,0 };
	float x = 0;
	float ratio = 0;
	float radianDist = m_EndRadian - m_StartRadian;
	float frameMax = ani->m_Duration;
	for (float frameID = 0; frameID < frameMax; frameID += FRAME_STRIDE)
	{
		// sin(x),[-pi/2,pi/2]
		x = frameID / frameMax * PI - PI * 0.5f;
		ratio = (h(x) + 1.0f) / 2.0f;
		radian = m_StartRadian + ratio * radianDist;
		posX = cosf(radian) * m_Radius;
		posY = sinf(radian) * m_Radius;

		NowPosition = { posX,posY,0 };
		D3DXQuaternionSlerp(&NowRotation, &StartRotation, &EndRotation, ratio);
		D3DXVec3Lerp(&NowScale, &StartScale, &EndScale, ratio);

		ani->m_AnimePositions.emplace_back(NowPosition);
		ani->m_AnimeRotations.emplace_back(NowRotation);
		ani->m_AnimeScales.emplace_back(NowScale);
	}
}
#pragma endregion

#pragma region ========== old version ==========

TransformAnime::TransformAnime(
	GameObject* owner,
	TransformAnimeDescripition desc,
	int order) :
	Component(owner, order),
	m_LoopCount(desc.LoopCount),
	m_Duration((float)desc.Duration),
	m_EndEvent(desc.EndEvent),
	m_FrameCount(0)
{
	//set end data
	//index 0 is end
	m_AnimePositions.emplace_back(desc.EndPosition);
	m_AnimeRotations.emplace_back(desc.EndRotation);
	m_AnimeScales.emplace_back(desc.EndScale);

	//set start data
	//index 1 is start
	m_AnimePositions.emplace_back(desc.StartPosition);
	m_AnimeRotations.emplace_back(desc.StartRotation);
	m_AnimeScales.emplace_back(desc.StartScale);

	//æŒvŽZ
	(*desc.ComputeAniDataFunc)(this);
}

TransformAnime::~TransformAnime()
{
}

void TransformAnime::Update()
{
	if (m_LoopCount > 0)
	{
		if (m_Duration - m_FrameCount > 0)
		{
			float ratio = m_FrameCount / m_Duration;

			//set owner transform
			int frameID = (int)(m_FrameCount / FRAME_STRIDE) + 1;
			m_Owner->GetTransform()->SetPosition(m_AnimePositions[frameID]);
			m_Owner->GetTransform()->SetRotation(m_AnimeRotations[frameID]);
			m_Owner->GetTransform()->SetScale(m_AnimeScales[frameID]);
			m_Owner->GetTransform()->UpdateTransform();
			m_Owner->GetTransform()->DisableUpdateThisFrame();

			m_FrameCount += FRAME_STRIDE;
		}
		else
		{
			m_LoopCount--;
			m_FrameCount = 0;

			m_Owner->GetTransform()->SetPosition(m_AnimePositions[0]);
			m_Owner->GetTransform()->SetRotation(m_AnimeRotations[0]);
			m_Owner->GetTransform()->SetScale(m_AnimeScales[0]);
			m_Owner->GetTransform()->UpdateTransform();
			m_Owner->GetTransform()->DisableUpdateThisFrame();
		}
	}
	else
	{
		m_Owner->GetTransform()->SetPosition(m_AnimePositions[0]);
		m_Owner->GetTransform()->SetRotation(m_AnimeRotations[0]);
		m_Owner->GetTransform()->SetScale(m_AnimeScales[0]);
		m_Owner->GetTransform()->UpdateTransform();
		m_Owner->GetTransform()->DisableUpdateThisFrame();

		m_EndEvent();
		SetState(DEAD);
	}
}
#pragma endregion




/*********************************************************
* @brief    animator component
********************************************************/
Animator::Animator(
	GameObject* owner,
	int order):
	Component(owner,order),
	m_EndEvent([]() {; })
{
}

Animator::~Animator()
{
	while (!m_Animes.empty())
	{
		delete m_Animes.back();
	}
}

void Animator::Update()
{
	for (auto anime : m_Animes)
	{
		anime->Update();
	}

	while (!m_TrashAnimes.empty())
	{
		auto finishedAni = m_TrashAnimes.begin();
		delete *finishedAni;
		m_TrashAnimes.erase(finishedAni);
	}

	if (m_Animes.empty())
	{
		m_EndEvent();
		SetState(Component::DEAD);
	}
}

void Animator::AddAnime(Anime* ani)
{
	m_Animes.emplace_back(ani);
}

void Animator::RemoveAnime(Anime* ani)
{
	m_Animes.remove(ani);
}

void Animator::AnimeFinsh(Anime* ani)
{
	m_TrashAnimes.emplace_back(ani);
}

/*********************************************************
* @brief    anime
********************************************************/
Anime::Anime(Animator* animator, const AnimeDescripition& desc) :
	OwnAnimator(animator),
	TargetObject(animator->GetGameObject()),
	LoopCount(desc.LoopCount),
	Duration(desc.Duration),
	FrameMax(desc.Duration/FRAME_STRIDE),
	FrameCount(0)
{
	OwnAnimator->AddAnime(this);
}

void Anime::Update()
{
	if (LoopCount == 0)
	{
		//LoopCount--;
		//OwnAnimator->AnimeFinsh();
		//delete this;
		OwnAnimator->AnimeFinsh(this);
	}
	else
	{
		AnimeUpdate();

		if (FrameMax > FrameCount)
		{
			FrameCount += FRAME_STRIDE;
		}

		if (FrameMax <= FrameCount)
		{
			LoopCount--;
			FrameCount = 0;
		}
	}
}

Anime::~Anime()
{
	OwnAnimator->RemoveAnime(this);
	//TargetObject = nullptr;
	//OwnAnimator = nullptr;
}

Vector3Anime::Vector3Anime(Animator* animator, const AnimeDescripition& desc, ComputeFunction& computeFunc) :
	Anime(animator, desc)
{
	computeFunc(this, desc);
}
void Anime_Position::AnimeUpdate()
{
	TargetObject->GetTransform()->SetPosition(Vector3Datas[(int)FrameCount]);
	TargetObject->GetTransform()->UpdateTransform();
	TargetObject->GetTransform()->DisableUpdateThisFrame();
}
void Anime_Scale::AnimeUpdate()
{
	TargetObject->GetTransform()->SetScale(Vector3Datas[(int)FrameCount]);
	TargetObject->GetTransform()->UpdateTransform();
	TargetObject->GetTransform()->DisableUpdateThisFrame();
}

QuaternionAnime::QuaternionAnime(Animator* animator, const AnimeDescripition& desc, ComputeFunction& computeFunc):
	Anime(animator,desc)
{
	computeFunc(this, desc);
}
void Anime_Rotation::AnimeUpdate()
{
	TargetObject->GetTransform()->SetRotation(QuaternionDatas[(int)FrameCount]);
	TargetObject->GetTransform()->UpdateTransform();
	TargetObject->GetTransform()->DisableUpdateThisFrame();
}

Anime_Texture::Anime_Texture(
	Animator* animator,
	const AniDesc_Texture& aniDesc,
	DrawObject* drawObj) :
	Anime(animator, aniDesc),
	TargetDrawObj(drawObj)
{
	//compute 
	{
		AniDesc_Texture& desc = (AniDesc_Texture&)aniDesc;

		int clipNum = desc.ClipNum; 
		float tx = desc.StartTexcoord.x;
		float ty = desc.StartTexcoord.y;
		float w = desc.StartTexcoord.z;
		float h = desc.StartTexcoord.w;

		//compute play speed
		{
			PerClipDuration = desc.Duration / (float)clipNum;
		}

		//compute tex clip datas
		{
			int clipCount = 0;
			for (int row = 0; row < desc.RowNum; row++)
			{
				for (int column = 0; column < desc.ColumnNum; column++)
				{
					D3DXVECTOR4 data = { 
						tx + column * w,
						ty + row * h,
						w,
						h };
					ClipDatas.emplace_back(data);
					
					clipCount++;
					if (clipCount >= clipNum)
					{
						break;
					}
				}
			}
		}

		//set frame max
		FrameMax = desc.Duration;
	}
}

void Anime_Texture::AnimeUpdate()
{
	int id = (int)FrameCount / (int)PerClipDuration;
	id %= ClipDatas.size();
	TargetDrawObj->Texcoord = ClipDatas[id];
}


Anime_Event::Anime_Event(
	Animator* animator,
	const AniDesc_Event& desc):
	Anime(animator, desc),
	EventFrame(desc.EventFrame),
	Event(desc.Event)
{
	if (desc.EventFrame >= desc.Duration)
	{
		EventFrame = desc.Duration;
	}
}

void Anime_Event::AnimeUpdate()
{
	if (FrameCount >= EventFrame)
	{
		Event();
	}
}

/*********************************************************
* @brief    compute func
********************************************************/
void ComputeUniformVec3::operator()(Anime* anime, const AnimeDescripition& aniDesc)
{
	Vector3Anime* ani = (Vector3Anime*)anime;
	AniDesc_Vec3StartEnd& desc = (AniDesc_Vec3StartEnd&)aniDesc;

	//compute data
	{
		D3DXVECTOR3 pos;
		float ratio = 0;
		const float frameMax = desc.Duration;
		for (float frameCount = 0; frameCount < frameMax; frameCount += FRAME_STRIDE)
		{
			ratio = frameCount / frameMax;
			D3DXVec3Lerp(&pos, &desc.Start, &desc.End, ratio);
			ani->Vector3Datas.emplace_back(pos);
		}
	}

	//set end data
	ani->Vector3Datas.emplace_back(desc.End);

	//set frame max
	ani->FrameMax = (float)ani->Vector3Datas.size();
}

void ComputeSlowLerpVec3::operator()(Anime* anime, const AnimeDescripition& aniDesc)
{
	Vector3Anime* ani = (Vector3Anime*)anime;
	AniDesc_Vec3StartEnd& desc = (AniDesc_Vec3StartEnd&)aniDesc;

	//compute data
	{
		float ratio = 0;
		D3DXVECTOR3 pos = desc.Start;
		const float frameMax = desc.Duration;
		for (float frameCount = 0; frameCount < frameMax; frameCount += FRAME_STRIDE)
		{
			ratio = frameCount / frameMax;
			D3DXVec3Lerp(&pos, &pos, &desc.End, ratio);
			ani->Vector3Datas.emplace_back(pos);
		}
	}

	//set end data
	ani->Vector3Datas.emplace_back(desc.End);

	//set frame max
	ani->FrameMax = (float)ani->Vector3Datas.size();
}

void ComputeUniformQuaternion::operator()(Anime* anime, const AnimeDescripition& aniDesc)
{
	QuaternionAnime* ani = (QuaternionAnime*)anime;
	AniDesc_QuaternionStartEnd& desc = (AniDesc_QuaternionStartEnd&)aniDesc;

	//compute data
	{
		D3DXQUATERNION rot;
		float ratio = 0;
		const float frameMax = desc.Duration;
		for (float frameCount = 0; frameCount < frameMax; frameCount += FRAME_STRIDE)
		{
			ratio = frameCount / frameMax;
			D3DXQuaternionSlerp(&rot, &desc.Start, &desc.End, ratio);
			ani->QuaternionDatas.emplace_back(rot);
		}
	}

	//set end data
	ani->QuaternionDatas.emplace_back(desc.End);

	//set frame max
	ani->FrameMax = (float)ani->QuaternionDatas.size();
}

void ComputeSlowStartSlowEndVec3::operator()(Anime* anime, const AnimeDescripition& aniDesc)
{
	Vector3Anime* ani = (Vector3Anime*)anime;
	AniDesc_Vec3StartEnd& desc = (AniDesc_Vec3StartEnd&)aniDesc;

	//compute data
	{
		D3DXVECTOR3 pos;
		float x = 0;
		float ratio = 0;
		const float frameMax = desc.Duration;
		for (float frameCount = 0; frameCount < frameMax; frameCount += FRAME_STRIDE)
		{
			x = frameCount / frameMax * PI - PI_HALF;
			ratio = (h(x) + 1) * 0.5f;
			D3DXVec3Lerp(&pos, &desc.Start, &desc.End, ratio);
			ani->Vector3Datas.emplace_back(pos);
		}
	}

	//set end data
	ani->Vector3Datas.emplace_back(desc.End);

	//set frame max
	ani->FrameMax = (float)ani->Vector3Datas.size();
}

void ComputePositionCircleRotate::operator()(Anime* anime, const AnimeDescripition& aniDesc)
{
	Vector3Anime* ani = (Vector3Anime*)anime;
	AniDesc_PositionCircleRotate& desc = (AniDesc_PositionCircleRotate&)aniDesc;

	//compute data
	{
		const float radius = desc.Radius;
		const float startRadian = desc.StartRadian;
		const float radianDist = desc.EndRadian - desc.StartRadian;

		D3DXVECTOR3 pos;
		float x = 0;
		float ratio = 0;
		float radian = 0;
		float posX = 0;
		float posY = 0;
		const float frameMax = desc.Duration;
		for (float frameCount = 0; frameCount < frameMax; frameCount += FRAME_STRIDE)
		{
			x = frameCount / frameMax * PI - PI_HALF;
			ratio = (h(x) + 1) * 0.5f;
			radian = startRadian + ratio * radianDist;
			posX = cosf(radian) * radius;
			posY = sinf(radian) * radius;

			pos = { posX,posY,0 };
			ani->Vector3Datas.emplace_back(pos);
		}

		//set end data
		radian = desc.EndRadian;
		posX = cosf(radian) * radius;
		posY = sinf(radian) * radius;
		pos = { posX,posY,0 };
		ani->Vector3Datas.emplace_back(pos);
	}

	//set frame max
	ani->FrameMax = (float)ani->Vector3Datas.size();
}

void ComputeHermiteVec3::operator()(Anime* anime, const AnimeDescripition& aniDesc)
{
	Vector3Anime* ani = (Vector3Anime*)anime;
	AniDesc_Vec3Hermite& desc = (AniDesc_Vec3Hermite&)aniDesc;

	//compute frame data
	{
		float H00 = 0, H01 = 0, H10 = 0, H11 = 0;
		float t = 0, x = 0;

		D3DXVECTOR3 pos;
		const float frameMax = desc.Duration;
		for (float frameCount = 0; frameCount < frameMax; frameCount += FRAME_STRIDE)
		{
			////sin(x),[-pi/2,pi/2]
			//x = frameCount / frameMax * PI - PI_HALF;
			//t = (h(x) + 1.0f) * 0.5f;
			
			t = frameCount / frameMax;

			H00 = 2 * t * t * t - 3 * t * t + 1;
			H01 = -2 * t * t * t + 3 * t * t;
			H10 = t * t * t - 2 * t * t + t;
			H11 = t * t * t - t * t;

			// “_‚ÌÀ•W‚ð‹‚ßŠi”[
			pos =
				H00 * desc.StartVec3 +
				H01 * desc.EndVec3 +
				H10 * desc.StartTangent +
				H11 * desc.EndTangent;

			ani->Vector3Datas.emplace_back(pos);
		}
	}

	//set end data
	ani->Vector3Datas.emplace_back(desc.EndVec3);

	//set frame max
	ani->FrameMax = (float)ani->Vector3Datas.size();
}

