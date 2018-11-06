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

struct Position {
	double x, y;
};

class Geschoss 
{
	const double vg = 7;
	const int16_t lebenserwartung = 15*60;

	Gosu::Image bild = Gosu::Image("Geschoss.png");

	GameWindow& Fenster;

	double x;
	double y;
	double vx;
	double vy;	
	int16_t timetolive;
	bool alive = true;

public:
	Geschoss(double x, double y, double angle, GameWindow& f) : x(x), y(y), vx(vg*sin(Gosu::degrees_to_radians(angle))), vy(-vg*cos(Gosu::degrees_to_radians(angle))), Fenster(f)
	{
		this->timetolive = this->lebenserwartung;
	};
	void act();

	double get_x(void) { return this->x; };
	double get_y(void) { return this->y; };
	Gosu::Image get_bild(void) { return this->bild; };
	bool is_alive(void) { return this->alive; };
};


class Mauer
{
	const double breite = 10;

	double laenge;
	double hoehe;
	Orientierung orientierung;
	double x;
	double y;

public:
	Mauer(double l, Orientierung o, double x, double y) : orientierung(o), x(x), y(y)
	{
		if (this->orientierung == horizontal) {
			this->laenge = l;
			this->hoehe = this->breite;
		}
		else if (this->orientierung == vertikal) {
			this->laenge = this->breite;
			this->hoehe = l;
		}
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

	bool alive = true;
	
	const int16_t nachladezeit = 1*60;
	int16_t schussfreigabe=0;

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

	vector<Position> erzeuge_Rahmen();

	bool touchiertMauer();

	bool getroffen();

	void schuss();

	double get_x(void) { return this->x; };
	double get_y(void) { return this->y; };
	double get_angle(void) { return this->angle; };
	bool is_alive(void) { return this->alive; };
	Gosu::Image get_bild(void) { return this->bild; };
};


class GameWindow : public Gosu::Window
{
	Zustand zustand = Start;
	vector<Mauer> MauernListe;
	vector<Panzer> PanzerListe;
	vector<Geschoss> Geschossliste;
	

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
			vector<Panzer> PanzerListeAktiv;
			for (Panzer& panzer : this->PanzerListe) {
				if (panzer.is_alive()) {
					PanzerListeAktiv.push_back(panzer);
				}
			}
			//this->PanzerListe = PanzerListeAktiv;
			for (Panzer& panzer : this->PanzerListe) {
				panzer.act();
			}

			vector<Geschoss> GeschosslisteAktiv;
			for (Geschoss& geschoss : this->Geschossliste) {
				if (geschoss.is_alive()) {
					GeschosslisteAktiv.push_back(geschoss);
				}
			}
			//this->Geschossliste = GeschosslisteAktiv;
			for (Geschoss& geschoss : this->Geschossliste) {
				geschoss.act();
			}
		}
	}

	void draw() override
	{
		Gosu::Graphics::draw_rect(0.0, 0.0, this->width(), this->height(), Gosu::Color::WHITE, 0.0);
		//Mauern zeichnen
		for (Mauer& mauer : this->MauernListe) {
			Gosu::Graphics::draw_rect(mauer.get_x(), mauer.get_y(), mauer.get_laenge(), mauer.get_hoehe(), Gosu::Color::BLACK, 2.0);
		}

		//Panzer zeichnen
		for (Panzer& panzer : this->PanzerListe) {
			if (panzer.is_alive()) {
				panzer.get_bild().draw_rot(panzer.get_x(), panzer.get_y(), 1.0, panzer.get_angle());
			}			
		}

		//Geschosse zeichnen
		for (Geschoss& geschoss : this->Geschossliste) {
			if (geschoss.is_alive()) {
				geschoss.get_bild().draw_rot(geschoss.get_x(), geschoss.get_y(), 1.0, 0.0);
			}
			
		}

		//Rahmen zeichnen
		
		for (Panzer panzer : this->PanzerListe) {
			vector<Position> Rahmen = panzer.erzeuge_Rahmen();
			for (Position pos : Rahmen) {
				Gosu::Graphics::draw_rect(pos.x, pos.y, 2, 2, Gosu::Color::YELLOW, 3.0);
			}
		}
	}

	vector<Mauer> get_MauernListe(void) {
		return this->MauernListe;
	}

	bool istPositionInMauer(Position pos) {
		for (Mauer mauer : this->MauernListe) {
			if (pos.y >= mauer.get_y() && pos.y <= mauer.get_y() + mauer.get_hoehe()) {
				if (pos.x >= mauer.get_x() && pos.x <= mauer.get_x() + mauer.get_laenge()) {
					return true;
				}
			}

		}
		return false;
	}
	void erzeuge_Geschoss(double x, double y, double angle) {
		Geschoss neues_Geschoss(x, y, angle, *this);
		this->Geschossliste.push_back(neues_Geschoss);
	}
	bool istPositionVonGeschoss(Position pos) {
		double toleranz = 2;
		for (Geschoss geschoss : this->Geschossliste) {
			Position posG = { geschoss.get_x(), geschoss.get_y() };
			if (round(pos.x) <= round(posG.x)+toleranz && round(pos.x) >= round(posG.x) - toleranz && round(pos.y) <= round(posG.y) + toleranz && round(pos.y) >= round(posG.y) - toleranz) {
				return true;
			}
		}
		return false;
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
		if (this->schussfreigabe > 0) {
			this->schussfreigabe--;
		}
		if (this->getroffen()) {
			this->alive = false;
		}

		if (this->spielernr == 1) {
			if (this->Fenster.input().down(Gosu::KB_W)) {
				this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer()) {
					this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
					this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_S)) {
				this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer()) {
					this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
					this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_A)) {
				double angle_old = this->angle;
				this->angle -= this->vrot;
				if (this->angle < 0.0) {
					this->angle = 360.0 + this->angle;
				}
				if (this->touchiertMauer()) {
					this->angle = angle_old;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_D)) {
				double angle_old = this->angle;
				this->angle += this->vrot;
				if (this->angle > 360.0) {
					this->angle = this->angle - 360.0;
				}
				if (this->touchiertMauer()) {
					this->angle = angle_old;
				}
			}

			if (this->Fenster.input().down(Gosu::KB_SPACE)) {
				this->schuss();
			}
		}
		else if (this->spielernr == 2) {
			if (this->Fenster.input().down(Gosu::KB_UP)) {
				this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer()) {
					this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
					this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_DOWN)) {
				this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer()) {
					this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
					this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_LEFT)) {
				double angle_old = this->angle;
				this->angle -= this->vrot;
				if (this->angle < 0.0) {
					this->angle = 360.0 + this->angle;
				}
				if (this->touchiertMauer()) {
					this->angle = angle_old;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_RIGHT)) {
				double angle_old = this->angle;
				this->angle += this->vrot;
				if (this->angle > 360.0) {
					this->angle = this->angle - 360.0;
				}
				if (this->touchiertMauer()) {
					this->angle = angle_old;
				}
			}

			if (this->Fenster.input().down(Gosu::KB_RETURN)) {
				this->schuss();
			}
		}
	}
}

