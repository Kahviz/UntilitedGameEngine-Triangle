#include "Camera.h"
#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

Camera::Camera() noexcept
{
    SetProjectionValues(90.0f, 16.0f / 9.0f, 0.1f, 100.0f);
}

// POSITION FUNCTIONS
void Camera::SetPosition(float x, float y, float z) noexcept
{
    pos = XMFLOAT3(x, y, z);
    isViewDirty = true;
}

void Camera::SetRotation(float pitch, float yaw, float roll) noexcept
{
    rot = XMFLOAT3(pitch, yaw, roll);
    NormalizeAngles(rot);
    isViewDirty = true;
}

void Camera::AdjustPosition(float x, float y, float z) noexcept
{
    pos.x += x;
    pos.y += y;
    pos.z += z;
    isViewDirty = true;
}

void Camera::AdjustPosition(XMFLOAT3 delta) noexcept
{
    pos.x += delta.x;
    pos.y += delta.y;
    pos.z += delta.z;
    isViewDirty = true;
}

void Camera::AdjustRotation(float pitch, float yaw, float roll) noexcept
{
    rot.x += pitch;
    rot.y += yaw;
    rot.z += roll;
    NormalizeAngles(rot);
    isViewDirty = true;
}

// VECTOR GETTERS
XMVECTOR Camera::GetPositionVector() const noexcept
{
    return XMLoadFloat3(&pos);
}

XMVECTOR Camera::GetRotationVector() const noexcept
{
    return XMLoadFloat3(&rot);
}

// DIRECTION VECTORS
XMFLOAT3 Camera::GetForwardVector() const noexcept
{
    // Convert angles to radians
    float pitchRad = ToRadians(rot.x);
    float yawRad = ToRadians(rot.y);

    // Calculate forward vector from angles (FPS-style)
    XMFLOAT3 forward;
    forward.x = sinf(yawRad) * cosf(pitchRad);
    forward.y = -sinf(pitchRad); // Negative because Y is up in world but down in screen
    forward.z = cosf(yawRad) * cosf(pitchRad);

    // Normalize
    float length = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
    if (length > 0.0f)
    {
        forward.x /= length;
        forward.y /= length;
        forward.z /= length;
    }

    return forward;
}

XMFLOAT3 Camera::GetRightVector() const noexcept
{
    XMFLOAT3 forward = GetForwardVector();
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f }; // World up

    // Right = cross(world up, forward)
    XMFLOAT3 right;
    right.x = up.y * forward.z - up.z * forward.y;
    right.y = up.z * forward.x - up.x * forward.z;
    right.z = up.x * forward.y - up.y * forward.x;

    // Normalize
    float length = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
    if (length > 0.0f)
    {
        right.x /= length;
        right.y /= length;
        right.z /= length;
    }

    return right;
}

XMFLOAT3 Camera::GetUpVector() const noexcept
{
    XMFLOAT3 forward = GetForwardVector();
    XMFLOAT3 right = GetRightVector();

    // Up = cross(forward, right)
    XMFLOAT3 up;
    up.x = forward.y * right.z - forward.z * right.y;
    up.y = forward.z * right.x - forward.x * right.z;
    up.z = forward.x * right.y - forward.y * right.x;

    return up;
}

// XMVECTOR VERSIONS
XMVECTOR Camera::GetForwardVectorXM() const noexcept
{
    XMFLOAT3 forward = GetForwardVector();
    return XMLoadFloat3(&forward);
}

XMVECTOR Camera::GetRightVectorXM() const noexcept
{
    XMFLOAT3 right = GetRightVector();
    return XMLoadFloat3(&right);
}

XMVECTOR Camera::GetUpVectorXM() const noexcept
{
    XMFLOAT3 up = GetUpVector();
    return XMLoadFloat3(&up);
}

// MATRIX GETTERS
XMMATRIX Camera::GetViewMatrix() const noexcept
{
    if (isViewDirty)
    {
        UpdateViewMatrix();
    }
    return viewMatrix;
}

XMMATRIX Camera::GetProjectionMatrix() const noexcept
{
    return projectionMatrix;
}

XMMATRIX Camera::GetViewProjectionMatrix() const noexcept
{
    return GetViewMatrix() * GetProjectionMatrix();
}

// PROJECTION SETTINGS
void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) noexcept
{
    this->fov = fovDegrees;
    this->aspectRatio = aspectRatio;
    this->nearZ = nearZ;
    this->farZ = farZ;
    this->isPerspective = true;
    isProjectionDirty = true;
}

