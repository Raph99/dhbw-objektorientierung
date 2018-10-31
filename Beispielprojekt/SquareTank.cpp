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
	const int16_t breite = 1;
	int16_t laenge;
	enum orientierung{horizontal, vertikal};
public:
	Mauer() {};

};

class GameWindow : public Gosu::Window
{
public:
	GameWindow()
		: Window(720, 720)
	{
		set_caption("SquareTank");
	}

	// wird bis zu 60x pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinkt
	void update() override
	{
		
	}

	void draw() override
	{
		Gosu::Graphics::draw_rect(0.0, 0.0, this->width(), this->height(), Gosu::Color::WHITE, 0.0);
	}
};
// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}