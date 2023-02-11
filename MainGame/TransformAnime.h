#pragma once
#include "Component.h"
#include <vector>
#include <functional>

#pragma region ========== old version ==========
/*********************************************************
* @brief    各フレームのデータの計算関数
********************************************************/
class ComputeFrameTransformDataFunc
{
public:
    //class TransformAnime* TargetTransformAnime;

    D3DXVECTOR3		StartPosition;
    D3DXQUATERNION	StartRotation;
    D3DXVECTOR3		StartScale;

    D3DXVECTOR3		EndPosition;
    D3DXQUATERNION	EndRotation;
    D3DXVECTOR3		EndScale;

    D3DXVECTOR3		NowPosition;
    D3DXQUATERNION	NowRotation;
    D3DXVECTOR3		NowScale;

public:
    virtual void operator()(class TransformAnime* ani) = 0;
protected:
    void InitData(class TransformAnime* ani);
};

class UniformSpeedAnime :
    public ComputeFrameTransformDataFunc
{
public:
    void operator()(class TransformAnime* ani);
};

class SlowLerpAnime :
    public ComputeFrameTransformDataFunc
{
public:
    void operator()(class TransformAnime* ani);
};

class HermiteCurveAnime :
    public ComputeFrameTransformDataFunc
{
public:
    HermiteCurveAnime(
        const D3DXVECTOR3& startTang,
        const D3DXVECTOR3& endTang
    );
    void operator()(TransformAnime* ani);

private:
    D3DXVECTOR3 m_StartTangent;
    D3DXVECTOR3 m_EndTangent;
};

class SineMoveAnime :
    public ComputeFrameTransformDataFunc
{
public:
    SineMoveAnime(
        const D3DXVECTOR3& minPos,
        const D3DXVECTOR3& maxPos,
        float min = -PI,
        float max = PI
    );
    void operator()(class TransformAnime* ani);

private:
    D3DXVECTOR3 m_MinPosition;
    D3DXVECTOR3 m_MaxPosition;
    float m_XMin;
    float m_XMax;
};

class RotateMoveAnime :
    public ComputeFrameTransformDataFunc
{
public:
    RotateMoveAnime(
        const D3DXVECTOR3& center,
        float radius,
        float startRadian,
        float endRadian);
    void operator()(class TransformAnime* ani);

private:
    D3DXVECTOR3 m_Center;
    float m_Radius;
    float m_StartRadian;
    float m_EndRadian;
};



/*********************************************************
* @brief    変換アニメの情報
********************************************************/
struct TransformAnimeDescripition
{
    D3DXVECTOR3 StartPosition = V3_ZERO;
    D3DXQUATERNION StartRotation = QUAT_IDENTITY;
    D3DXVECTOR3 StartScale = V3_ONE;
    D3DXVECTOR3 EndPosition = V3_ZERO;
    D3DXQUATERNION EndRotation = QUAT_IDENTITY;
    D3DXVECTOR3 EndScale = V3_ONE;

    int LoopCount = 1;//INT_MAX:infinity loop
    float Duration = 60;

    SlowLerpAnime _computeFunc;
    ComputeFrameTransformDataFunc* ComputeAniDataFunc = &_computeFunc;
    std::function<void()> EndEvent = []() {; };



    TransformAnimeDescripition() {};
    TransformAnimeDescripition(
        D3DXVECTOR3 startPos,
        D3DXVECTOR3 endPos)
    {
        StartPosition = startPos;
        EndPosition = endPos;
    };
    TransformAnimeDescripition(
        class Transform3D* start,
        class Transform3D* end)
    {
        if (start != nullptr)
        {
            StartPosition = start->GetPosition();
            StartRotation = start->GetRotation();
            StartScale = start->GetScale();
        }

        if (end != nullptr)
        {
            EndPosition = end->GetPosition();
            EndRotation = end->GetRotation();
            EndScale = end->GetScale();
        }
    }
};

class TransformAnime :
    public Component
{
public:
    TransformAnime(
        class GameObject* owner,
        TransformAnimeDescripition desc,
        int order = COMP_TRANSFORMANIME);
    ~TransformAnime();
    void Update() override;

private:
    std::function<void()> m_EndEvent;

public:
    //D3DXVECTOR3 m_StartPosition;
    //D3DXQUATERNION m_StartRotation;
    //D3DXVECTOR3 m_StartScale;
    //D3DXVECTOR3 m_EndPosition;
    //D3DXQUATERNION m_EndRotation;
    //D3DXVECTOR3 m_EndScale;

    //D3DXVECTOR3 m_NowPosition;
    //D3DXQUATERNION m_NowRotation;
    //D3DXVECTOR3 m_NowScale;

    int m_LoopCount;
    const float m_Duration;
    float m_FrameCount;

    // @brief   生成する時全フレームのデータを計算して生成
    std::vector<D3DXVECTOR3> m_AnimePositions;
    std::vector<D3DXQUATERNION> m_AnimeRotations;
    std::vector<D3DXVECTOR3> m_AnimeScales;
};
#pragma endregion

#pragma region ========== new version ==========


