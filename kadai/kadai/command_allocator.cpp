#include "command_allocator.h"
#include <cassert>

/**
 * @brief デストラクタ
 */
CommandAllocator::~CommandAllocator() {
    if (commandAllocator_) {
        commandAllocator_->Release();
        commandAllocator_ = nullptr;
    }
}

/**
 * @brief コマンドアロケータの作成
 * @param device デバイスクラスのインスタンス
 * @param type コマンドリストのタイプ
 * @return 成功すれば true
 */
bool CommandAllocator::create(const Device& device, const D3D12_COMMAND_LIST_TYPE type) noexcept {
    type_ = type;

    const auto hr = device.get()->CreateCommandAllocator(type_, IID_PPV_ARGS(&commandAllocator_));
    if (FAILED(hr)) {
        assert(false && "Failed to create Command Allocator");
        return false;
    }

    return true;
}

/**
 * @brief コマンドアロケータをリセットする
 */
void CommandAllocator::reset() noexcept {
    if (!commandAllocator_) {
        assert(false && "Command Allocator is null");
    }
    commandAllocator_->Reset();
}

/**
 * @brief コマンドアロケータを取得する
 */
ID3D12CommandAllocator* CommandAllocator::get() const noexcept {
    if (!commandAllocator_) {
        assert(false && "Command Allocator is null");
        return nullptr;
    }
    return commandAllocator_;
}

/**
 * @brief コマンドリストのタイプを取得する
 */
D3D12_COMMAND_LIST_TYPE CommandAllocator::getType() const noexcept {
    return type_;
}
