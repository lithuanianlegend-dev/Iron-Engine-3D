#pragma once

#include <iostream>

struct RenderableComponent {
	uint32_t    meshID = 0;
	uint32_t    shaderID = 0;
	uint32_t    textureID = 0;
	bool		visible = true;
};