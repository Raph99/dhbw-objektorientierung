#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <math.h>
#include <iostream>

using namespace std;

// Simulationsgeschwindigkeit
const double DT = 100.0;

//Beginn unser Programm

//Enum für StateMachine
enum Zustand { Start, SpielfeldAufbauen, SpielerErstellen, Spielen };

//Enum für Mauern
enum Orientierung { horizontal, vertikal };

//Klassenprototypen
class Geschoss;
class Mauer;
class Panzer;
class GameWindow;

class Geschoss 
{
	const double vg = 20;
	const int16_t lebenserwartung = 5000;

	GameWindow& Fenster;

	double x;
	double y;
	double vx;
	double vy;	
	int16_t timetolive;

public:
	Geschoss(double x, double y, double vx, double vy, int16_t l, GameWindow& f) : x(x), y(y), vx(vx), vy(vy), lebenserwartung(l), Fenster(f)
	{
	};
};


class Mauer
{
	const double hoehe = 10;

	double laenge;
	Orientierung orientierung;
	double x;
	double y;

public:
	Mauer(double l, Orientierung o, double x, double y) : laenge(l), orientierung(o), x(x), y(y)
	{
	};

	double get_hoehe(void) { return hoehe; };
	double get_laenge(void) { return laenge; };
	Orientierung get_orientierung(void) { return orientierung; };
	double get_x(void) { return x; };
	double get_y(void) { return y; };
};

class Panzer
{
	GameWindow& Fenster;
	int16_t spielernr;
	Gosu::Image bild;
	double vg = 3;
	double vrot = 2;
	
	double x;
	double y;
	double angle;
	int16_t munition = 10;
	
	const int16_t nachladezeit = 1*60;
	int16_t schussfreigabe;

	vector<Geschoss> geschoss;

public:
	Panzer(double x, double y, double a, int16_t s, GameWindow& f) : x(x), y(y), angle(a), spielernr(s), Fenster(f) 
	{
		if (this->spielernr == 1) {
			bild = Gosu::Image("PanzerGruen.png");
		}
		else if (this->spielernr == 2) {
			bild = Gosu::Image("PanzerRot.png");
		}
	};

	void act();

	double get_x(void) { return this->x; };
	double get_y(void) { return this->y; };
	double get_angle(void) { return this->angle; };
	Gosu::Image get_bild(void) { return this->bild; };
};


class GameWindow : public Gosu::Window
{
	Zustand zustand = Start;
	vector<Mauer> MauernListe;
	vector<Panzer> PanzerListe;

	void MauerErzeugen(double l, Orientierung o, double x, double y) {
		this->MauernListe.push_back(Mauer(l, o, x, y));
	}

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
		if (this->zustand == Start) {
			this->zustand = SpielfeldAufbauen;
		}
		if (this->zustand == SpielfeldAufbauen) {
			//Rahmen
			MauerErzeugen(720, horizontal, 0, 0);
			MauerErzeugen(720, horizontal, 0, 710);
			MauerErzeugen(720, vertikal, 0, 0);
			MauerErzeugen(720, vertikal, 710, 0);

			//Rest
			MauerErzeugen(578, horizontal, 0, 142);
			MauerErzeugen(578, horizontal, 142, 284);
			MauerErzeugen(578, horizontal, 0, 426);
			MauerErzeugen(578, horizontal, 142, 568);

			this->zustand = SpielerErstellen;
		}
		if (this->zustand == SpielerErstellen) {
			this->PanzerListe.push_back(Panzer(76.0, 76.0, 90.0, 1, *this));
			this->PanzerListe.push_back(Panzer(644.0, 644.0, 270.0, 2, *this));

			this->zustand = Spielen;
		}
		if (this->zustand == Spielen) {
			for (Panzer& panzer : this->PanzerListe) {
				panzer.act();
			}
		}
	}

	void draw() override
	{
		Gosu::Graphics::draw_rect(0.0, 0.0, this->width(), this->height(), Gosu::Color::WHITE, 0.0);
		//Mauern zeichnen
		for (Mauer& mauer : this->MauernListe) {
			if (mauer.get_orientierung() == horizontal) {
				Gosu::Graphics::draw_rect(mauer.get_x(), mauer.get_y(), mauer.get_laenge(), mauer.get_hoehe(), Gosu::Color::BLACK, 1.0);
			}
			else if (mauer.get_orientierung() == vertikal) {
				Gosu::Graphics::draw_rect(mauer.get_x(), mauer.get_y(), mauer.get_hoehe(), mauer.get_laenge(), Gosu::Color::BLACK, 1.0);
			}
		}

		//Panzer zeichnen
		for (Panzer& panzer : this->PanzerListe) {
			panzer.get_bild().draw_rot(panzer.get_x(), panzer.get_y(), 0.5, panzer.get_angle());
		}

		//Geschosse zeichnen
		//...
	}
};
// C++ Hauptprogramm
int main()
{
	GameWindow window;
	window.show();
}

void Panzer::act(void) {
	{
		if (this->spielernr == 1) {
			if (this->Fenster.input().down(Gosu::KB_W)) {
				this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
			}
			if (this->Fenster.input().down(Gosu::KB_S)) {
				this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
			}
			if (this->Fenster.input().down(Gosu::KB_A)) {
				this->angle -= this->vrot;
				if (this->angle < 0.0) {
					this->angle = 360.0 + this->angle;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_D)) {
				this->angle += this->vrot;
				if (this->angle > 360.0) {
					this->angle = this->angle - 360.0;
				}
			}
		}
		else if (this->spielernr == 2) {
			if (this->Fenster.input().down(Gosu::KB_UP)) {
				this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
			}
			if (this->Fenster.input().down(Gosu::KB_DOWN)) {
				this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
			}
			if (this->Fenster.input().down(Gosu::KB_LEFT)) {
				this->angle -= this->vrot;
				if (this->angle < 0.0) {
					this->angle = 360.0 + this->angle;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_RIGHT)) {
				this->angle += this->vrot;
				if (this->angle > 360.0) {
					this->angle = this->angle - 360.0;
				}
			}
		}
	}
}