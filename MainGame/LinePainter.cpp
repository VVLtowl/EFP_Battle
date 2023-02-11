#include "main.h"
#include "LinePainter.h"

LinePainter::LinePainter()
{
	Polygon2DDescription desc;
	desc.DrawObjDesc.Layer = LAYER_WORLD_LINE_TOOL;
	desc.DrawObjDesc.DepthEnable = false;
	desc.TextureID = TEXID_FIELD004;
	desc.Size = { 2,2 };
	//new Polygon2D(this, desc);
}

LinePainter::~LinePainter()
{
}

void LinePainter::UpdateGameObject()
{
}

void LinePainter::MakeLine(DrawLineDescription desc)
{
	DrawLine* dl = new DrawLine(this, desc);
	m_DrawLines.emplace_back(dl);
}
