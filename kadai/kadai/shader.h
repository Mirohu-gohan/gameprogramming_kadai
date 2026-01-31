#pragma once

#include "device.h"

class Shader final {
public:
    Shader() = default;
    ~Shader();

    [[nodiscard]] bool create(const Device& device) noexcept;
    [[nodiscard]] ID3DBlob* vertexShader() const noexcept;
    [[nodiscard]] ID3DBlob* pixelShader() const noexcept;

private:
    ID3DBlob* vertexShader_{};
    ID3DBlob* pixelShader_{};
};
