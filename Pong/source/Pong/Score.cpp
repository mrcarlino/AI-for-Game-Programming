#include "pch.h"
#include "Score.h"

using namespace DirectX;
using namespace Library;
using namespace std;
using namespace Microsoft::WRL;

namespace Pong
{
	const XMFLOAT2 Score::rightTextPosition = { 460.0f, 10.0f };
	const XMFLOAT2 Score::leftTextPosition = { 300.0f, 10.0f };
	const XMFLOAT2 Score::gameOverPosition = { 248.0f, 10.0f };
	const XMFLOAT2 Score::restartTextPosition = { 340.0f, 250.0f };
	const XMFLOAT2 Score::scoreLimitPosition = { 680.0f, 0.0f };
	const int scoreLimit = 3;
	bool gameOverSound = true;
	random_device Score::sDevice;
	wostringstream scoreTextRight;
	wostringstream scoreTextLeft;
	wostringstream scoreLimitText;
	wostringstream gameOverMessage;
	wostringstream restartMessage;


	Score::Score(Game& game) :
		DrawableGameComponent(game), mRenderStateHelper(game), mBounds(Rectangle::Empty)
	{
	}

	void Score::Initialize()
	{
		// Load a texture
		mHelpFont = make_unique<SpriteFont>(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_36_Regular.spritefont");
		mSmallFont = make_unique<SpriteFont>(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");

		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\centerBar.png";

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");
		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		mKeyboard = reinterpret_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));
		mSound = reinterpret_cast<AudioEngineComponent*>(mGame->Services().GetService(AudioEngineComponent::TypeIdClass()));

		winSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\win.wav");
		loseSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\lose.wav");
		startSound = make_unique<SoundEffect>(mSound->AudioEngine().get(), L"Content\\Sounds\\start.wav");

		Reset();
	}

	void Score::Update(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		// reset score on game restart
		if (getLeftScore() == scoreLimit || getRightScore() == scoreLimit)
		{
			if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
			{
				Reset();
			}
		}
	}

	void Score::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);
		mRenderStateHelper.SaveAll();

		// render the scores
		if (getLeftScore() < scoreLimit && getRightScore() < scoreLimit)
		{
			SpriteManager::DrawString(mHelpFont, scoreTextRight.str().c_str(), rightTextPosition);
			SpriteManager::DrawString(mHelpFont, scoreTextLeft.str().c_str(), leftTextPosition);
		}
		// render the gameover/restart screen
		else
		{
			wstring gameOver;
			if (getLeftScore() == scoreLimit)
			{
				gameOver = L"  You     Win!";

				if (gameOverSound == true)
				{
					winSound->Play();
					gameOverSound = false;
				}
			}
			else if (getRightScore() == scoreLimit)
			{
				gameOver = L"Game  Over";

				if (gameOverSound == true)
				{
					loseSound->Play();
					gameOverSound = false;
				}
			}
			wstring restart = L"  play     again\n\npress    space";
			gameOverMessage.str(gameOver);
			gameOverMessage.clear();
			restartMessage.str(restart);
			restartMessage.clear();
			SpriteManager::DrawString(mHelpFont, gameOverMessage.str().c_str(), gameOverPosition);
			SpriteManager::DrawString(mSmallFont, restartMessage.str().c_str(), restartTextPosition);
		}
		SpriteManager::DrawString(mSmallFont, scoreLimitText.str().c_str(), scoreLimitPosition);
		mRenderStateHelper.RestoreAll();

		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);
	}

	void Score::Reset()
	{
		startSound->Play();
		wstring limit = L"Score Limit: " + to_wstring(scoreLimit);
		wstring left = L"0";
		wstring right = L"0";
		scoreLimitText.str(limit);
		scoreTextLeft.str(left);
		scoreTextLeft.clear();
		scoreTextRight.str(right);
		scoreTextRight.clear();
		mRenderStateHelper.SaveAll();
		SpriteManager::DrawString(mSmallFont, scoreLimitText.str().c_str(), scoreLimitPosition);
		SpriteManager::DrawString(mHelpFont, scoreTextRight.str().c_str(), rightTextPosition);
		SpriteManager::DrawString(mHelpFont, scoreTextLeft.str().c_str(), leftTextPosition);
		mRenderStateHelper.RestoreAll();

		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		mBounds.X = center.X - mTextureHalfSize.X;
		mBounds.Y = center.Y - mTextureHalfSize.Y;
		gameOverSound = true;
	}

	void Score::incRightScore()
	{
		int score = stoi(scoreTextRight.str());
		score++;
		wstring str = to_wstring(score);
		scoreTextRight.str(str);
		scoreTextRight.clear();
	}

	void Score::incLeftScore()
	{
		int score = stoi(scoreTextLeft.str());
		score++;
		wstring str = to_wstring(score);
		scoreTextLeft.str(str);
		scoreTextLeft.clear();
	}

	int Score::getLeftScore()
	{
		int score = stoi(scoreTextLeft.str().c_str());
		return score;
	}

	int Score::getRightScore()
	{
		int score = stoi(scoreTextRight.str().c_str());
		return score;
	}
}