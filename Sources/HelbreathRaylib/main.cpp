#include <memory>
#include "Helbreath.h"

int main() {
	SetTraceLogLevel(LOG_ERROR);
	auto app = std::make_unique<Helbreath>();
	app->Run();
}