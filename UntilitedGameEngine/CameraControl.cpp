#include "CameraControl.h"
#include <string>

void CameraControl::MakeCameraControls(Window& wnd, float deltaTime)
{
    float speed = 5.0f * deltaTime;
    float sensitivity = 0.002f; // hiiren herkkyys

    Camera& Cam = wnd.Gfx().GetCamera(); // viite, ei kopiota
    XMFLOAT3 forward = Cam.GetForward();
    XMFLOAT3 right = Cam.GetRight();

    if (wnd.kbd.KeyIsPressed(0x41)) // A
        Cam.AdjustPosition(-right.x * speed, -right.y * speed, -right.z * speed);
    if (wnd.kbd.KeyIsPressed(0x44)) // D
        Cam.AdjustPosition(right.x * speed, right.y * speed, right.z * speed);
    if (wnd.kbd.KeyIsPressed(0x53)) // S
        Cam.AdjustPosition(-forward.x * speed, -forward.y * speed, -forward.z * speed);
    if (wnd.kbd.KeyIsPressed(0x57)) // W
        Cam.AdjustPosition(forward.x * speed, forward.y * speed, forward.z * speed);

    if (wnd.kbd.KeyIsPressed(0x51)) // Q
        Cam.AdjustPosition(0.0f, -speed, 0.0f);
    if (wnd.kbd.KeyIsPressed(0x45)) // E
        Cam.AdjustPosition(0.0f, speed, 0.0f);


    if (wnd.mouse.RightIsPressed())
    {
        int DeltaX = wnd.mouse.GetDeltaX();
        int DeltaY = wnd.mouse.GetDeltaY();

        Cam.AdjustRotation(DeltaY * sensitivity, DeltaX * sensitivity, 0.0f);
    }
}

