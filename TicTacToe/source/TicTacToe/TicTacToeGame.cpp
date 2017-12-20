#include "pch.h"
#include "TicTacToeGame.h"
#include "Minimax.h"
#include "Player.h"
#include <ImGui\imgui_impl_dx11.h>

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

IMGUI_API LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace TicTacToe
{
	const XMVECTORF32 TicTacToeGame::BackgroundColor = Colors::Black;

	TicTacToe::Minimax game;

	Move moveCoordinates;
	bool aiTurn = false;
	bool makeMovePressed = false;
	bool isGameOver = false;
	bool isGamePlaying = false;
	bool didForfeit = false;
	int winner = INT_MAX;
	int movesLeft;
	int lastWinner = INT_MAX;

	TicTacToeGame::TicTacToeGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback), mShowMenuWindow(true), mShowInputWindow(false), mShowTextWindow(false), mShowStatisticsWindow(false), mBounds(Rectangle::Empty)
	{
	}

	const Library::Rectangle& TicTacToeGame::Bounds() const
	{
		return mBounds;
	}

	void TicTacToeGame::Initialize()
	{
		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mMouse = make_shared<MouseComponent>(*this, MouseModes::Absolute);
		mComponents.push_back(mMouse);
		mServices.AddService(MouseComponent::TypeIdClass(), mMouse.get());

		mImGui = make_shared<ImGuiComponent>(*this);
		mComponents.push_back(mImGui);
		mServices.AddService(ImGuiComponent::TypeIdClass(), mImGui.get());
		auto imGuiWndProcHandler = make_shared<UtilityWin32::WndProcHandler>(ImGui_ImplDX11_WndProcHandler);
		UtilityWin32::AddWndProcHandler(imGuiWndProcHandler);
		// TODO: setup visuals here!!

		// Load textures
		ComPtr<ID3D11Resource> textureResource;

		//// Human Piece
		wstring textureName = L"Content\\Textures\\human.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mHumanPiece.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		
		//// AI Piece
		textureName = L"Content\\Textures\\ai.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mAIPiece.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		
		//// Board Piece (None)
		textureName = L"Content\\Textures\\board.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mBoardPiece.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		
		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");
		
		mBounds = TextureHelper::GetTextureBounds(texture.Get());

		auto sampleImGuiRenderBlock1 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			ImVec2 window_pos = ImVec2(510, 20);
			ImVec2 window_size = ImVec2(200, 100);
			ImGui::SetNextWindowPos(window_pos);
			ImGui::SetNextWindowSize(window_size);
			ImGui::Begin("Game Menu");
			if (ImGui::Button("Begin", ImVec2(160, 20))) { 
				mShowInputWindow = 1;
				mShowTextWindow = 1;
				mShowStatisticsWindow = 1;
				isGamePlaying = true;
				isGameOver = false;
				movesLeft = 9;
			};
			if (ImGui::Button("Forfeit", ImVec2(160, 20))) {
				mShowInputWindow = 0;
				isGameOver = true;
				isGamePlaying = false;
				didForfeit = true;
			};
			if (ImGui::Button("Reset", ImVec2(160, 20))) { 
				mShowInputWindow = 0;
				mShowTextWindow = 0;
				mShowStatisticsWindow = 0;
				isGameOver = false;
				isGamePlaying = false;
				didForfeit = false;
				aiTurn = false;
				winner = INT_MAX;
				movesLeft = 9;
				mMoveX = 0;
				mMoveY = 0;
				mAIMoveX = 0;
				mAIMoveY = 0;
				// initialize the board to empty
				for (int i = 0; i < boardWidth; i++)
				{
					for (int j = 0; j < boardHeight; j++)
					{
						board[i][j] = '-';
					}
				}
			}
			ImGui::End();
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock1);

		auto sampleImGuiRenderBlock2 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			if (mShowInputWindow)
			{
				ImVec2 window_pos = ImVec2(510, 160);
				ImVec2 window_size = ImVec2(200, 100);
				ImGui::SetNextWindowPos(window_pos);
				ImGui::SetNextWindowSize(window_size);
				ImGui::Begin("Player Input Coordinates");
				ImGui::InputInt("X Coord", &mMoveX);
				ImGui::InputInt("Y Coord", &mMoveY);
				if (ImGui::Button("Make Move", ImVec2(180, 20)))
				{
					makeMovePressed = true;
				};
				ImGui::End();

				// limits on nodes coordinates to not go out of bounds
				if (mMoveX < 0) mMoveX = 0; else if (mMoveX > 2) mMoveX = 2;
				if (mMoveY < 0) mMoveY = 0; else if (mMoveY > 2) mMoveY = 2;
			}
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock2);

		auto sampleImGuiRenderBlock3 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			if (mShowTextWindow)
			{
				ImVec2 window_pos = ImVec2(60, 440);
				ImVec2 window_size = ImVec2(300, 100);
				ImGui::SetNextWindowPos(window_pos);
				ImGui::SetNextWindowSize(window_size);
				ImGui::Begin("Game Information");
				
				if (isGameOver)
				{
					ImGui::Text("Game Over");

					winner = game.Evaluation(board);

					if (didForfeit)
					{
						ImGui::Text("You forfeit, AI Wins...");
						lastWinner = -1;
					}
					else if (winner == 1)
					{
						ImGui::Text("Player 1 (X) is the winner!");
						lastWinner = 1;
					}
					else if (winner == -1)
					{
						ImGui::Text("AI (O) is the winner!");
						lastWinner = -1;
					}
					else if (winner == 0)
					{
						ImGui::Text("The game is a draw");
						lastWinner = 0;
					}
				}
				else if (aiTurn == false)
				{
					ImGui::Text("Player 1, your turn!");
					ImGui::Text("Click on board or use coordinates");
				}

				ImGui::End();
			}
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock3);

		auto sampleImGuiRenderBlock4 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			if (mShowStatisticsWindow)
			{
				ImVec2 window_pos = ImVec2(460, 440);
				ImVec2 window_size = ImVec2(300, 100);
				ImGui::SetNextWindowPos(window_pos);
				ImGui::SetNextWindowSize(window_size);
				ImGui::Begin("Game Statistics");

				ImGui::Text("Moves Left: %d", movesLeft);
				ImGui::Text("Last Player Move: %d, %d", mMoveX, mMoveY);
				ImGui::Text("Last AI Move: %d, %d", mAIMoveX, mAIMoveY);

				if (lastWinner == -1)
					ImGui::Text("Last Winner: AI");
				else if (lastWinner == 1)
					ImGui::Text("Last Winner: Player");
				else if (lastWinner == 0)
					ImGui::Text("Last Winner: Draw");
				else
					ImGui::Text("Last Winner: None");

				ImGui::End();
			}
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock4);

		// initialize the board to empty
		for (int i = 0; i < boardWidth; i++)
		{
			for (int j = 0; j < boardHeight; j++)
			{
				board[i][j] = '-';
			}
		}

		Game::Initialize();
	}

	void TicTacToeGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();

		Game::Shutdown();
	}

	void TicTacToeGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		if (!game.GameOver(board))
		{
			if (isGamePlaying)
			{
				if (aiTurn == true)
				{
					// run minimax algorithm for ai move
					moveCoordinates = game.FindBestMove(board);
					board[moveCoordinates.row][moveCoordinates.col] = ai;
					mAIMoveX = moveCoordinates.col;
					mAIMoveY = moveCoordinates.row;
					aiTurn = false;
					movesLeft--;
				}
				else // get human player move
				{
					// used coordinate system
					if (makeMovePressed == true) {
						board[mMoveY][mMoveX] = player;
						makeMovePressed = false;
						aiTurn = true;
						movesLeft--;
					}

					// clicked on board
					if (mMouse->WasButtonPressedThisFrame(MouseButtons::Left))
					{
						mClickX = mMouse->X();
						mClickY = mMouse->Y();
					}
				}
			}
		}
		else
		{
			isGameOver = true;
		}

		Game::Update(gameTime);
	}

	void TicTacToeGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				
		// loops through game board and renders the board and pieces
		for (int i = 0; i < boardWidth; i++)
		{
			for (int j = 0; j < boardHeight; j++)
			{
				XMFLOAT2 position(static_cast<float>(j * boardMultiple + boardOffset), static_cast<float>(i * boardMultiple + boardOffset));

				// player clicks on board square
				if (mClickY > j * boardMultiple + boardOffset && mClickY < j * boardMultiple + boardMultiple && mClickX > i * boardMultiple + boardOffset && mClickX < i * boardMultiple + boardMultiple)
				{
					if (board[j][i] == '-')
					{
						mClickX = 0;
						mClickY = 0;
						mMoveX = i;
						mMoveY = j;
						makeMovePressed = true;
					}
				}

				if (board[i][j] == player)
					SpriteManager::DrawTexture2D(mHumanPiece.Get(), position);
				else if (board[i][j] == ai)
					SpriteManager::DrawTexture2D(mAIPiece.Get(), position);
				else
					SpriteManager::DrawTexture2D(mBoardPiece.Get(), position);
			}
		}

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

	void TicTacToeGame::Exit()
	{
		PostQuitMessage(0);
	}
}