#include "Expection.h"
#include <sstream>

Expection::Expection(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{
}

const char* Expection::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

int Expection::GetLine() const noexcept
{
	return line;
}

const char* Expection::GetType() const noexcept
{
	return "Expection";
}

const std::string& Expection::GetFile() const noexcept
{
	return file;
}

std::string Expection::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}