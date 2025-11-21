#include <Windows.h>
#include "Dx12.h" 

// グローバルにDx12インスタンスを定義
Dx12 g_dx12;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        // ★ 修正：BeginPaint/EndPaintで描画領域を「検証済み」とマークする
    {
        PAINTSTRUCT ps;
        // BeginPaintとEndPaintの呼び出しにより、Windowsに対して
        // このウィンドウの描画領域が処理されたことを通知する。
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    // --- 1. ウィンドウクラス登録・作成 ---
    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("GameWindow");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = nullptr;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        TEXT("GameWindow"),
        TEXT("My Game"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720,
        NULL, NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    // --- 2. DirectX 12 初期化の実行 ---
    if (!g_dx12.Initialize(hwnd)) {
        // ★ 修正: TEXT()マクロを適用
        MessageBox(hwnd, TEXT("DirectX12 の初期化に失敗しました。"), TEXT("エラー"), MB_OK);
        return 1;
    }

    // --- 3. メッセージループ ---
    bool nextframe = true;
    while (nextframe)
    {
        MSG msg{};

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                nextframe = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!nextframe) break;

        // ★ 毎フレームの描画処理を実行
        g_dx12.Render();
    }

    // Dx12デストラクタでReleaseAll()が呼ばれる
    return 0;
}