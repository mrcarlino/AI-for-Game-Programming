#include "pch.h"
#include <GridHelper.h>
#include <StopWatch.h>
#include "BreadthFirstSearch.h"
#include "GreedyBestFirst.h"
#include "AStar.h"
#include "Dijkstra.h"

using namespace std;
using namespace Library;

void DrawGrid(Graph graph, int32_t graphWidth, int32_t graphHeight, set<shared_ptr<Library::Node>> closedSet, shared_ptr<Library::Node> start, shared_ptr<Library::Node> end);
set<shared_ptr<Library::Node>> BuildPath(set<shared_ptr<Library::Node>> closedSet, shared_ptr<Library::Node> end);

int main(int argc, char* argv[])
{
	Pathfinding::BreadthFirstSearch BFS;
	Pathfinding::GreedyBestFirst Greedy;
	Pathfinding::AStar AStar;
	Pathfinding::Dijkstra Dijkstra;

	string filename = (argc == 2 ? argv[1] : "Content\\Grid.grid");
	set<shared_ptr<Library::Node>> closedSet;
	StopWatch stopwatch;
	int32_t gridWidth;
	int32_t gridHeight;
	Graph graph = GridHelper::LoadGridFromFile(filename, gridWidth, gridHeight);
	int startX, startY, endX, endY;
	string algorithm, restart;

	// program loop
	while (true)
	{
		// user selection and input
		cout << "Please enter which algorithm you would like to run:" << endl << endl;
		cout << " a - Breadth First Search" << endl;
		cout << " b - Greedy Best First Search" << endl;
		cout << " c - Dijkstra's Algorithm" << endl;
		cout << " d - A*" << endl;
		cout << " exit - Quit Program" << endl << endl;
		cout << "Choice: ";
		cin >> algorithm;

		while (algorithm != "a" && algorithm != "b" && algorithm != "c" && algorithm != "d" && algorithm != "exit")
		{
			cout << endl << "Not a valid choice, try again..." << endl << endl << "Choice: ";
			cin >> algorithm;
		}

		if (algorithm == "exit")
			break;

		// select start and end nodes
		cout << endl << "---Enter coordinates for the 10x10 grid---" << endl;
		cout << "Start node coordinates x y: ";
		cin >> startX >> startY;
		cout << "End node coordinates x y: ";
		cin >> endX >> endY;

		system("cls");

		auto start = graph.At(startX, startY);
		auto end = graph.At(endX, endY);

		cout << "Grid Size: " << gridWidth << "x" << gridHeight << endl;
		cout << "Start Node: (" << startX << ", " << startY << ")" << endl;
		cout << "End Node: (" << endX << ", " << endY << ")" << endl;

		// BFS
		if (algorithm == "a")
		{
			cout << endl << "Breadth First Search" << endl;
			stopwatch.Start();
			BFS.FindPath(start, end, closedSet);
			stopwatch.Stop();
			DrawGrid(graph, gridWidth, gridHeight, closedSet, start, end);
		}
		// Greedy
		else if (algorithm == "b")
		{
			cout << endl << "Greedy Best First Search" << endl;
			stopwatch.Start();
			Greedy.FindPath(start, end, closedSet);
			stopwatch.Stop();
			DrawGrid(graph, gridWidth, gridHeight, closedSet, start, end);
		}
		// Dijkstras
		else if (algorithm == "c")
		{
			cout << endl << "Dijkstra's Algorithm" << endl;
			stopwatch.Start();
			Dijkstra.FindPath(start, end, closedSet);
			stopwatch.Stop();
			DrawGrid(graph, gridWidth, gridHeight, closedSet, start, end);
		}
		// A*
		else if (algorithm == "d")
		{
			cout << endl << "A*" << endl;
			stopwatch.Start();
			AStar.FindPath(start, end, closedSet);
			stopwatch.Stop();
			DrawGrid(graph, gridWidth, gridHeight, closedSet, start, end);
		}
		else
		{
			cout << "Invalid input, please try again!";
		}
		

		if (start->Type() == NodeType::Normal && end->Type() == NodeType::Normal)
		{
			cout << endl << "Elapsed Time (ms): " << stopwatch.ElapsedMilliseconds().count() << endl;
			cout << "Visitied Nodes: " << closedSet.size() << endl << endl;
		}

		closedSet.clear();
		stopwatch.Reset();

		cout << "restart - Choose Again" << endl;
		cout << "exit - Quit Program" << endl;
		cout << endl << "Choice: ";
		cin >> restart;

		while (restart != "restart" && restart != "exit")
		{
			cout << endl << "Not a valid choice, try again..." << endl << endl << "Choice: ";
			cin >> restart;
		}

		if (restart == "exit")
			break;
		else
			system("cls");
	}

    return 0;
}

void DrawGrid(Graph graph, int32_t graphWidth, int32_t graphHeight, set<shared_ptr<Library::Node>> closedSet, shared_ptr<Library::Node> start, shared_ptr<Library::Node> end)
{
	start->SetParent(nullptr);
	string displayString;
	set<shared_ptr<Library::Node>> fullPath = BuildPath(closedSet, end);

	for (int i = 0; i < graphWidth; i++)
	{
		for (int j = 0; j < graphHeight; j++)
		{
			shared_ptr<Node> node = graph.At(j, i);

			if (node == start && node->Type() == NodeType::Normal)
				displayString = " S ";
			else if (node == end && node->Type() == NodeType::Normal)
				displayString = " E ";
			else if (fullPath.count(node) && node->Type() == NodeType::Normal && (start->Type() == NodeType::Normal && end->Type() == NodeType::Normal))
				displayString = " X ";
			else if (node->Type() == NodeType::Wall)
				displayString = " | ";
			else
				displayString = " - ";

			cout << displayString;
		}
		cout << endl;
	}
	
	if (fullPath.size() == 0)
		cout << endl << "No path was found!" << endl << endl;
	if (start->Type() == NodeType::Wall)
		cout << endl << "Start node is on a wall..." << endl << endl;
	else if (end->Type() == NodeType::Wall)
		cout << endl << "End node is on a wall..." << endl << endl;
}

set<shared_ptr<Library::Node>> BuildPath(set<shared_ptr<Library::Node>> closedSet, shared_ptr<Library::Node> end)
{
	set<shared_ptr<Library::Node>> path;
	shared_ptr<Library::Node> current = end;

	if (!closedSet.empty() && closedSet.count(end))
	{
		// end node
		path.insert(current);

		while (current->Parent().expired() == false)
		{
			current = current->Parent().lock();
			path.insert(current);
			
		}
	}
	return path;
}

