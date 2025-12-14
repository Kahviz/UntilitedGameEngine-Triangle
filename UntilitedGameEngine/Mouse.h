#pragma once
#include <queue>
#include <bitset>

class Mouse
{
    friend class Window;

public:
    class Event
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,

			Enter,
			Leave,
            //Middle
			MPress,
			MRelease,
            //Mousen sivussa
            X1Press,
            X1Release,
            X2Press,
            X2Release,

            Invalid
        };

    private:
        Type type;
        unsigned char code;
        bool leftIsPressed;
        bool rightIsPressed;
        bool middleIsPressed;
        int x;
        int y;

    public:
        Event() noexcept
            :
            type(Type::Invalid),
            code(0u),
            leftIsPressed(false),
			middleIsPressed(false),
            rightIsPressed(false),
            x(0),
            y(0)
        {}

        Event(Type type, const Mouse& parent) noexcept
            :
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			middleIsPressed(parent.middleIsPressed),
			x(parent.x),
			y(parent.y)
		{}
        bool IsValid() const noexcept
        {
			return type != Type::Invalid;
        }
        Type GetType() const noexcept
        {
            return type;
		}
        std::pair<int, int> GetPos() const noexcept;
        int GetPosX() const noexcept;
        int GetPosY() const noexcept;
        bool LeftIsPressed() const noexcept;
        bool RightIsPressed() const noexcept;
        bool MiddleIsPressed() const noexcept;
    };

public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    std::pair<int, int> GetPos() const noexcept;

	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
    bool LeftIsPressed() const noexcept;
    bool RightIsPressed() const noexcept;
    bool MiddleIsPressed() const noexcept;

	Mouse::Event Read() noexcept;

	bool isEmpty() const noexcept
    {
        return buffer.empty();
	}
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;

	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;

	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;

    void OnMiddlePressed(int x, int y) noexcept;
    void OnMiddleReleased(int x, int y) noexcept;

	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;
    void OnWheelDelta(int x, int y,int delta);
private:
    static constexpr unsigned int bufferSize = 16u;

    int x = 0;
    int y = 0;

    bool leftIsPressed = false;
    bool middleIsPressed = false;
    bool rightIsPressed = false;
	bool isInWindow = false;
    int wheelDeltaCarry = 0;

	std::queue<Event> buffer;
};
