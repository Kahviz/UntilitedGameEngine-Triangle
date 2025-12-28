#pragma once
#include "Object.h"
class Saving
{
public:
	void SaveAll(const std::vector<Object>& Drawables);
	std::vector<Object> LoadDrawables(const std::string& path);
private:
};