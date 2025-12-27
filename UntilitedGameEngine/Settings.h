#pragma once

class Settings {

public:
	int GetWidth();
	int GetHeight();
	void SetHeight(int height);
	void SetWidth(int width);
private:
	int Width;
	int Height;
};