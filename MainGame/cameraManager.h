#pragma once
#include <list>

/*********************************************************
* @brief	カメラ　ヘッダー
********************************************************/
#include "Camera.h"

class CameraManager
{
private:
	static Camera* m_MainCamera;
	static std::list<Camera*> m_Cameras;

public:
	static void SetMainCamera(Camera* cmr);
	static Camera* GetMainCamera();
	static void SelectMainCameraView();

	static void Register(Camera* cmr);
	static void Quit(Camera* cmr);
	static void Init();
	static void UpdateCameraDraw();
};
