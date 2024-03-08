#pragma once
#include <string>
#include <format>

class Logger {
public:
	static void OutputLog(const std::string& str);
	static void OutputLog(const std::wstring& str);
private:
	static std::wstring ConvertString(const std::string& str);
	static std::string ConvertString(const std::wstring& str);
};