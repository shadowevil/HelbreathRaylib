#include <memory>
#include "core/Application.h"
#include "Helbreath.h"

int main() {
	auto app = std::make_unique<Helbreath>();
	app->Run();
}