#include "Settings.h"

int Settings::GetWidth()
{
	return Width;
}

int Settings::GetHeight()
{
	return Height;
}

void Settings::SetHeight(int height)
{
	Height = height;
}
void Settings::SetWidth(int width)
{
	Width = width;
}