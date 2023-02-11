#pragma once
#include <list>

/*********************************************************
* @brief	ライト　ヘッダー
********************************************************/
#include "MyLight.h"

class LightManager
{
private:
	static MyLight* m_MainShadowLight;

public:
	static void SetMainLight(MyLight* light);
	static MyLight* GetMainLight();
	static void SelectMainLightView();
};
