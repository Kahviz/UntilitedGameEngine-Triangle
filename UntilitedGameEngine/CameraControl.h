#pragma once
#include "Window.h"
#include "Graphics.h"
#include "Mouse.h"
#include "Camera.h"

class CameraControl
{
public:
	void MakeCameraControls(Window& wnd, float deltaTime);
};