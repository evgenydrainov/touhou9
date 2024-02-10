#include "game.h"

int main(int argc, char* argv[]) {
	Game game_instance = {};
	game = &game_instance;

	{
		double t = glfwGetTime();

		game->init();

		log_info("Initialization took %.2fms.", (glfwGetTime() - t) * 1000.0);
		log_info("");
	}

	game->run();

	game->destroy();

	return 0;
}

