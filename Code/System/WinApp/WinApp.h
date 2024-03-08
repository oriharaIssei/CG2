#pragma once
#include <Windows.h>

#include <string>
#include <memory>

#include <cstdint>

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp {
public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	WinApp() = default;
	~WinApp();

	void CreateGameWindow (const wchar_t* title , UINT windowStyle ,
		int32_t clientWidth, int32_t clientHeight);

	void TerminateGameWindow();

	bool ProcessMessage();
private:
	HWND hwnd_ = nullptr;   // ウィンドウハンドル
	std::unique_ptr<WNDCLASSEX> wndClass_ = nullptr; // ウィンドウクラス
	UINT windowStyle_;

	std::wstring title_;//ウィンドウクラス名も兼ねている
public:
};