void Panzer::schuss() {
	if (this->schussfreigabe == 0) {
		double hoehe = this->bild.height();
		double breite = this->bild.width();
		this->Fenster.erzeuge_Geschoss(this->x + sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0, this->y - cos(Gosu::degrees_to_radians(-this->angle))*hoehe / 2.0, this->angle);
		this->schussfreigabe = this->nachladezeit;
	}
}

bool Panzer::touchiertMauer(void) {
	vector<Position> Rahmen = this->erzeuge_Rahmen();


	for (Position pos : Rahmen) {
		if (this->Fenster.istPositionInMauer(pos)) {
			return true;
		}		
	}
	return false;
}

vector<Position> Panzer::erzeuge_Rahmen() {
	vector<Position> Rahmen;
	double Auflösung = 4;
	double hoehe = this->bild.height();
	double breite = this->bild.width();

	Position ObenMitte = { this->x + sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0,  this->y - cos(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0 };
	Rahmen.push_back(ObenMitte);
	for (double d = 0; d < breite / 2; d = d + Auflösung) {
		Rahmen.push_back({ ObenMitte.x + sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , ObenMitte.y - cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
		Rahmen.push_back({ ObenMitte.x - sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , ObenMitte.y + cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
	}

	Position UntenMitte = { this->x - sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0,  this->y + cos(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0};
	Rahmen.push_back(UntenMitte);
	for (double d = 0; d < breite / 2; d = d + Auflösung) {
		Rahmen.push_back({ UntenMitte.x + sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , UntenMitte.y - cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
		Rahmen.push_back({ UntenMitte.x - sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , UntenMitte.y + cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
	}

	Position LinksMitte = { this->x + sin(Gosu::degrees_to_radians(-(90-this->angle)))*breite / 2.0,  this->y - cos(Gosu::degrees_to_radians(-(90-this->angle)))*breite / 2.0 };
	Rahmen.push_back(LinksMitte);
	for (double d = 0; d < hoehe / 2; d = d + Auflösung) {
		Rahmen.push_back({ LinksMitte.x + sin(Gosu::degrees_to_radians(this->angle))*d,  LinksMitte.y - cos(Gosu::degrees_to_radians(this->angle))*d});
		Rahmen.push_back({ LinksMitte.x - sin(Gosu::degrees_to_radians(this->angle))*d,  LinksMitte.y + cos(Gosu::degrees_to_radians(this->angle))*d});
	}

	Position RechtsMitte = { this->x - sin(Gosu::degrees_to_radians(-(90 - this->angle)))*breite / 2.0,  this->y + cos(Gosu::degrees_to_radians(-(90 - this->angle)))*breite / 2.0 };
	Rahmen.push_back(RechtsMitte);
	for (double d = 0; d < hoehe / 2; d = d + Auflösung) {
		Rahmen.push_back({ RechtsMitte.x + sin(Gosu::degrees_to_radians(this->angle))*d,  RechtsMitte.y - cos(Gosu::degrees_to_radians(this->angle))*d });
		Rahmen.push_back({ RechtsMitte.x - sin(Gosu::degrees_to_radians(this->angle))*d,  RechtsMitte.y + cos(Gosu::degrees_to_radians(this->angle))*d });
	}

	return Rahmen;
}

void Geschoss::act() {
	double radius = this->bild.height() / 2;

	Position Oben = { this->x, this->y - radius };
	Position Unten = { this->x, this->y + radius };
	Position Rechts = { this->x + radius, this->y };
	Position Links = { this->x - radius, this->y };

	if (this->Fenster.istPositionInMauer(Oben) || this->Fenster.istPositionInMauer(Unten)) {
		this->vy = this->vy *(-1.0);
	}
	if (this->Fenster.istPositionInMauer(Links) || this->Fenster.istPositionInMauer(Rechts)) {
		this->vx = this->vx *(-1.0);
	}

	this->x += this->vx;
	this->y += this->vy;

	if (this->timetolive > 0) {
		this->timetolive--;
	}
	else {
		this->alive = false;
	}
}

bool Panzer::getroffen() {
	vector<Position> Rahmen = this->erzeuge_Rahmen();


	for (Position pos : Rahmen) {
		if (this->Fenster.istPositionVonGeschoss(pos)) {
			return true;
		}
	}
	return false;
}
