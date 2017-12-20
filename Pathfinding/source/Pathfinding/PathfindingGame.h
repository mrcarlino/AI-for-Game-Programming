#pragma once

#include "Game.h"
#include "DrawableGameComponent.h"
#include "Rectangle.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace Library
{
	class KeyboardComponent;
	class ImGuiComponent;
	class Camera;
}

namespace Pathfinding
{
	class PathfindingGame : public Library::Game
	{
	public:
		PathfindingGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		const Library::Rectangle& Bounds() const;

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		void Exit();

		static DirectX::XMVECTORF32 BackgroundColor;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mNormalSquare;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mWallSquare;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mStartSquare;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mEndSquare;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mPathSquare;
		Library::Rectangle mBounds;

		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::ImGuiComponent> mImGui;

		bool mShowHeuristicWindow;
		bool mShowInputWindow;
		bool mShowInfoWindow;
		int mStartX;
		int mStartY;
		int mEndX;
		int mEndY;
	};
}