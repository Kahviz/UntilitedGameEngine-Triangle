#include "Mouse.h"
#include "Windows.h"

std::pair<int, int> Mouse::Event::GetPos() const noexcept
{
	return { x, y };
}

int Mouse::Event::GetPosX() const noexcept
{
	return x;
}

int Mouse::GetDeltaX() const noexcept
{
	return x - prevX;
}

int Mouse::GetDeltaY() const noexcept
{
	return y - prevY;
}

bool Mouse::Event::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

bool Mouse::Event::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (!buffer.empty())
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

void Mouse::Update() noexcept
{
	prevX = x;
	prevY = y;
}
bool Mouse::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	prevX = x;
	prevY = y;

	x = newx;
	y = newy;

	buffer.push(Event(Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push(Event(Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push(Event(Event::Type::Enter, *this));
	TrimBuffer();
}

bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}
void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;

	buffer.push(Event(Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;
	buffer.push(Event(Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePressed(int x, int y) noexcept
{
	middleIsPressed = true;

	buffer.push(Event(Event::Type::MPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddleReleased(int x, int y) noexcept
{
	middleIsPressed = false;
	buffer.push(Event(Event::Type::MRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;
	buffer.push(Event(Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;
	buffer.push(Event(Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	buffer.push(Event(Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	buffer.push(Event(Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

void Mouse::OnWheelDelta(int x, int y,int delta)
{
	wheelDeltaCarry += delta;

	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}