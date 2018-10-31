#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "Planet.h"
#include "Vektor2d.h"


// Simulationsgeschwindigkeit
const double DT = 100.0;
enum Orientierung { horizontal, vertikal };
class Mauer
{
	const int16_t hoehe = 10;
	int16_t laenge;
	Orientierung orientierung;
	int16_t x;
	int16_t y;
public:
	Mauer(int16_t l, Orientierung o, int16_t x, int16_t y) : laenge(l), orientierung(o), x(x), y(y)
	{};
	int16_t get_hoehe(void) { return hoehe; };
	int16_t get_laenge(void) { return laenge; };
	Orientierung get_orientierung(void) { return orientierung; };
	int16_t get_x(void) { return x; };
	int16_t get_y(void) { return y; };
};
class Panzer
{

};
class Geschoss : public Panzer
{
	int16_t x;
	int16_t y;
	int16_t vx;
	int16_t vy;
	const int16_t vg = 20;
	// GameWindow Fenster !!!
	const int16_t lebenserwartung = 5000;
	int16_t timetolive;

};

class GameWindow : public Gosu::Window
{
public:
	GameWindow()
		: Window(640, 480)
	{
		set_caption("SquareTank");
	}

	// wird bis zu 60x pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinktwaw
	int x = 0;
	int y = 0;
	double a = 0;
	double b = 0;
	double angle = 0;
	
	void update() override
	{
		x = input().mouse_x();
		y = input().mouse_y();

		if (input().down(Gosu::ButtonName::MS_LEFT))
		{
			for (int i = 0; i <= 360; i++)
			{
				a--;
			}
		}
		if (input().down(Gosu::ButtonName::MS_RIGHT))
		{
			for (int i = 0; i <= 360; i++)
			{
				a++;
			}
		}
	}

	void draw() override
	{
		Gosu::Graphics::draw_line(
			x, y, Gosu::Color::RED,
			200, 10, Gosu::Color::RED,
			0.0
		);
	}
};
// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}