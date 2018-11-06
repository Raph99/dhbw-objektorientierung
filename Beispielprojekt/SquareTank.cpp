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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Beginn unser Programm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Enum für StateMachine
enum Zustand { Start, SpielfeldAufbauen, SpielerErstellen, Spielen };

//Enum für Mauern
enum Orientierung { horizontal, vertikal };

//Struct für Berührungsprüfung
struct Position {
	double x, y;
};

//Klassendeklaration
class Geschoss;
class Mauer;
class Panzer;
class GameWindow;

//Klassendefinitionen
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

	//Deklaration ausgelagerter Funktionen
	void act();

	//Getter und Setter
	Gosu::Image get_bild(void) const{ return this->bild; };

	double get_x(void) const{ return this->x; };
	double get_y(void) const{ return this->y; };
	
	bool is_alive(void) const{ return this->alive; };
};


class Mauer
{
	const double breite = 10;

	double x;
	double y;
	Orientierung orientierung;
	double laenge;
	double hoehe;	

public:
	Mauer(double x, double y, Orientierung o, double l) : x(x), y(y), orientierung(o)
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

	//Getter und Setter
	double get_x(void) const{ return this->x; };
	double get_y(void) const{ return this->y; };

	Orientierung get_orientierung(void) const{ return this->orientierung; };
	double get_hoehe(void) const{ return this->hoehe; };
	double get_laenge(void) const{ return this->laenge; };
};

class Panzer
{	
	const double vg = 3;
	const double vrot = 2;
	const int16_t munitionskapazitaet = 10;
	const int16_t nachladezeit = 1 * 60;

	Gosu::Image bild;
	int16_t spielernr;
	GameWindow& Fenster;
	
	double x;
	double y;
	double angle;

	int16_t munition = this->munitionskapazitaet;
	int16_t schussfreigabe = 0;
	bool alive = true;	

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
	/*Panzer(const Panzer& panzer) : x(panzer.get_x()), y(panzer.get_y()), angle(panzer.get_angle()), spielernr(panzer.get_spielernr()), Fenster(panzer.get_Fenster())
	{
		if (this->spielernr == 1) {
			bild = Gosu::Image("PanzerGruen.png");
		}
		else if (this->spielernr == 2) {
			bild = Gosu::Image("PanzerRot.png");
		}
	};*/

	//Deklaration ausgelagerter Funktionen
	vector<Position> erzeuge_Rahmen();
	bool touchiertMauer();
	bool getroffen();
	void schuss();
	void gameOver();

	void act();	

	//Getter und Setter
	int16_t get_spielernr() const { return this->spielernr; };
	GameWindow& get_Fenster() const { return this->Fenster; };
	Gosu::Image get_bild(void) const { return this->bild; };

	double get_x(void) const { return this->x; };
	double get_y(void) const { return this->y; };
	double get_angle(void) const { return this->angle; };
	bool is_alive(void) const { return this->alive; };
};


class GameWindow : public Gosu::Window
{
	Zustand zustand = Start;

	vector<Mauer> MauernListe;
	vector<Panzer> PanzerListe;
	vector<Geschoss> GeschossListe;
	
	//Private Methoden
	void MauerErzeugen(double x, double y, Orientierung o, double l) {
		this->MauernListe.push_back(Mauer(x, y, o, l));
	}

public:
	GameWindow()
		: Window(720, 720)
	{
		set_caption("SquareTank");
	}

	// wird bis zu 60 Mal pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinkt
	void update() override
	{
		if (this->zustand == Start) {
			this->zustand = SpielfeldAufbauen;
		}

		if (this->zustand == SpielfeldAufbauen) {
			//Rahmen
			MauerErzeugen(0.0, 0.0, horizontal, 720.0);
			MauerErzeugen(0.0, 710.0, horizontal, 720.0);
			MauerErzeugen(0.0, 0.0, vertikal, 720.0);
			MauerErzeugen(710.0, 0.0, vertikal, 720.0);

			//Rest
			MauerErzeugen(0.0, 142.0, horizontal, 578.0);
			MauerErzeugen(142.0, 284.0, horizontal, 578.0);
			MauerErzeugen(0.0, 426.0, horizontal, 578.0);
			MauerErzeugen(142.0, 568.0, horizontal, 578.0);

			this->zustand = SpielerErstellen;
		}

		if (this->zustand == SpielerErstellen) {
			this->PanzerListe.push_back(Panzer(76.0, 76.0, 90.0, 1, *this));
			this->PanzerListe.push_back(Panzer(644.0, 644.0, 270.0, 2, *this));

			this->zustand = Spielen;
		}
		if (this->zustand == Spielen) {
			/*vector<Panzer> PanzerListeAktiv;
			for (Panzer& panzer : this->PanzerListe) {
				if (panzer.is_alive()) {
					PanzerListeAktiv.push_back(panzer);
				}
			}
			this->PanzerListe = PanzerListeAktiv; */
			for (Panzer& panzer : this->PanzerListe) {
				panzer.act();
			}

			/*vector<Geschoss> GeschossListeAktiv;
			for (Geschoss& geschoss : this->GeschossListe) {
				if (geschoss.is_alive()) {
					GeschossListeAktiv.push_back(geschoss);
				}
			}
			this->Geschossliste = GeschosslisteAktiv;*/
			for (Geschoss& geschoss : this->GeschossListe) {
				geschoss.act();
			}
		}
	}

