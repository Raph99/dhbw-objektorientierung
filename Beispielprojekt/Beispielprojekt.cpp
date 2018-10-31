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

class Mauer
{
	const double hoehe;
	double laenge;
	
};

class GameWindow : public Gosu::Window
{
public:
	Gosu::Image bild;
	GameWindow()
		: Window(640, 480)
		, bild("rakete.png")
	{
		set_caption("Gosu Tutorial Game mit Git");
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
		bild.draw_rot(x, y,0.0, a ,0.5, 0.5);
	}
};
// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}