#pragma once

#include<DirectXMath.h>
#include "object.h"

namespace game {

	class GameObject :public Object {

	public:

		GameObject() = default;
		GameObject(uint64_t parent):parent_(parent){}

		virtual ~GameObject() = default;

	public:

		virtual void initialize() noexcept override;

		virtual void update() noexcept override {};

		virtual void createDrawBuffer() noexcept override;

		virtual void updateDrawBuffer() noexcept override;

	public:
		virtual void onHit() noexcept {};

		[[nodiscard]] virtual uint64_t hitTargetTypeid() noexcept { return{}; }

	public:
		void setHandle(uint64_t handle) noexcept;

		[[nodiscard]] uint64_t handle() const noexcept;

		void set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 coler, uint64_t shapeid)noexcept;


		[[nodiscard]] DirectX::XMMATRIX world() const noexcept;

		[[nodiscard]] DirectX::XMFLOAT4 color() const noexcept;

	protected:
		DirectX::XMMATRIX world_ = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4 color_ = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f,1.0f);
		uint64_t shapeid_{};
		uint64_t handle_{};
		uint64_t parent_{};
		float radius_{};
	};
}