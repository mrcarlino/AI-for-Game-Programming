#include "pch.h"
#include "PathfindingGame.h"
#include "BreadthFirstSearch.h"
#include "GreedyBestFirst.h"
#include "AStar.h"
#include "Dijkstra.h"
#include <GridHelper.h>
#include <StopWatch.h>
#include <ImGui\imgui_impl_dx11.h>

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

IMGUI_API LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawGrid(Graph graph, int32_t graphWidth, int32_t graphHeight, set<shared_ptr<Library::Node>> closedSet, shared_ptr<Library::Node> start, shared_ptr<Library::Node> end);
set<shared_ptr<Library::Node>> BuildPath(set<shared_ptr<Library::Node>> closedSet, shared_ptr<Library::Node> end);

namespace Pathfinding
{
	XMVECTORF32 PathfindingGame::BackgroundColor = Colors::Black;
	
	Pathfinding::BreadthFirstSearch BFS;
	Pathfinding::GreedyBestFirst Greedy;
	Pathfinding::AStar Astar;
	Pathfinding::Dijkstra Dijkstras;

	string filename = "Content\\Grid.grid";
	set<shared_ptr<Library::Node>> closedSet;
	set<shared_ptr<Library::Node>> path;
	shared_ptr<Library::Node> start;
	shared_ptr<Library::Node> end;
	shared_ptr<Library::Node> current;
	StopWatch stopwatch;
	int32_t gridWidth;
	int32_t gridHeight;
	Graph graph = GridHelper::LoadGridFromFile(filename, gridWidth, gridHeight);
	
	int startX, startY, endX, endY;
	string algorithm, heuristic;
	bool beginPressed;

