#pragma once
#include <string>
#include <format>

class Logger {
public:
	void OutputLog(const std::string& str);
	void OutputLog(const std::wstring& str);
private:
	std::wstring ConvertString(const std::string& str);
	std::string ConvertString(const std::wstring& str);
};