#pragma once
#include <DirectXMath.h>

class Camera
{
public:
    Camera() noexcept;

    // Perustoiminnot
    void SetPosition(float x, float y, float z) noexcept;
    void SetRotation(float pitch, float yaw, float roll = 0.0f) noexcept;
    void AdjustPosition(float x, float y, float z) noexcept;
    void AdjustPosition(DirectX::XMFLOAT3 delta) noexcept;
    void AdjustRotation(float pitch, float yaw, float roll = 0.0f) noexcept;

    DirectX::XMFLOAT3 GetPositionFloat3() const noexcept { return pos; }
    DirectX::XMFLOAT3 GetRotationFloat3() const noexcept { return rot; }
    DirectX::XMVECTOR GetPositionVector() const noexcept;
    DirectX::XMVECTOR GetRotationVector() const noexcept;

    DirectX::XMFLOAT3 GetForwardVector() const noexcept;
    DirectX::XMFLOAT3 GetRightVector() const noexcept;
    DirectX::XMFLOAT3 GetUpVector() const noexcept;
    DirectX::XMVECTOR GetForwardVectorXM() const noexcept;
    DirectX::XMVECTOR GetRightVectorXM() const noexcept;
    DirectX::XMVECTOR GetUpVectorXM() const noexcept;

    DirectX::XMMATRIX GetViewMatrix() const noexcept;
    DirectX::XMMATRIX GetProjectionMatrix() const noexcept;
    DirectX::XMMATRIX GetViewProjectionMatrix() const noexcept;

    // Projektion asetukset
    void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) noexcept;
    void SetOrthographicValues(float viewWidth, float viewHeight, float nearZ, float farZ) noexcept;

    void MoveForward(float distance) noexcept;
    void MoveBackward(float distance) noexcept;
    void MoveLeft(float distance) noexcept;
    void MoveRight(float distance) noexcept;
    void MoveUp(float distance) noexcept;
    void MoveDown(float distance) noexcept;

    void LookAt(DirectX::XMFLOAT3 target) noexcept;

    void Update(float dt) noexcept;

private:
    DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };

    DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixIdentity();

    float fov = 90.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearZ = 0.1f;
    float farZ = 1000.0f;
    bool isPerspective = true;

    mutable DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
    mutable bool isViewDirty = true;
    mutable bool isProjectionDirty = true;

    void UpdateViewMatrix() const noexcept;
    void UpdateProjectionMatrix() noexcept;

    static float ToRadians(float degrees) noexcept { return degrees * (DirectX::XM_PI / 180.0f); }

    static void NormalizeAngles(DirectX::XMFLOAT3& angles) noexcept;
};