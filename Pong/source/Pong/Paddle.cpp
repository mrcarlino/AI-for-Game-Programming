#include "pch.h"
#include "Paddle.h"
#include "Ball.h"
#include "Score.h"

using namespace DirectX;
using namespace Library;
using namespace std;
using namespace Microsoft::WRL;

namespace Pong
{
	class Ball;
	class Score;

	bool sound = true;
	const int scoreLimit = 3;
	const int aiPaddleSpeed = 4;
	const int userPaddleSpeed = 4;

	random_device Paddle::sDevice;

	Paddle::Paddle(Game& game) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty), aiBounds(Rectangle::Empty)
	{
	}

	const Library::Rectangle& Paddle::Bounds() const
	{
		return mBounds;
	}

	const Library::Rectangle& Paddle::BoundsAI() const
	{
		return aiBounds;
	}

	void Paddle::Initialize()
	{
		// Load a texture
		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\Paddle.png";

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		aiBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		mKeyboard = reinterpret_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));
		mBall = reinterpret_cast<Ball*>(mGame->Services().GetService(Ball::TypeIdClass()));
		mSound = reinterpret_cast<AudioEngineComponent*>(mGame->Services().GetService(AudioEngineComponent::TypeIdClass()));
		
		paddleSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\paddle.wav");

		Reset();
	}

	void Paddle::Update(const Library::GameTime& gameTime)
	{
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();
		XMFLOAT2 positionDelta(elapsedTime, elapsedTime);
		Library::Rectangle ballBounds = mBall->Bounds();
		DirectX::XMFLOAT2 ballVelocity = mBall->Velocity();
		auto& viewport = mGame->Viewport();
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		
		// center the AI paddle to prepare for next hit
		if (ballVelocity.x <= 0.0f)
		{
			if (aiBounds.Y < center.Y - mTextureHalfSize.Y)
				aiBounds.Y += aiPaddleSpeed;
			else if (aiBounds.Y > center.Y - mTextureHalfSize.Y)
				aiBounds.Y -= aiPaddleSpeed;
		}

		// downward movement
		if (mBounds.Y + mBounds.Height < viewport.Height)
		{
			if (mKeyboard->IsKeyDown(Keys::Down))
			{
				mBounds.Y += userPaddleSpeed;
			}
		}

		if (aiBounds.Y + aiBounds.Height < viewport.Height)
		{
			if (aiBounds.Y < ballBounds.Y)
			{
					aiBounds.Y += aiPaddleSpeed;
			}
		}

		// upward movement
		if (mBounds.Y > 0)
		{
			if (mKeyboard->IsKeyDown(Keys::Up))
			{
				mBounds.Y -= userPaddleSpeed;
			}
		}

		if (aiBounds.Y > 0)
		{
			if (aiBounds.Y > ballBounds.Y)
			{
					aiBounds.Y -= aiPaddleSpeed;
			}
		}

		// ball hits right paddles
		if (ballBounds.X >= (viewport.Width - (aiBounds.Width + 15)) && ballVelocity.x >= 0.0f)
		{
			if (ballBounds.Y >= aiBounds.Y - mTextureHalfSize.Y + 50 && ballBounds.Y <= aiBounds.Y + mTextureHalfSize.Y + 50)
			{
				mBall->reverseBallX();

				// reverse ball y velocity if it hits the half of the paddle it approaches
				if (ballBounds.Y < aiBounds.Y + mTextureHalfSize.Y && ballVelocity.y > 0.0f)
				{
					mBall->reverseBallY();
				}

				if (ballBounds.Y > aiBounds.Y + mTextureHalfSize.Y && ballVelocity.y < 0.0f)
				{
					mBall->reverseBallY();
				}

				paddleSound->Play();
			}
		}

		// ball hits left paddle
		if (ballBounds.X <= mBounds.Width && ballVelocity.x <= 0.0f)
		{
			if (ballBounds.Y >= mBounds.Y - mTextureHalfSize.Y + 50 && ballBounds.Y <= mBounds.Y + mTextureHalfSize.Y + 50)
			{
				mBall->reverseBallX();

				if (ballBounds.Y < mBounds.Y + mTextureHalfSize.Y && ballVelocity.y > 0.0f)
				{
					mBall->reverseBallY();
				}

				if (ballBounds.Y > mBounds.Y + mTextureHalfSize.Y && ballVelocity.y < 0.0f)
				{
					mBall->reverseBallY();
				}

				paddleSound->Play();
			}
		}

		// reset paddle for new game
		if (Score::getLeftScore() == scoreLimit || Score::getRightScore() == scoreLimit)
		{
			sound = false;
			if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
			{
				Reset();
			}
		}
	}

	void Paddle::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);

		XMFLOAT2 positionAI(static_cast<float>(aiBounds.X), static_cast<float>(aiBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), positionAI);
	}

	void Paddle::Reset()
	{
		auto& viewport = mGame->Viewport();
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		aiBounds.X = static_cast<int>(viewport.Width - aiBounds.Width);
		aiBounds.Y = center.Y - mTextureHalfSize.Y;
		mBounds.X = static_cast<int>(0);
		mBounds.Y = center.Y - mTextureHalfSize.Y;
	}
}