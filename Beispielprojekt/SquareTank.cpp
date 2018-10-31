#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "Planet.h"
#include "Vektor2d.h"

using namespace std;


// Simulationsgeschwindigkeit
const double DT = 100.0;

enum Orientierung { horizontal, vertikal };
enum Zustand {Start, SpielfeldAufbauen, SpielerErstellen};

class Geschoss 
{
	int16_t x;
	int16_t y;
	int16_t vx;
	int16_t vy;
	const int16_t vg = 20;
	// GameWindow Fenster !!!
	const int16_t lebenserwartung = 5000;
	int16_t timetolive;
public:
	Geschoss(int16_t x, int16_t y, int16_t vx, int16_t vy, int16_t l) : x(x), y(y), vx(vx), vy(vy), lebenserwartung(l)
	{};
};
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
	int16_t spielernr;
	// GameWindow Fenster !! als Zeiger
	int16_t x;
	int16_t y;
	int16_t munition = 10;
	vector<Geschoss> geschoss;
	const int16_t nachladezeit = 3;
	int16_t schussfreigabe;
public:
	Panzer(int16_t s/*Fenser*/) : spielernr(s) { };
};


class GameWindow : public Gosu::Window
{
public:
	Zustand zustand = Start;
	vector<Mauer> Mauern;
	vector<Panzer> Panzer;

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
		if (this->zustand == Start) {
			this->zustand = SpielfeldAufbauen;
		}
		if (this->zustand == SpielfeldAufbauen) {
			this->Mauern.push_back(Mauer(720, horizontal, 0, 0));
			this->Mauern.push_back(Mauer(720, horizontal, 0, 710));
			this->Mauern.push_back(Mauer(720, vertikal, 0, 0));
			this->Mauern.push_back(Mauer(720, vertikal, 710, 0));
		}
	}

	void draw() override
	{
		Gosu::Graphics::draw_rect(0.0, 0.0, this->width(), this->height(), Gosu::Color::WHITE, 0.0);

		//Mauern zeichnen
		for (Mauer mauer : this->Mauern) {
			if (mauer.get_orientierung() == vertikal) {
				Gosu::Graphics::draw_rect(mauer.get_x(), mauer.get_y(), mauer.get_hoehe(), mauer.get_laenge(), Gosu::Color::BLACK, 1.0);
			}
			else if (mauer.get_orientierung() == horizontal) {
				Gosu::Graphics::draw_rect(mauer.get_x(), mauer.get_y(), mauer.get_laenge(), mauer.get_hoehe(), Gosu::Color::BLACK, 1.0);
			}
		}
	}
};
// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}