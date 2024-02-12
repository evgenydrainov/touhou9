#include "game.h"

int main(int argc, char* argv[]) {

	log_info("Starting game...");

	Game game_instance = {};
	game = &game_instance;

	bool result;
	{
		double t = glfwGetTime();

		result = game->init();

		log_info("Initialization took %.2fms.", (glfwGetTime() - t) * 1000.0);
		log_info("");
	}

	if (result) {
		game->run();
	}

	game->destroy();

	log_info("Game finished.");

	return 0;
}