void Camera::SetOrthographicValues(float viewWidth, float viewHeight, float nearZ, float farZ) noexcept
{
    this->fov = viewHeight;
    this->aspectRatio = viewWidth / viewHeight;
    this->nearZ = nearZ;
    this->farZ = farZ;
    this->isPerspective = false;
    isProjectionDirty = true;
}

// MOVEMENT FUNCTIONS
void Camera::MoveForward(float distance) noexcept
{
    XMFLOAT3 forward = GetForwardVector();
    AdjustPosition(forward.x * distance, forward.y * distance, forward.z * distance);
}

void Camera::MoveBackward(float distance) noexcept
{
    MoveForward(-distance);
}

void Camera::MoveLeft(float distance) noexcept
{
    XMFLOAT3 right = GetRightVector();
    AdjustPosition(-right.x * distance, -right.y * distance, -right.z * distance);
}

void Camera::MoveRight(float distance) noexcept
{
    XMFLOAT3 right = GetRightVector();
    AdjustPosition(right.x * distance, right.y * distance, right.z * distance);
}

void Camera::MoveUp(float distance) noexcept
{
    AdjustPosition(0.0f, distance, 0.0f);
}

void Camera::MoveDown(float distance) noexcept
{
    AdjustPosition(0.0f, -distance, 0.0f);
}

// LOOK AT
void Camera::LookAt(XMFLOAT3 target) noexcept
{
    // Simple look-at: calculate rotation to look at target
    XMVECTOR eye = XMLoadFloat3(&pos);
    XMVECTOR targetVec = XMLoadFloat3(&target);

    // Calculate direction vector
    XMVECTOR direction = XMVectorSubtract(targetVec, eye);
    direction = XMVector3Normalize(direction);

    // Extract yaw and pitch from direction vector
    XMFLOAT3 dir;
    XMStoreFloat3(&dir, direction);

    // Calculate yaw (horizontal angle)
    float yaw = atan2f(dir.x, dir.z) * (180.0f / XM_PI);

    // Calculate pitch (vertical angle)
    float pitch = -asinf(dir.y) * (180.0f / XM_PI); // Negative because in screen Y goes down

    SetRotation(pitch, yaw, rot.z);
}

// UPDATE
void Camera::Update(float dt) noexcept
{
    // Optional: implement smooth movement or interpolation here
    // Currently does nothing, but can be used for advanced camera behaviors
}

// PRIVATE METHODS
void Camera::UpdateViewMatrix() const noexcept
{
    // Convert angles to radians
    float pitchRad = ToRadians(rot.x);
    float yawRad = ToRadians(rot.y);
    float rollRad = ToRadians(rot.z);

    // Create rotation quaternion
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);

    // Create rotation matrix from quaternion
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

    // Set forward vector (looking down positive Z)
    XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    forward = XMVector3TransformNormal(forward, rotationMatrix);

    // Set up vector (positive Y)
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    up = XMVector3TransformNormal(up, rotationMatrix);

    // Calculate target position
    XMVECTOR position = XMLoadFloat3(&pos);
    XMVECTOR target = XMVectorAdd(position, forward);

    // Create view matrix
    viewMatrix = XMMatrixLookAtLH(position, target, up);
    isViewDirty = false;
}

void Camera::UpdateProjectionMatrix() noexcept
{
    if (isPerspective)
    {
        // Perspective projection
        float fovRad = ToRadians(fov);
        projectionMatrix = XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
    }
    else
    {
        // Orthographic projection
        float halfWidth = fov * aspectRatio * 0.5f;
        float halfHeight = fov * 0.5f;
        projectionMatrix = XMMatrixOrthographicOffCenterLH(
            -halfWidth, halfWidth,
            -halfHeight, halfHeight,
            nearZ, farZ
        );
    }
    isProjectionDirty = false;
}

void Camera::NormalizeAngles(XMFLOAT3& angles) noexcept
{
    // Keep pitch between -89 and 89 degrees (prevent gimbal lock)
    if (angles.x > 89.0f) angles.x = 89.0f;
    if (angles.x < -89.0f) angles.x = -89.0f;

    // Wrap yaw between 0 and 360 degrees
    angles.y = fmodf(angles.y, 360.0f);
    if (angles.y < 0.0f) angles.y += 360.0f;

    // Wrap roll between 0 and 360 degrees
    angles.z = fmodf(angles.z, 360.0f);
    if (angles.z < 0.0f) angles.z += 360.0f;
}