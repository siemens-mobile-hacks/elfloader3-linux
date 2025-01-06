#include "src/io/Loop.h"
#include <spdlog/spdlog.h>

int main() {
	spdlog::set_level(spdlog::level::debug);
	spdlog::debug("ewfwef");

	Loop::instance()->setTimeout([]() {
		spdlog::debug("timeout called");
	}, 1000);
	Loop::instance()->setInterval([]() {
		spdlog::debug("interval called");
	}, 1000);

	for (int i = 0; i < 1000; i++) {
		Loop::instance()->setInterval([]() {
			// ???
		}, i);
	}

	Loop::instance()->run();
	return 0;
}
