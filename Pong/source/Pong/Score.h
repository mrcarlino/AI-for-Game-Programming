#pragma once

#include "DrawableGameComponent.h"
#include "Rectangle.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "RenderStateHelper.h"

namespace Library
{
	class KeyboardComponent;
}

namespace Pong
{
	class Score final : public Library::DrawableGameComponent
	{
	public:
		Score(Library::Game& game);

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		static void Score::incRightScore();
		static void Score::incLeftScore();
		static int Score::getLeftScore();
		static int Score::getRightScore();

		void Reset();

	private:

		static const DirectX::XMFLOAT2 rightTextPosition;
		static const DirectX::XMFLOAT2 leftTextPosition;
		static const DirectX::XMFLOAT2 gameOverPosition;
		static const DirectX::XMFLOAT2 restartTextPosition;
		static const DirectX::XMFLOAT2 scoreLimitPosition;
		static std::random_device sDevice;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
		Library::Point mTextureHalfSize;
		Library::Rectangle mBounds;
		std::shared_ptr<DirectX::SpriteFont> mHelpFont;
		std::shared_ptr<DirectX::SpriteFont> mSmallFont;
		Library::RenderStateHelper mRenderStateHelper;
		Library::KeyboardComponent* mKeyboard;
		Library::AudioEngineComponent* mSound;
		std::unique_ptr<DirectX::SoundEffect> winSound;
		std::unique_ptr<DirectX::SoundEffect> loseSound;
		std::unique_ptr<DirectX::SoundEffect> startSound;
	};
}