#include "pch.h"
#include "PathHelper.h"

using namespace std;

namespace Pathfinding
{
	float PathHelper::manhattenHeuristic(shared_ptr<Library::Node> current, shared_ptr<Library::Node> end)
	{
		float x = static_cast<float>(abs(current->Location().X - end->Location().X));
		float y = static_cast<float>(abs(current->Location().Y - end->Location().Y));
		return x + y;
	}
	
	float PathHelper::euclideanHeuristic(shared_ptr<Library::Node> current, shared_ptr<Library::Node> end)
	{
		float x = static_cast<float>(pow(current->Location().X - end->Location().X, 2));
		float y = static_cast<float>(pow(current->Location().Y - end->Location().Y, 2));
		return sqrt(x + y);
	}
	
	float PathHelper::zeroHeuristic()
	{
		return 0.0f;
	}
}