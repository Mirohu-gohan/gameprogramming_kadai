#include "root_signature.h"
#include <cassert>

RootSignature::~RootSignature() {
    if (rootSignature_) {
        rootSignature_->Release();
        rootSignature_ = nullptr;
    }
}

[[nodiscard]] bool RootSignature::create(const Device& device) noexcept {
    D3D12_DESCRIPTOR_RANGE r0{};
    r0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    r0.NumDescriptors = 1;
    r0.BaseShaderRegister = 0;
    r0.RegisterSpace = 0;
    r0.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_DESCRIPTOR_RANGE r1{};
    r1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    r1.NumDescriptors = 1;
    r1.BaseShaderRegister = 1;
    r1.RegisterSpace = 0;
    r1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    constexpr auto paramNum = 2;
    D3D12_ROOT_PARAMETER rootParameters[paramNum]{};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &r0;
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[1].DescriptorTable.pDescriptorRanges = &r1;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = paramNum;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* signature{};
    ID3DBlob* error{};
    auto res = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signature,
        &error);

    bool success = SUCCEEDED(res);
    if (!success) {
        assert(false && "Failed to serialize root signature");
    }
    else {
        res = device.get()->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature_));

        success &= SUCCEEDED(res);
        if (!success) {
            assert(false && "Failed to create root signature");
        }
    }

    if (error) { error->Release(); }
    if (signature) { signature->Release(); }

    return success;
}

[[nodiscard]] ID3D12RootSignature* RootSignature::get() const noexcept {
    if (!rootSignature_) {
        assert(false && "Root signature not created");
    }
    return rootSignature_;
}
