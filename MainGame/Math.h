#pragma once

const float PI = 3.1415926f;
const float PI_HALF = 1.570796251f;

const D3DXVECTOR2 V2_ZERO = { 0,0 };
const D3DXVECTOR2 V2_ONE = { 1,1 };

const D3DXVECTOR3 V3_ZERO = { 0,0,0 };
const D3DXVECTOR3 V3_ONE = { 1,1,1 };
const D3DXVECTOR3 V3_AXIS_X = { 1,0,0 };
const D3DXVECTOR3 V3_AXIS_Y = { 0,1,0 };
const D3DXVECTOR3 V3_AXIS_Z = { 0,0,1 };

const D3DXVECTOR4 V4_ZERO = { 0,0,0,0 };
const D3DXVECTOR4 V4_ONE = { 1,1,1,1 };

const D3DXQUATERNION QUAT_IDENTITY = { 0,0,0,1 };

const D3DXMATRIX MTX_NOTRANSFORM=
{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1,
};