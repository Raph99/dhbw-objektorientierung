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

class GameWindow : public Gosu::Window
{
public:
	GameWindow()
		: Window(1920, 1080, true)
	{
		set_caption("Gosu Tutorial Game mit Git");
	}

	// wird bis zu 60x pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinkt
	int x, y;
	int angle = 0;
	Gosu::Image rocket = Gosu::Image("rakete.png");


	void draw() override
	{
		this->graphics().draw_triangle(
			10.0,
			10.0,
			Gosu::Color::BLUE,
			x,
			y,
			Gosu::Color::RED,
			260.0,
			250.0,
			Gosu::Color::GREEN,
			0.0,
			Gosu::AlphaMode::AM_DEFAULT
		);
		rocket.draw_rot(500, 500, 0.0, angle);

	}

	// Wird 60x pro Sekunde aufgerufen
	void update() override
	{
		x = input().mouse_x();
		y = input().mouse_y();

		if (input().down(Gosu::ButtonName::KB_UP)) {
			angle = (angle + 1);
		}
		if (input().down(Gosu::ButtonName::KB_DOWN)) {
			angle = (angle - 1);

		}
		if (input().down(Gosu::ButtonName::KB_ESCAPE)) {
			close();
		}
	}
};

// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}