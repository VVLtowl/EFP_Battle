#pragma once

#pragma region ========== default ani data ==========
const D3DXVECTOR3 BUTTON_UI_ORIGINAL_POSITION = V3_ZERO;
const D3DXVECTOR3 BUTTON_UI_HIGHLIGHT_MOVE = { 0,0.02f,0 };
const D3DXVECTOR3 BUTTON_UI_CLICK_MOVE = { 0,-0.02f,0 };

const D3DXVECTOR3 BUTTON_UI_ORIGINAL_SCALE = V3_ONE;
const float BUTTON_UI_HIGHLIGHT_SCALE = 1.2f;
const float BUTTON_UI_CLICK_SCALE = 0.9f;

const D3DXVECTOR4 BUTTON_UI_DISABLE_COLOR = { 0.6f,0.6f,0.6f, 0.6f };
const D3DXVECTOR4 BUTTON_UI_ORIGINAL_COLOR = V4_ONE;
const D3DXVECTOR4 BUTTON_UI_HIGHLIGHT_COLOR = { 0.1f,1,0.05f,1 };
const D3DXVECTOR4 BUTTON_UI_CLICK_COLOR = { 0.8f,0.5f,0.5f,1 };
#pragma endregion

struct ButtonAnimeDescripition
{
	D3DXVECTOR3 OriginalUIPosition = BUTTON_UI_ORIGINAL_POSITION;
	D3DXVECTOR3 HighlightUIPosition = BUTTON_UI_ORIGINAL_POSITION + BUTTON_UI_HIGHLIGHT_MOVE;
	D3DXVECTOR3 ClickUIPosition = BUTTON_UI_ORIGINAL_POSITION + BUTTON_UI_CLICK_MOVE;

	D3DXVECTOR3 OriginalUIScale = BUTTON_UI_ORIGINAL_SCALE;
	D3DXVECTOR3 HighlightUIScale = BUTTON_UI_ORIGINAL_SCALE * BUTTON_UI_HIGHLIGHT_SCALE;
	D3DXVECTOR3 ClickUIScale = BUTTON_UI_ORIGINAL_SCALE * BUTTON_UI_CLICK_SCALE;

	D3DXVECTOR4 DisableUIColor = BUTTON_UI_DISABLE_COLOR;
	D3DXVECTOR4 OriginalUIColor = BUTTON_UI_ORIGINAL_COLOR;
	D3DXVECTOR4 HighlightUIColor = BUTTON_UI_HIGHLIGHT_COLOR;
	D3DXVECTOR4 ClickUIColor = BUTTON_UI_CLICK_COLOR;
};

class ButtonHelper
{
public:
	ButtonHelper(
		class GameObject* uiObject,
		class Button* btn,
		class Polygon2D* uiPolygon);
	~ButtonHelper();

public:
	class GameObject* UIObject;
	class Button* UIButton;
	class Polygon2D* UIPolygon2D;
	class Animator* TargetAnimator;

public:
	void InitButtonAnime();
	void StartUIAppear();
	void StartUIDisappear();
};

