#include "pch.h"
#include "PongGame.h"
#include "Ball.h"
#include "Paddle.h"
#include "Score.h"

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

namespace Pong
{
	const XMVECTORF32 PongGame::BackgroundColor = Colors::Black;

	PongGame::PongGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback)
	{
	}

	void PongGame::Initialize()
	{
		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());

		auto audioEngine = make_shared<AudioEngineComponent>(*this);
		mComponents.push_back(audioEngine);
		mServices.AddService(AudioEngineComponent::TypeIdClass(), audioEngine.get());

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mBall = make_shared<Ball>(*this);
		mPaddle = make_shared<Paddle>(*this);
		mScore = make_shared<Score>(*this);
		mComponents.push_back(mBall);
		mComponents.push_back(mPaddle);
		mComponents.push_back(mScore);
		mServices.AddService(Ball::TypeIdClass(), mBall.get());
		mServices.AddService(Score::TypeIdClass(), mBall.get());


		Game::Initialize();
	}

	void PongGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();
	}

	void PongGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void PongGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				
		Game::Draw(gameTime);

		HRESULT hr = mSwapChain->Present(1, 0);

		// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr, "IDXGISwapChain::Present() failed.");
		}
	}

	void PongGame::Exit()
	{
		PostQuitMessage(0);
	}
}