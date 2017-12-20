#pragma once

#include "Game.h"
#include "Rectangle.h"

namespace Library
{
	class KeyboardComponent;
}

namespace Pong
{
	class Ball;
	class Paddle;
	class Score;

	class PongGame : public Library::Game
	{
	public:
		PongGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		void Exit();

		static const DirectX::XMVECTORF32 BackgroundColor;

		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Ball> mBall;
		std::shared_ptr<Paddle> mPaddle;
		std::shared_ptr<Score> mScore;
	};
}