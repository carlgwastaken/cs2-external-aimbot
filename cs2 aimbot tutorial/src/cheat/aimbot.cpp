#include "entity.hpp"

void Aimbot::doAimbot()
{
	// sleep for 1ms to save cpu %
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// get our view_matrix
	auto view_matrix = mem.Read<view_matrix_t>(reader.client + offset::dwViewMatrix);

	// create our playerPositions vector, although i would recommend moving this out of the loop.
	std::vector<Vector> playerPositions;

	// clear our playerPositions vector to remove old players
	playerPositions.clear();

	for (const auto& player : reader.playerList)
	{
		// get the 3D position of the player we're CURRENTLY looping through.
		Vector playerPosition = mem.Read<Vector>(player.pCSPlayerPawn + offset::m_vOldOrigin);

		// get the spotted state struct
		uintptr_t spottedState = mem.Read<uintptr_t>(player.pCSPlayerPawn + offset::m_entitySpottedState);

		// create a headPosition Vector, this is kind of ghetto but it works fine.
		Vector headPos = { playerPosition.x += 0.0, playerPosition.y += 0.0, playerPosition.z += 65.0f };

		// create our out variables for the world_to_screen function.
		Vector f, h;

		if (Vector::world_to_screen(view_matrix, playerPosition, f) &&
			Vector::world_to_screen(view_matrix, headPos, h))
		{
			// add the filtered player to our vector
			playerPositions.push_back(h);
		}
		else
			printf("Failed w2s.\n");
	}

	// check if the user is holding the right mouse button.
	if (GetAsyncKeyState(VK_RBUTTON)) 
	{
		// find the closest player and store it in a variable
		auto closest_player = findClosest(playerPositions);

		// move the mouse to the player
		MoveMouseToPlayer(closest_player);
	}
}

Vector Aimbot::findClosest(const std::vector<Vector> playerPositions)
{
	// check if the player positions vector is empty, if it is then just break out of the function.
	if (playerPositions.empty())
	{
		printf("playerPositions vector was empty.\n");
		return { 0.0f, 0.0f, 0.0f };
	}

	// get the center of the screen to be able to subtract the playerPosition by the center of the screen so we know where they are on the screen.
	Vector center_of_screen{ (float)GetSystemMetrics(0) / 2, (float)GetSystemMetrics(1) / 2, 0.0f };

	// keep track of the lowest distance found
	float lowestDistance = 10000;

	// find the index of the new lowest distance in the vector and store it (-1 means there wasn't one found)
	int index = -1;

	// loop through every single vector.
	for (int i = 0; i < playerPositions.size(); ++i)
	{
		// at the current index we're at, check the playerPosition and then calculate its distance from the center.
		float distance(std::pow(playerPositions[i].x - center_of_screen.x, 2) + std::pow(playerPositions[i].y - center_of_screen.y, 2));

		// if the distance is lower than the last vector we checked, then add it and save the index.
		if (distance < lowestDistance) {
			lowestDistance = distance;
			index = i;
		}
	}

	// check if we even found a player.
	if (index == -1) {
		return { 0.0f, 0.0f, 0.0f };
	}

	// return the player at that index.
	return { playerPositions[index].x, playerPositions[index].y, 0.0f };
}

void Aimbot::MoveMouseToPlayer(Vector position)
{
	// check if the position is valid, make a function for this for better practice. this is also just ugly.
	if (position.IsZero())
		return;

	// get the center of our screen.
	Vector center_of_screen{ (float)GetSystemMetrics(0) / 2, (float)GetSystemMetrics(1) / 2, 0.0f };

	// get our new x and y, by subtracting the position by the center of the screen, giving us a position to move the mouse to.
	auto new_x = position.x - center_of_screen.x;
	auto new_y = position.y - center_of_screen.y;

	// move the mouse to said position.
	mouse_event(MOUSEEVENTF_MOVE, new_x, new_y, 0, 0);
}