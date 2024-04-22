#include "Application.h"

#include <memory>

auto main(int argc, char* argv[]) -> int
{
	auto app = std::make_unique<Application>();

	app->Run();
}
