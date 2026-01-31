#pragma once

#include "device.h"
#include "shader.h"
#include "root_signature.h"

class PiplineStateObject final {
public:
    PiplineStateObject() = default;
    ~PiplineStateObject();

    [[nodiscard]] bool create(const Device& device, const Shader& shader, const RootSignature& rootSignature) noexcept;
    [[nodiscard]] ID3D12PipelineState* get() const noexcept;

private:
    ID3D12PipelineState* pipelineState_ = {};
};