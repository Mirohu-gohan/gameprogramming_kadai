#include "shader.h"
#include "device.h"
#include <cassert>
#include <string>
#include <algorithm>
#include <filesystem>
#include <D3Dcompiler.h>
#include <stdio.h>

Shader::~Shader() {
    if (vertexShader_) {
        vertexShader_->Release();
        vertexShader_ = nullptr;
    }
    if (pixelShader_) {
        pixelShader_->Release();
        pixelShader_ = nullptr;
    }
}

[[nodiscard]] bool Shader::create(const Device& device) noexcept {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    std::filesystem::path fullPath(exePath);
    std::wstring dir = fullPath.parent_path().wstring();
    std::wstring shaderFullPath = dir + L"\\shader.hlsl";

    ID3DBlob* error = nullptr;
    HRESULT res;
    bool success = true;

    res = D3DCompileFromFile(
        shaderFullPath.data(),
        nullptr,
        nullptr,
        "vs",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexShader_,
        &error
    );

    if (FAILED(res)) {
        success = false;
        if (error) {
            error->Release();
            error = nullptr;
        }
        assert(false && "Vertex Shader Compile Failed");
    }

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
            error->Release();
            error = nullptr;
        }
        assert(false && "Pixel Shader Compile Failed");
    }

    return success;
}

[[nodiscard]] ID3DBlob* Shader::vertexShader() const noexcept {
    return vertexShader_;
}

[[nodiscard]] ID3DBlob* Shader::pixelShader() const noexcept {
    return pixelShader_;
}