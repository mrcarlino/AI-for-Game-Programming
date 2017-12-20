#pragma once
#include <IPathFinder.h>

namespace Pathfinding
{
	class AStar : public Library::IPathFinder
	{
	public:
		AStar();
		virtual std::deque<std::shared_ptr<Library::Node>> FindPath(std::shared_ptr<Library::Node> start, std::shared_ptr<Library::Node> end, std::set<std::shared_ptr<Library::Node>>& closedSet) override;
		void SetHeuristic(std::string heuristic);

	private:
		std::string mHeuristic;
	};
}