	void draw() override
	{
		//Hintergrund zeichnen
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
		for (Geschoss& geschoss : this->GeschossListe) {
			if (geschoss.is_alive()) {
				geschoss.get_bild().draw_rot(geschoss.get_x(), geschoss.get_y(), 1.0, 0.0);
			}			
		}

		//Rahmen von Panzern zeichnen zeichnen (für Entwicklungszwecke)
		/*for (Panzer panzer : this->PanzerListe) {
			vector<Position> Rahmen = panzer.erzeuge_Rahmen();
			for (Position pos : Rahmen) {
				Gosu::Graphics::draw_rect(pos.x, pos.y, 2, 2, Gosu::Color::YELLOW, 3.0);
			}
		}*/
	}


	void erzeuge_Geschoss(double x, double y, double angle) {
		Geschoss neues_Geschoss(x, y, angle, *this);
		this->GeschossListe.push_back(neues_Geschoss);
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

	bool istPositionInGeschoss(Position pos) {
		double toleranz = 2;
		for (Geschoss geschoss : this->GeschossListe) {
			double x_g = geschoss.get_x();
			double y_g = geschoss.get_y();
			if (pos.x <= x_g + toleranz && pos.x >= x_g - toleranz && pos.y <= y_g + toleranz && pos.y >= y_g - toleranz) {
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


//Funktionsdefinitionen für Panzer
void Panzer::act(void) {
	{
		//Teil für beide Spieler
		if (this->schussfreigabe > 0) {
			this->schussfreigabe--;
		}
		if (this->getroffen()) {
			this->gameOver();
		}

		//Spielerspezifischer Teil
		//Spieler 1 (grün)
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

		//Spieler 2 (rot)
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


vector<Position> Panzer::erzeuge_Rahmen() {
	double Aufloesung = 4;
	double hoehe = this->bild.height();
	double breite = this->bild.width();

	vector<Position> Rahmen;	

	Position ObenMitte = { this->x + sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0,  this->y - cos(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0 };
	Rahmen.push_back(ObenMitte);
	for (double d = 0; d < breite / 2; d = d + Aufloesung) {
		Rahmen.push_back({ ObenMitte.x + sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , ObenMitte.y - cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
		Rahmen.push_back({ ObenMitte.x - sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , ObenMitte.y + cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
	}

	Position UntenMitte = { this->x - sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0,  this->y + cos(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0 };
	Rahmen.push_back(UntenMitte);
	for (double d = 0; d < breite / 2; d = d + Aufloesung) {
		Rahmen.push_back({ UntenMitte.x + sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , UntenMitte.y - cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
		Rahmen.push_back({ UntenMitte.x - sin(Gosu::degrees_to_radians(-(90 - this->angle)))*d , UntenMitte.y + cos(Gosu::degrees_to_radians(-(90 - this->angle)))*d });
	}

	Position LinksMitte = { this->x + sin(Gosu::degrees_to_radians(-(90 - this->angle)))*breite / 2.0,  this->y - cos(Gosu::degrees_to_radians(-(90 - this->angle)))*breite / 2.0 };
	Rahmen.push_back(LinksMitte);
	for (double d = 0; d < hoehe / 2; d = d + Aufloesung) {
		Rahmen.push_back({ LinksMitte.x + sin(Gosu::degrees_to_radians(this->angle))*d,  LinksMitte.y - cos(Gosu::degrees_to_radians(this->angle))*d });
		Rahmen.push_back({ LinksMitte.x - sin(Gosu::degrees_to_radians(this->angle))*d,  LinksMitte.y + cos(Gosu::degrees_to_radians(this->angle))*d });
	}

	Position RechtsMitte = { this->x - sin(Gosu::degrees_to_radians(-(90 - this->angle)))*breite / 2.0,  this->y + cos(Gosu::degrees_to_radians(-(90 - this->angle)))*breite / 2.0 };
	Rahmen.push_back(RechtsMitte);
	for (double d = 0; d < hoehe / 2; d = d + Aufloesung) {
		Rahmen.push_back({ RechtsMitte.x + sin(Gosu::degrees_to_radians(this->angle))*d,  RechtsMitte.y - cos(Gosu::degrees_to_radians(this->angle))*d });
		Rahmen.push_back({ RechtsMitte.x - sin(Gosu::degrees_to_radians(this->angle))*d,  RechtsMitte.y + cos(Gosu::degrees_to_radians(this->angle))*d });
	}

	return Rahmen;
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

void Panzer::schuss() {
	if (this->schussfreigabe == 0) {
		double hoehe = this->bild.height();
		double breite = this->bild.width();
		this->Fenster.erzeuge_Geschoss(this->x + sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0, this->y - cos(Gosu::degrees_to_radians(-this->angle))*hoehe / 2.0, this->angle);
		this->schussfreigabe = this->nachladezeit;
	}
}

bool Panzer::getroffen() {
	vector<Position> Rahmen = this->erzeuge_Rahmen();
	
	for (Position pos : Rahmen) {
		if (this->Fenster.istPositionInGeschoss(pos)) {
			return true;
		}
	}
	return false;
}

void Panzer::gameOver() {
	this->alive = false;
}

//Funktionsdefinitionen für Geschoss
void Geschoss::act() {
	double radius = this->bild.height() / 2;

	//Bewegungsrichtung anpassen
	Position Oben = { this->x, this->y - radius };
	Position Unten = { this->x, this->y + radius };
	Position Links = { this->x - radius, this->y };
	Position Rechts = { this->x + radius, this->y };	

	if (this->Fenster.istPositionInMauer(Oben) || this->Fenster.istPositionInMauer(Unten)) {
		this->vy = this->vy *(-1.0);
	}
	if (this->Fenster.istPositionInMauer(Links) || this->Fenster.istPositionInMauer(Rechts)) {
		this->vx = this->vx *(-1.0);
	}

	//Bewegen
	this->x += this->vx;
	this->y += this->vy;

	//Lebenszeit ablaufen lassen
	if (this->timetolive > 0) {
		this->timetolive--;
	}
	else {
		this->alive = false;
	}
}