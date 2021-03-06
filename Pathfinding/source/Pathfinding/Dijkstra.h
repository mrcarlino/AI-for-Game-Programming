#pragma once
#include <IPathFinder.h>

namespace Pathfinding
{
	class Dijkstra : public Library::IPathFinder
	{
	public:
		virtual std::deque<std::shared_ptr<Library::Node>> FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet) override;
	};
}

