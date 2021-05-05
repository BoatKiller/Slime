#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>

float angle[500000];
olc::vf2d vBall[500000];
int bc[1005][1005];
bool decay = false;
bool randAngles = false;
int numOfBalls = 100000;


class Slime : public olc::PixelGameEngine
{



public:

	Slime()
	{
		sAppName = "Slime simulation";
	}

public:
	

	int sense(olc::vd2d pos, double viewAngle, double viewDist)
	{
		int pX = pos.x + cos(viewAngle) * viewDist;
		int pY = pos.y + sin(viewAngle) * viewDist;
		
		return bc[pX][pY] + bc[pX + 1][pY] + bc[pX + 2][pY] + bc[pX][pY + 1] + bc[pX + 1][pY + 1] + bc[pX + 2][pY + 1] + bc[pX][pY + 2] + bc[pX + 1][pY + 2] + bc[pX + 2][pY + 2];

	}
	
public:

	bool OnUserCreate() override
	{
		
		srand(time(NULL));
		for (int i = 0; i < numOfBalls; i++) {
			double alpha = ((rand() % 10000) / 10000.0) * 8 * atan(1);
			double dist = ((rand() % 10000) / 10000.0) * 400;

			vBall[i] = { float(ScreenWidth() / 2 + cos(alpha) * dist), float(ScreenHeight() / 2 + sin(alpha) * dist) };

			if (randAngles) {
				angle[i] = ((rand() % 10000) / 10000.0) * 8 * atan(1);
			}
			else {
				angle[i] = 4 * atan(1) + alpha + ((rand() % 10000) / 10000.0 - 0.5) * atan(1) / 2;
			}
			
		}
		for (int i = 0; i <= ScreenWidth()+1; i++) {
			for (int j = 1; j <= ScreenHeight()+1; j++) {
				bc[i][j] = 0;
			}
		}
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		
		//Blur the screen
		int decayBuff = decay ? fElapsedTime * 30 : 0; //Disable for longer trail

		for (int i = ScreenWidth() + 1; i > 0; i--) {
			for (int j = ScreenHeight() + 1; j > 0; j--) {
					int b = (bc[i - 1][j - 1] + bc[i][j - 1] + bc[i + 1][j - 1] + bc[i - 1][j] + bc[i][j] + bc[i + 1][j] + bc[i - 1][j + 1] + bc[i][j + 1] + bc[i + 1][j + 1]) / 9;
					if (b >= decayBuff) b -= decayBuff;

					bc[i][j] = b;
					Draw(i - 1, j - 1, olc::Pixel(0, b, b));
				}
			}
		
		double sightRangeAngle = atan(1) / 2;
		int w1, w2, w3;
		int newX, newY;
		float speed = fElapsedTime * 50;
		int viewDist = 15;

		srand(time(NULL));

		for (int k = 0; k < numOfBalls; k++) {

			//Choose to stay like that or to slightly change angle according to light at three spots
			w1 = sense(vBall[k], angle[k] + sightRangeAngle, viewDist);
			w2 = sense(vBall[k], angle[k], viewDist);
			w3 = sense(vBall[k], angle[k] - sightRangeAngle, viewDist);
			

			int v = rand() % 10000;
			if ((w2 < w1 && w2 < w3) || v == 0) { //random move
				angle[k] += (((rand() % 1000) / 1000.0) - 0.5) * sightRangeAngle * 3;
			}
			else if (w2 > w1 && w2 > w3) { //same direction

			}
			else if (w3 > w2) { //turn right
				angle[k] -= sightRangeAngle;
			}
			else if (w1 > w2) { //turn left
				angle[k] += sightRangeAngle;
			}


			//Collision check
			newX = vBall[k].x + cos(angle[k]) * speed;
			newY = vBall[k].y + sin(angle[k]) * speed;

			if (newX < 0 || newX > ScreenWidth()) angle[k] = 4 * atan(1) - angle[k];
			if (newY < 0 || newY > ScreenHeight()) angle[k] = -angle[k];

			olc::vf2d dir = { cos(angle[k]) * speed / 2, sin(angle[k]) * speed / 2 };
			


			bc[int(vBall[k].x + dir.x) + 1][int(vBall[k].y + dir.y) + 1] = 255;

			vBall[k] += dir * 2;


			bc[int(vBall[k].x) + 1][int(vBall[k].y) + 1] = 255;


			Draw(vBall[k], olc::CYAN);
		}
		
		

		

		DrawString(0, 3, "num of particles : " + std::to_string(numOfBalls), olc::DARK_RED);
		if (GetKey(olc::SPACE).bPressed) decay = !decay;
		DrawString(0, 13, "decay : " + std::to_string(decay), olc::DARK_RED);
		DrawString(0, 23, "press SPACE to change decay", olc::DARK_RED);

		if (GetKey(olc::R).bPressed) {
			for (int i = 0; i < numOfBalls; i++) {
				angle[i] = ((rand() % 10000) / 10000.0) * 8 * atan(1);
			}
		}
		DrawString(0, 33, "press R to randomize angles", olc::DARK_RED);

		DrawString(0, 43, "press T to restart the simulation", olc::DARK_RED);
		
		if (GetKey(olc::ESCAPE).bHeld) return false;
		DrawString(0, 53, "press ESCAPE to leave", olc::DARK_RED);
		if (GetKey(olc::T).bPressed) OnUserCreate();
		return true;
	}
};

int main()
{
	char a;
	std::cout << "Number of particles : ";
	std::cin >> numOfBalls;

	std::cout << "Apply decay ? Yes (y) or No (n) : ";
	std::cin >> a;
	if (a == 'y') decay = true;

	std::cout << "Random angles ? Yes (y) or No (n) : ";
	std::cin >> a;
	if (a == 'y') randAngles = true;
	
	Slime demo;
	if (demo.Construct(1000, 1000, 1, 1))
		demo.Start();
	return 0;
}
