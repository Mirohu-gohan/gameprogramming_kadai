// ウィンドウ制御クラスの実装

#include "window.h"

namespace {
    //---------------------------------------------------------------------------------
    /**
     * @brief	ウィンドウプロシージャ
     * @param	handle		ウィンドウハンドル
     * @param	msg			メッセージ
     * @param	wParam		メッセージパラメータ
     * @param	lParam		メッセージパラメータ
     * @return	処理結果
     */
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_DESTROY:  // ウィンドウが閉じられたとき
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}  // namespace

// 修正: std::string_view を std::wstring に変換して、LPCWSTR に対応させる  
[[nodiscard]] HRESULT Window::create(HINSTANCE instance, int width, int height, std::string_view name) noexcept {
    // ウィンドウ名を std::wstring に変換  
    std::wstring wname(name.begin(), name.end());

    // ウィンドウの定義  
    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = wname.c_str();  // 修正: wname.c_str() を使用  
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // ウィンドウクラスの登録  
    RegisterClass(&wc);

    // ウィンドウの作成  
    handle_ = CreateWindow(wc.lpszClassName, wc.lpszClassName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, instance, nullptr);
    if (!handle_) {
        return E_FAIL;
    }

    // ウインドウの表示  
    ShowWindow(handle_, SW_SHOW);

    // ウィンドウを更新  
    UpdateWindow(handle_);

    // ウィンドウのサイズを保存  
    witdh_ = width;
    height_ = height;

    // 成功を返す  
    return S_OK;
}

//---------------------------------------------------------------------------------
/**
 * @brief	メッセージループ
 */
[[nodiscard]] bool Window::messageLoop() const noexcept {
    MSG msg{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;  // WM_QUITメッセージが来たらループを抜ける
        }

        // メッセージ処理
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ウィンドウハンドルを取得する
 * @return	ウィンドウハンドル
 */
[[nodiscard]] HWND Window::handle() const noexcept {
    return handle_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ウィンドウのサイズを取得する
 * @return　ウィンドウのサイズ (横幅, 縦幅)
 */
[[nodiscard]] std::pair<int, int> Window::size() const noexcept {
    return { witdh_, height_ };
}