	PathfindingGame::PathfindingGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback), mShowHeuristicWindow(false), mShowInputWindow(false), mShowInfoWindow(false), mStartX(), mStartY(), mEndX(), mEndY(), mBounds(Rectangle::Empty)
	{
	}

	const Library::Rectangle& PathfindingGame::Bounds() const
	{
		return mBounds;
	}

	void PathfindingGame::Initialize()
	{
		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mImGui = make_shared<ImGuiComponent>(*this);
		mComponents.push_back(mImGui);
		mServices.AddService(ImGuiComponent::TypeIdClass(), mImGui.get());	
		auto imGuiWndProcHandler = make_shared<UtilityWin32::WndProcHandler>(ImGui_ImplDX11_WndProcHandler);
		UtilityWin32::AddWndProcHandler(imGuiWndProcHandler);

		// Load a texture
		ComPtr<ID3D11Resource> textureResource;

		// Normal Square
		wstring textureName = L"Content\\grass.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mNormalSquare.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		// Wall Square
		textureName = L"Content\\wall.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mWallSquare.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		// Start Square
		textureName = L"Content\\begin.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mStartSquare.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		// End Square
		textureName = L"Content\\finish.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mEndSquare.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		// Path Square
		textureName = L"Content\\path.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mPathSquare.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		mBounds = TextureHelper::GetTextureBounds(texture.Get());

		auto sampleImGuiRenderBlock1 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			ImVec2 window_pos = ImVec2(560, 20);
			ImGui::SetNextWindowPos(window_pos);
			ImGui::Begin("Choose Algorithm");
			if (ImGui::Button("Breadth First Search", ImVec2(160, 20))) { algorithm = "Breadth First Search"; mShowHeuristicWindow = 0; mShowInputWindow = 1; }
			if (ImGui::Button("Greedy Best-First", ImVec2(160, 20))) { algorithm = "Greedy"; mShowHeuristicWindow = 1; mShowInputWindow = 0; }
			if (ImGui::Button("Dijkstra's Algorithm", ImVec2(160, 20))) { algorithm = "Dijkstra's Algorithm"; mShowHeuristicWindow = 0; mShowInputWindow = 1; }
			if (ImGui::Button("A* Algorithm", ImVec2(160, 20))) { algorithm = "A*"; mShowHeuristicWindow = 1; mShowInputWindow = 0; }
			ImGui::End();
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock1);

		auto sampleImGuiRenderBlock2 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			if (mShowHeuristicWindow)
			{
				ImVec2 window_pos = ImVec2(560, 200);
				ImGui::SetNextWindowPos(window_pos);
				ImGui::Begin("Choose Heuristic", &mShowHeuristicWindow);
				if (ImGui::Button("Manhatten Heuristic", ImVec2(160, 20))) { heuristic = "Manhatten"; mShowInputWindow = 1; }
				if (ImGui::Button("Euclidean Heuristic", ImVec2(160, 20))) { heuristic = "Euclidean"; mShowInputWindow = 1; }
				ImGui::End();
			}
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock2);

		auto sampleImGuiRenderBlock3 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			if (mShowInputWindow)
			{
				string inputWindowTitle = mShowHeuristicWindow ? algorithm + " - " + heuristic : algorithm;

				ImVec2 window_pos = ImVec2(560, 340);
				ImVec2 window_size = ImVec2(200, 400);
				ImGui::SetNextWindowPos(window_pos);
				ImGui::SetNextWindowSize(window_size);
				ImGui::Begin(inputWindowTitle.c_str());
				ImGui::InputInt("Start X", &mStartX);
				ImGui::InputInt("Start Y", &mStartY);
				ImGui::InputInt("End X", &mEndX);
				ImGui::InputInt("End Y", &mEndY);
				if (ImGui::Button("Begin", ImVec2(180, 20))) 
				{
					start = graph.At(mStartX, mStartY);
					end = graph.At(mEndX, mEndY);
					mShowInfoWindow = 1;
					beginPressed = true;
				};
				if (ImGui::Button("Clear", ImVec2(180, 20)))
				{
					mShowInfoWindow = 0;
					stopwatch.Reset();
					start.reset();
					end.reset();
					path.clear();
					closedSet.clear();
				}
				ImGui::End();

				// limits on nodes coordinates to not go out of bounds
				if (mStartX < 0) mStartX = 0; else if (mStartX > 9) mStartX = 9;
				if (mStartY < 0) mStartY = 0; else if (mStartY > 9) mStartY = 9;
				if (mEndX < 0) mEndX = 0; else if (mEndX > 9) mEndX = 9;
				if (mEndY < 0) mEndY = 0; else if (mEndY > 9) mEndY = 9;
			}
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock3);

		auto sampleImGuiRenderBlock4 = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			if (mShowInfoWindow)
			{
				ImVec2 window_pos = ImVec2(100, 520);
				ImVec2 window_size = ImVec2(300, 100);
				ImGui::SetNextWindowPos(window_pos);
				ImGui::SetNextWindowSize(window_size);
				ImGui::Begin("Information");
				ImGui::Text("Algorithm Run Time: %llu ms", stopwatch.ElapsedMilliseconds().count());
				ImGui::Text("Visited Nodes: %d", closedSet.size());

				if (start->Type() == NodeType::Wall)
					ImGui::Text("No path, start node is on a wall...");
				else if (end->Type() == NodeType::Wall)
					ImGui::Text("No path, end node is on a wall...");
				else if (path.size() == 0)
					ImGui::Text("No path was found!");


				ImGui::End();
			}
		});
		mImGui->AddRenderBlock(sampleImGuiRenderBlock4);

		Game::Initialize();
	}

	void PathfindingGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();

		Game::Shutdown();
	}

	void PathfindingGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		if (beginPressed == true)
		{
			// run algorithm
			if (algorithm == "Breadth First Search")
			{
				stopwatch.Start();
				BFS.FindPath(start, end, closedSet);
				stopwatch.Stop();
			}
			else if (algorithm == "Greedy")
			{
				Greedy.SetHeuristic(heuristic);
				stopwatch.Start();
				Greedy.FindPath(start, end, closedSet);
				stopwatch.Stop();
			}
			else if (algorithm == "Dijkstra's Algorithm")
			{
				stopwatch.Start();
				Dijkstras.FindPath(start, end, closedSet);
				stopwatch.Stop();
			}
			else if (algorithm == "A*")
			{
				Astar.SetHeuristic(heuristic);
				stopwatch.Start();
				Astar.FindPath(start, end, closedSet);
				stopwatch.Stop();
			}

			// build path
			current = end;

			if (!closedSet.empty() && closedSet.count(end))
			{
				path.insert(current);

				while (current->Parent().expired() == false)
				{
					current = current->Parent().lock();
					path.insert(current);

				}
			}

			beginPressed = false;
		}

		Game::Update(gameTime);
	}

	void PathfindingGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				
		for (int i = 0; i < gridWidth; i++)
		{
			for (int j = 0; j < gridHeight; j++)
			{
				shared_ptr<Node> node = graph.At(j, i);
				XMFLOAT2 position(static_cast<float>(j * 51), static_cast<float>(i * 51));

				if (node == start && node->Type() == NodeType::Normal)
					SpriteManager::DrawTexture2D(mStartSquare.Get(), position);
				else if (node == end && node->Type() == NodeType::Normal)
					SpriteManager::DrawTexture2D(mEndSquare.Get(), position);
				else if (path.count(node) && node->Type() == NodeType::Normal && (start->Type() == NodeType::Normal && end->Type() == NodeType::Normal))
					SpriteManager::DrawTexture2D(mPathSquare.Get(), position);
				else if (node->Type() == NodeType::Wall)
					SpriteManager::DrawTexture2D(mWallSquare.Get(), position);
				else
					SpriteManager::DrawTexture2D(mNormalSquare.Get(), position);
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

	void PathfindingGame::Exit()
	{
		PostQuitMessage(0);
	}
}