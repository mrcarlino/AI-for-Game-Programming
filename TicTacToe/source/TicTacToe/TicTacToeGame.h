#pragma once

#include "Game.h"
#include "DrawableGameComponent.h"
#include "Rectangle.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>

enum Piece { X, O, None };

namespace Library
{
	class KeyboardComponent;
	class MouseComponent;
	class ImGuiComponent;
}

namespace TicTacToe
{
	class TicTacToeGame : public Library::Game
	{
	public:
		TicTacToeGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		const Library::Rectangle& Bounds() const;

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		void Exit();

		static const DirectX::XMVECTORF32 BackgroundColor;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mHumanPiece;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mAIPiece;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mBoardPiece;
		Library::Rectangle mBounds;

		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::MouseComponent> mMouse;
		std::shared_ptr<Library::ImGuiComponent> mImGui;

		bool mShowMenuWindow;
		bool mShowInputWindow;
		bool mShowTextWindow;
		bool mShowStatisticsWindow;

		int mClickX;
		int mClickY;
		int mMoveX;
		int mMoveY;
		int mAIMoveX;
		int mAIMoveY;

		char board[3][3];
		const int boardWidth = 3;
		const int boardHeight = 3;
		const int boardMultiple = 135;
		const int boardOffset = 10;
	};
}