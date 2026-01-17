// Shader.cpp

#include "shader.h"
#include "device.h" // Deviceクラスが必要
#include <cassert>
#include <string>
#include <algorithm>
#include <filesystem>
#include <D3Dcompiler.h>
#include <stdio.h> // OutputDebugStringA のために必要

#pragma comment(lib, "d3dcompiler.lib")

//---------------------------------------------------------------------------------
/**
 * @brief	デストラクタ
 */
//三角形
Shader::~Shader() {
    // ComPtrを使っていないため、手動で解放
    if (vertexShader_) {
        vertexShader_->Release();
        vertexShader_ = nullptr;
    }
    if (pixelShader_) {
        pixelShader_->Release();
        pixelShader_ = nullptr;
    }
}



//---------------------------------------------------------------------------------
/**
 * @brief	シェーダを作成する
 * @param	device	デバイスクラスのインスタンス
 * @return	成功すれば true
 */
[[nodiscard]] bool Shader::create(const Device& device) noexcept {

    // 実行ファイルがあるディレクトリの絶対パスを取得し、シェーダファイルのパスを構築する
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    std::filesystem::path fullPath(exePath);
    std::wstring dir = fullPath.parent_path().wstring();

    // shader.hlsl への絶対パスを構築 (実行ファイルと同じディレクトリを想定)
    std::wstring shaderFullPath = dir + L"\\shader.hlsl";

    ID3DBlob* error = nullptr;
    HRESULT res;
    bool success = true;

    //---------------------------------------------------------------------
    // 1. 頂点シェーダのコンパイル (エントリポイント名は "main" を使用)
    res = D3DCompileFromFile(
        shaderFullPath.data(),
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexShader_,
        &error
    );

    if (FAILED(res)) {
        success = false;
        if (error) {
            char* p = static_cast<char*>(error->GetBufferPointer());
            OutputDebugStringA("VS Compile Error: ");
            OutputDebugStringA(p);
            assert(false && "頂点シェーダのコンパイルに失敗しました (出力ウィンドウを確認)");
            error->Release();
            error = nullptr;
        }
        else {
            assert(false && "頂点シェーダのコンパイルに失敗しました (ファイルパス/内容を確認)");
        }
    }
    if (error) { // 失敗しなかったが、警告などでBLOBが生成された場合の解放処理
        error->Release();
        error = nullptr;
    }
    if (!vertexShader_) { return false; } // コンパイル失敗時に false を返す

    //---------------------------------------------------------------------
    // 2. ピクセルシェーダのコンパイル
    res = D3DCompileFromFile(
        shaderFullPath.data(),
        nullptr,
        nullptr,
        "ps",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &pixelShader_,
        &error
    );

    if (FAILED(res)) {
        success = false;
        if (error) {
            char* p = static_cast<char*>(error->GetBufferPointer());
            OutputDebugStringA("PS Compile Error: ");
            OutputDebugStringA(p);
            assert(false && "ピクセルシェーダのコンパイルに失敗しました (出力ウィンドウを確認)");
            error->Release();
            error = nullptr;
        }
        else {
            assert(false && "ピクセルシェーダのコンパイルに失敗しました (ファイルパス/内容を確認)");
        }
    }
    if (error) {
        error->Release();
        error = nullptr;
    }
    if (!pixelShader_) { return false; }

    return success;
}

//---------------------------------------------------------------------------------
/**
 * @brief	頂点シェーダを取得する
 * @return	頂点シェーダのデータ
 */
[[nodiscard]] ID3DBlob* Shader::vertexShader() const noexcept {
    if (!vertexShader_) {
        assert(false && "頂点シェーダが未作成です");
    }

    return vertexShader_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ピクセルシェーダを取得する
 * @return	ピクセルシェーダのデータ
 */
[[nodiscard]] ID3DBlob* Shader::pixelShader() const noexcept {
    if (!pixelShader_) {
        assert(false && "ピクセルシェーダが未作成です");
    }

    return pixelShader_;
}