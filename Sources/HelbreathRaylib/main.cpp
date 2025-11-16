#include <memory>
#include "Helbreath.h"
#include "global_constants.h"

int main() {
	SetTraceLogLevel(LOG_ERROR);
	auto app = std::make_unique<Helbreath>();
	app->RunUpscaled(constant::BASE_WIDTH, constant::BASE_HEIGHT, constant::UPSCALE_FACTOR);
}