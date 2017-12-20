#pragma once

#include "DrawableGameComponent.h"
#include "Rectangle.h"
#include "Ball.h"

namespace Library
{
	class KeyboardComponent;
}

namespace Pong
{
	class Paddle final : public Library::DrawableGameComponent
	{
	public:
		Paddle(Library::Game& game);

		const Library::Rectangle& Bounds() const;
		const Library::Rectangle& BoundsAI() const;

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

		void Reset();

	private:
		static std::random_device sDevice;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
		Library::Point mTextureHalfSize;
		Library::Rectangle mBounds;
		Library::Rectangle aiBounds;
		Library::KeyboardComponent* mKeyboard;
		Library::AudioEngineComponent* mSound;
		Pong::Ball* mBall;
		std::unique_ptr<DirectX::SoundEffect> paddleSound;
	};
}