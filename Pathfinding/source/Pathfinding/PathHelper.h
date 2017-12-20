#pragma once

#include "Node.h"

using namespace std;

namespace Pathfinding
{
	class PathHelper
	{
	public:
		float manhattenHeuristic(shared_ptr<Library::Node> current, shared_ptr<Library::Node> end);
		float euclideanHeuristic(shared_ptr<Library::Node> current, shared_ptr<Library::Node> end);
		float zeroHeuristic();
	};
}