#include "pch.h"
#include "AStar.h"
#include "PathHelper.h"

using namespace std;

namespace Pathfinding
{
	struct compare
	{
		bool operator()(const shared_ptr<Library::Node> a, const shared_ptr<Library::Node> b) const
		{
			return a->TotalCost() > b->TotalCost();
		}
	};

	std::deque<std::shared_ptr<Library::Node>> AStar::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet)
	{
		shared_ptr<Library::Node> current;
		shared_ptr<Library::Node> node;
		priority_queue<shared_ptr<Library::Node>, vector<shared_ptr<Library::Node>>, compare> frontier;
		PathHelper pathHelper;
		float pathCost;
		float heuristic;
		string heuristicChoice;

		cout << endl << "Choose which heuristic: " << endl << endl;
		cout << " a - Manhatten" << endl;
		cout << " b - Euclidean" << endl << endl;
		cout << "Choice: ";
		cin >> heuristicChoice;

		while (heuristicChoice != "a" && heuristicChoice != "b")
		{
			cout << endl << "Not a valid choice, try again..." << endl << endl << "Choice: ";
			cin >> heuristicChoice;
		}

		start->SetHeuristic(0);
		start->SetPathCost(0);
		frontier.push(start);
		closedSet.insert(start);

		while (!frontier.empty())
		{
			current = frontier.top();
			frontier.pop();

			if (current == end)
			{
				break;
			}

			for (std::vector<weak_ptr<Library::Node>>::iterator it = current->Neighbors().begin(); it != current->Neighbors().end(); ++it)
			{
				node = it->lock();

				pathCost = current->PathCost() + 1.0f;

				if (!closedSet.count(node) || pathCost < node->PathCost())
				{
					closedSet.insert(node);
					node->SetPathCost(pathCost);
					
					if (heuristicChoice == "a")
						heuristic = pathHelper.manhattenHeuristic(node, end);
					else
						heuristic = pathHelper.euclideanHeuristic(node, end);

					node->SetHeuristic(heuristic);
					node->SetParent(current);
					frontier.push(node);
				}
			}
		}

		return std::deque<std::shared_ptr<Library::Node>>();
	}
}