/*********************************************************
* @brief    component
********************************************************/
class Animator :
    public Component
{
public:
    Animator(
        class GameObject* owner,
        int order = COMP_TRANSFORMANIME);
    ~Animator();
    void Update() override;

private:
    std::list<class Anime*> m_Animes;
    std::list<class Anime*> m_TrashAnimes;
    std::function<void()> m_EndEvent;

public:
    void AddEndEvent(std::function<void()> func) { m_EndEvent = func; };
    void AddAnime(class Anime* ani);
    void RemoveAnime(class Anime* ani);
    void AnimeFinsh(class Anime* ani);
};

/*********************************************************
* @brief    anime descripition
********************************************************/
//base
class AnimeDescripition
{
public:
    int LoopCount = 1;
    float Duration = 60;
};
class AniDesc_Vec3StartEnd :
    public AnimeDescripition
{
public:
    D3DXVECTOR3 Start;
    D3DXVECTOR3 End;
};
class AniDesc_QuaternionStartEnd :
    public AnimeDescripition
{
public:
    D3DXQUATERNION Start;
    D3DXQUATERNION End;
};
class AniDesc_PositionCircleRotate :
    public AnimeDescripition
{
public:
    D3DXVECTOR3 Center;
    float Radius;
    float StartRadian;
    float EndRadian;
};
class AniDesc_Vec3Hermite :
    public AnimeDescripition
{
public:
    D3DXVECTOR3 StartVec3;
    D3DXVECTOR3 StartTangent;
    D3DXVECTOR3 EndVec3;
    D3DXVECTOR3 EndTangent;
};
class AniDesc_Texture:
    public AnimeDescripition
{
public:
    D3DXVECTOR4 StartTexcoord;
    int ColumnNum;
    int RowNum;
    int ClipNum;
};

class AniDesc_Event :
    public AnimeDescripition
{
public:
    std::function<void()> Event;
    float EventFrame;
};

/*********************************************************
* @brief    anime
********************************************************/
class Anime
{
public:
    Anime(
        class Animator* animator, 
        const AnimeDescripition& desc);
    void Update();
    ~Anime();
private:
    virtual void AnimeUpdate() = 0;
public:
    int LoopCount;
    const float Duration;

    float FrameMax;
    float FrameCount;
public:
    class GameObject* TargetObject;
    class Animator* OwnAnimator;
};

//base
class Vector3Anime :
    public Anime
{
public:
    Vector3Anime(
        class Animator* animator,
        const AnimeDescripition& desc,
        class ComputeFunction& computeFunc);
    //virtual void AnimeUpdate()override;
public:
    std::vector<D3DXVECTOR3> Vector3Datas;
};
class Anime_Position :
    public Vector3Anime
{
public:
    Anime_Position(
        class Animator* animator,
        const AnimeDescripition& desc,
        class ComputeFunction& computeFunc) :
        Vector3Anime(animator, desc, computeFunc) {};
    void AnimeUpdate()override;
};
class Anime_Scale :
    public Vector3Anime
{
public:
    Anime_Scale(
        class Animator* animator,
        const AnimeDescripition& desc,
        class ComputeFunction& computeFunc) :
        Vector3Anime(animator, desc, computeFunc) {};
    void AnimeUpdate()override;
};

//base
class QuaternionAnime :
    public Anime
{
public:
    QuaternionAnime(
        class Animator* animator,
        const AnimeDescripition& desc,
        class ComputeFunction& computeFunc);
    //void AnimeUpdate()override;
public:
    std::vector<D3DXQUATERNION> QuaternionDatas;
};
class Anime_Rotation :
    public QuaternionAnime
{
public:
    Anime_Rotation(
        class Animator* animator,
        const AnimeDescripition& desc,
        class ComputeFunction& computeFunc) :
        QuaternionAnime(animator, desc, computeFunc) {};
    void AnimeUpdate()override;
};

class Anime_Texture :
    public Anime
{
public:
    Anime_Texture(
        class Animator* animator,
        const AniDesc_Texture& desc,
        class DrawObject* drawObj);
    
public:
    void AnimeUpdate()override;

public:
    class DrawObject* TargetDrawObj;
    float PerClipDuration=0;
    std::vector<D3DXVECTOR4> ClipDatas;
};

class Anime_Event :
    public Anime
{
public:
    Anime_Event(
        class Animator* animator,
        const AniDesc_Event& desc);

public:
    void AnimeUpdate()override;

private:
    float EventFrame;
    std::function<void()> Event;
};


/*********************************************************
* @brief    compute func
********************************************************/
class ComputeFunction
{
public:
    virtual void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc) = 0;
};

class ComputeUniformVec3 :
    public ComputeFunction
{
public:
    void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc)override;
};

class ComputeSlowLerpVec3 :
    public ComputeFunction
{
public:
    void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc)override;
};

class ComputeUniformQuaternion :
    public ComputeFunction
{
public:
    void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc)override;
};

class ComputeSlowStartSlowEndVec3 :
    public ComputeFunction
{
public:
    void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc)override;
};

class ComputePositionCircleRotate :
    public ComputeFunction
{
public:
    void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc)override;
};

class ComputeHermiteVec3 :
    public ComputeFunction
{
public:
    void operator()(
        Anime* anime,
        const AnimeDescripition& aniDesc)override;
};
#pragma endregion


