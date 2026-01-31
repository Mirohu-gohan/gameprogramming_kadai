#include "window.h"
#include "input.h"

namespace {
    /**
     * @brief   ウィンドウプロシージャ
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

/**
 * @brief   ウィンドウの生成
 */
[[nodiscard]] HRESULT Window::create(HINSTANCE instance, int width, int height, std::string_view name) noexcept {

    WNDCLASSA wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = name.data();
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClassA(&wc)) {
        return E_FAIL;
    }

    handle_ = CreateWindowA(
        wc.lpszClassName,
        wc.lpszClassName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!handle_) {
        return E_FAIL;
    }

    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);

    // メンバ変数への保存
    width_ = width;
    height_ = height;

    return S_OK;
}

/**
 * @brief   メッセージループ
 */
[[nodiscard]] bool Window::messageLoop() const noexcept {
    MSG msg{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // キー情報の取得
        static byte keyState[256]{};
        if (GetKeyboardState(keyState)) {
            Input::instance().updateKeyState(keyState);
        }
    }

    return true;
}

/**
 * @brief   ウィンドウハンドルを取得する
 */
[[nodiscard]] HWND Window::handle() const noexcept {
    return handle_;
}

/**
 * @brief   ウィンドウのサイズを取得する
 */
[[nodiscard]] std::pair<int, int> Window::size() const noexcept {
    return { width_, height_ };
}