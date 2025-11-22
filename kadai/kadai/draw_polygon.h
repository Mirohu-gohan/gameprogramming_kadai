#pragma once
#include "Dx12.h"
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

Vertex triangleVertices[] =
{
	{{0.0f,0.5f,0.0f},{1.0f,0.0f,0.0f,1.0f}},
	{{0.5f,-0.5f,0.0f},{0.0f,1.0f,0.0f,1.0f}},
	{{-0.5f,-0.5,0.0f},{0.0f,0.0f,1.0f,1.0f}}
};



