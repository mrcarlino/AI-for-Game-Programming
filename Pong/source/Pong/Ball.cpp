#include "pch.h"
#include "Ball.h"
#include "Score.h"

using namespace DirectX;
using namespace Library;
using namespace std;
using namespace Microsoft::WRL;

namespace Pong
{
	class Score;

	const int Ball::MinBallSpeed = 300;
	const int Ball::MaxBallSpeed = 350;
	const int scoreLimit = 3;

	random_device Ball::sDevice;
	default_random_engine Ball::sGenerator(sDevice());
	uniform_int_distribution<int> Ball::sBoolDistribution(0, 1);
	uniform_int_distribution<int> Ball::sSpeedDistribution(MinBallSpeed, MaxBallSpeed);

	Ball::Ball(Game& game) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty)
	{
	}

	const Library::Rectangle& Ball::Bounds() const
	{
		return mBounds;
	}

	DirectX::XMFLOAT2& Ball::Velocity()
	{
		return mVelocity;
	}

	void Ball::Initialize()
	{
		// Load a texture
		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\Ball.png";

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		mKeyboard = reinterpret_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));
		mSound = reinterpret_cast<AudioEngineComponent*>(mGame->Services().GetService(AudioEngineComponent::TypeIdClass()));

		whateverSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\whatever.wav");
		scoreSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\owee.wav");
		wallSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\wall.wav");

		Reset();
	}

	void Ball::Update(const Library::GameTime& gameTime)
	{
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();

		XMFLOAT2 positionDelta(mVelocity.x * elapsedTime, mVelocity.y * elapsedTime);
		mBounds.X += static_cast<int>(std::round(positionDelta.x));
		mBounds.Y += static_cast<int>(std::round(positionDelta.y));

		auto& viewport = mGame->Viewport();

		// score points
		if (mBounds.X + mBounds.Width >= viewport.Width && mVelocity.x > 0.0f)
		{
			if (Score::getLeftScore() < scoreLimit && Score::getRightScore() < scoreLimit)
				leftPlayerScores();
			Reset();
		}
		if (mBounds.X <= 0 && mVelocity.x < 0.0f)
		{
			if (Score::getLeftScore() < scoreLimit && Score::getRightScore() < scoreLimit)
				rightPlayerScores();
			Reset();
		}

		// Ball hits top or bottom of screen
		if (mBounds.Y + mBounds.Height >= viewport.Height && mVelocity.y > 0.0f)
		{
			mVelocity.y *= -1;
			wallSound->Play();
		}
		
		if (mBounds.Y <= 0 && mVelocity.y < 0.0f)
		{
			mVelocity.y *= -1;
			wallSound->Play();
		}

		// reset ball on game restart
		if (Score::getLeftScore() == scoreLimit || Score::getRightScore() == scoreLimit)
		{
			if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
			{
				Reset();
			}
		}
	}

	void Ball::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);
	}

	void Ball::Reset()
	{
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		mBounds.X = center.X - mTextureHalfSize.X;
		mBounds.Y = center.Y - mTextureHalfSize.Y;

		mVelocity.x = static_cast<float>(sSpeedDistribution(sGenerator) * (sBoolDistribution(sGenerator) ? 1 : -1));
		mVelocity.y = static_cast<float>(sSpeedDistribution(sGenerator) * (sBoolDistribution(sGenerator) ? 1 : -1));
	}

	void Ball::rightPlayerScores()
	{
		Score::incRightScore();
		whateverSound->Play();
	}

	void Ball::leftPlayerScores()
	{
		Score::incLeftScore();
		scoreSound->Play();
	}

	void Ball::reverseBallX()
	{
		mVelocity.x *= -1;
		mVelocity.x += (mVelocity.x >= 0.0f) ? 10.0f : -10.0f;
	}

	void Ball::reverseBallY()
	{
		mVelocity.y *= -1;
		mVelocity.y += (mVelocity.y >= 0.0f) ? 10.0f : -10.0f;
	}
}