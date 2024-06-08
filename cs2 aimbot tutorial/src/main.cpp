#include "cheat/entity.hpp"

int main()
{
	printf("Starting cheat\n");

	// create new thread
	std::thread ReadThread(&Reader::ThreadLoop, &reader);
	
	// detach thread so we don't have to care about it anymore.
	ReadThread.detach();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// run aimbot code
		aimbot.doAimbot();
	}
}