#include <HAPI_lib.h>

using namespace HAPISPACE;
using namespace std;

//stars
struct Star
{
	//position
	float x, y, z;
};


//number of stars
const int kNumStars{ 1000 };

//function to clear the screen to a colour
void ClearToColour(BYTE* screen, int screenWidth, int screenHeight, HAPI_TColour colour)
{
	for (int offset = 0; offset < screenWidth * screenHeight * 4; offset += 4)
	{
		memcpy(screen + offset, &colour, 4);
	}
}

void RenderStars(int eyeDistance, Star* stars, int screenwidth, int screenheight, BYTE* screen)
{
	int Cx = screenwidth / 2;
	int Cy = screenheight / 2;

	for (int i = 0; i < kNumStars; i++)
	{
		float starX = stars[i].x;
		float starY = stars[i].y;
		float starZ = stars[i].z;

		int Sx = ((eyeDistance * (starX - Cx)) / (starZ + eyeDistance)) + Cx;
		int Sy = ((eyeDistance * (starY - Cy)) / (starZ + eyeDistance)) + Cy;

		int offset = (Sx + Sy * screenwidth) * 4;

		// Check if the star is off the screen before rendering
		if (starZ > 0 && Sx >= 0 && Sx < screenwidth && Sy >= 0 && Sy < screenheight)
		{
			screen[offset] = 255;
			screen[offset + 1] = 255;
			screen[offset + 2] = 255;
		}
	}
}

void HAPI_Main()
{
	//screen width and height
	int width{ 1024 };
	int height{ 768 };
	int depth{ 500 };

	//values to move stars
	float speed = 0.2f;
	int eyeDistance = 100;

	//initialise the size of the screen
	if (!HAPI.Initialise(width, height, "C2460536 Tristan"))
		return;

	//change colour of pixel
	BYTE* screen = HAPI.GetScreenPointer();

	//keyboard
	const HAPI_TKeyboardData& keyData{HAPI.GetKeyboardData()};

	//create stars
	Star stars[kNumStars];

	// Initialize the star positions
	for (int i = 0; i < kNumStars; i++) {
		stars[i].x = static_cast<float>(rand() % width);  // Random x-coordinate up to 1000
		stars[i].y = static_cast<float>(rand() % height);   // Random y-coordinate up to 600
		stars[i].z = static_cast<float>(rand() % depth + 1);   // Random z-coordinate up to 500
	}
	
	//while loop
	while (HAPI.Update())
	{

		//fps
		HAPI.SetShowFPS(true);

		//colors
		HAPI_TColour bg_col{ HAPI_TColour::BLACK };
		HAPI_TColour white{ HAPI_TColour::WHITE };
		
		//clear screen
		ClearToColour(screen, width, height, bg_col);

		//change the color of a pixel
		int pixel_x = 100;
		int pixel_y = 200;
		int pixel_offset = (pixel_x + pixel_y * width) * 4;
		screen[pixel_offset] = 255;
		screen[pixel_offset + 1] = 0;
		screen[pixel_offset + 2] = 255;

		//move stars
		for (int i = 0; i < kNumStars; i++)
		{
			stars[i].z -= speed;

			if (stars[i].z <= 0) {
				// Star is behind the viewer, reset z and re-randomize X and Y
				stars[i].z = depth;
				stars[i].x = static_cast<float>(rand() % width);
				stars[i].y = static_cast<float>(rand() % height);
			}
		}
		

		//star speed -/+
		if ((keyData.scanCode['W']) && (speed < 5.00f))
		{
			speed += 0.01f;
		}
		else if ((keyData.scanCode['S']) && (speed > 0.001f))
		{
			speed -= 0.01f;
		}

		//eyedistance -/+
		if ((keyData.scanCode['E']) && (eyeDistance < 100))
		{
			eyeDistance += 1;
		}
		else if ((keyData.scanCode['D']) && (eyeDistance > 0))
		{
			eyeDistance -= 1;
		}
		RenderStars(eyeDistance, stars, width, height, screen);

		//text
		HAPI.RenderText(10, 20, white, "Use W/S to move stars: " + to_string(speed));
		HAPI.RenderText(10, 35, white, "Use E/D to change eye distance: " + to_string(eyeDistance));
	}
}