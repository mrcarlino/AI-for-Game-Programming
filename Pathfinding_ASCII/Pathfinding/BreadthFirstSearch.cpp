#include "pch.h"
#include "BreadthFirstSearch.h"

using namespace std;

// Implement FindPath, it IS our algorithm for breadth-first search (and similarly for the others but in their own classes)
namespace Pathfinding
{
	std::deque<std::shared_ptr<Library::Node>> BreadthFirstSearch::FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet)
	{
		shared_ptr<Library::Node> current;
		shared_ptr<Library::Node> node;
		queue<shared_ptr<Library::Node>> frontier;

		frontier.push(start);
		closedSet.insert(start);

		while (!frontier.empty())
		{
			current = frontier.front();
			frontier.pop();

			// early exit
			if (current == end)
			{
				break;
			}

			for (std::vector<weak_ptr<Library::Node>>::iterator it = current->Neighbors().begin(); it != current->Neighbors().end(); ++it)
			{
				node = it->lock();
				// only insert if not already present
				if (!closedSet.count(node))
				{
					node->SetParent(current);
					frontier.push(node);
					closedSet.insert(node);
				}
			}	
		}

		return std::deque<std::shared_ptr<Library::Node>>();
	}

}