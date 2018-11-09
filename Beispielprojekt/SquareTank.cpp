#include "stdafx.h"

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

// Simulationsgeschwindigkeit
const double DT = 100.0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Beginn unser Programm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Enum für StateMachine
enum Zustand { Start, StartEnde, SpielfeldAufbauen, Spielen, Spielende, SpielendeEnde };

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
	double get_x(void) const { return this->x; };
	double get_y(void) const { return this->y; };

	Orientierung get_orientierung(void) const { return this->orientierung; };
	double get_hoehe(void) const { return this->hoehe; };
	double get_laenge(void) const { return this->laenge; };
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
	//Konstruktor für erste Erstellung
	Geschoss(double x, double y, double angle, GameWindow& f) : x(x), y(y), vx(vg*sin(Gosu::degrees_to_radians(angle))), vy(-vg*cos(Gosu::degrees_to_radians(angle))), Fenster(f)
	{
		this->timetolive = this->lebenserwartung;
	};

	//Konstruktor für Kopie
	Geschoss(double x, double y, double vx, double vy, double ttl, bool a, GameWindow& f) : x(x), y(y), vx(vx), vy(vy), timetolive(ttl), alive(a), Fenster(f)
	{
	};

	//Operator =
	Geschoss& operator=(const Geschoss& g) {
		Geschoss kopie(g.get_x(), g.get_y(), g.get_vx(), g.get_vy(), g.get_ttl(), g.is_alive(), g.get_Fenster());
	};

	//Deklaration ausgelagerter Funktionen
	void act();

	//Getter und Setter
	Gosu::Image get_bild(void) const{ return this->bild; };

	GameWindow& get_Fenster(void) const { return this->Fenster; };

	double get_x(void) const{ return this->x; };
	double get_y(void) const{ return this->y; };
	double get_vx(void) const { return this->vx; };
	double get_vy(void) const { return this->vy; };
	
	double get_ttl(void) const { return this->timetolive; };
	bool is_alive(void) const{ return this->alive; };
};


class Panzer
{	
	const double vg = 3;
	const double vrot = 2;
	const int16_t munitionskapazitaet = 10;
	const int16_t nachladezeit = 1 * 60;
	Gosu::Sample schussSound = Gosu::Sample::Sample("schuss.wav");

	Gosu::Image bild;
	int16_t spielernr;
	GameWindow& Fenster;
	
	double x;
	double y;
	double angle;

	int16_t munition = this->munitionskapazitaet;
	int16_t schussfreigabe = 0;
	bool alive = true;	

	int16_t punkte = 0;

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

	void positionieren(double x, double y, double a) {
		this->x = x;
		this->y = y;
		this->angle = a;
	};

	void sieg(void) {
		this->punkte++;
	};

	//Deklaration ausgelagerter Funktionen
	vector<Position> erzeuge_Rahmen();
	bool touchiertMauer();
	void schuss();
	bool getroffen();

	void act();	

	//Getter und Setter
	int16_t get_spielernr() const { return this->spielernr; };
	GameWindow& get_Fenster() const { return this->Fenster; };
	Gosu::Image get_bild(void) const { return this->bild; };

	double get_x(void) const { return this->x; };
	double get_y(void) const { return this->y; };
	double get_angle(void) const { return this->angle; };
	int16_t get_punkte(void) {  
		this->munition = this->munitionskapazitaet;
		return this->punkte;
	};
	bool is_alive(void) const { return this->alive; };
};


class GameWindow : public Gosu::Window
{
	Gosu::Image Startbildschirm = Gosu::Image("Startbildschirm.png");
	Gosu::Image Zwischenbildschirm = Gosu::Image("Zwischenbildschirm.png");

	Gosu::Font PunkteStandSpieler1 = Gosu::Font(70);
	Gosu::Font PunkteStandSpieler2 = Gosu::Font(70);

	Zustand zustand = Start;

	Panzer Spieler1 = Panzer(0.0, 0.0, 0.0, 1, *this);
	Panzer Spieler2 = Panzer(0.0, 0.0, 0.0, 2, *this);

	vector<Mauer> MauernListe;

	vector<Geschoss> GeschossListe;
	
	//Private Methoden
	void MauerErzeugen(double x, double y, Orientierung o, double l) {
		this->MauernListe.push_back(Mauer(x, y, o, l));
	}

	int16_t level = 1;

public:
	GameWindow(): Window(720, 720)
	{
		set_caption("SquareTank");
	}

	// wird bis zu 60 Mal pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinkt
	void update() override
	{
		int level = 1;
		if (this->zustand == Start) {
			if (this->input().down(Gosu::KB_S)) {
				this->zustand = StartEnde;
			}			
		}
		if (this->zustand == StartEnde) {
			if (!this->input().down(Gosu::KB_S)) {
				this->zustand = SpielfeldAufbauen;
			}
		}

		if (this->zustand == SpielfeldAufbauen) {
			if (this->level == 1)
			{//Rahmen
				MauerErzeugen(0.0, 0.0, horizontal, 720.0);
				MauerErzeugen(0.0, 710.0, horizontal, 720.0);
				MauerErzeugen(0.0, 0.0, vertikal, 720.0);
				MauerErzeugen(710.0, 0.0, vertikal, 720.0);

				//Rest
				MauerErzeugen(0.0, 142.0, horizontal, 578.0);
				MauerErzeugen(142.0, 284.0, horizontal, 578.0);
				MauerErzeugen(0.0, 426.0, horizontal, 578.0);
				MauerErzeugen(142.0, 568.0, horizontal, 578.0);

				//Panzer positionieren
				Spieler1.positionieren(76.0, 76.0, 90.0);
				Spieler2.positionieren(644.0, 644.0, 270.0);
				this->zustand = Spielen;
			}
			if (this->level == 2)
			{
				//Rahmen
				MauerErzeugen(0.0, 0.0, horizontal, 720.0);
				MauerErzeugen(0.0, 710.0, horizontal, 720.0);
				MauerErzeugen(0.0, 0.0, vertikal, 720.0);
				MauerErzeugen(710.0, 0.0, vertikal, 720.0);

				//Rest
				MauerErzeugen(0.0, 142.0, horizontal, 294.0);
				MauerErzeugen(426.0, 142.0, horizontal, 294.0);
				MauerErzeugen(132.0, 284.0, horizontal, 456.0);
				MauerErzeugen(0.0, 426.0, horizontal, 294.0);
				MauerErzeugen(426.0, 426.0, horizontal, 294.0);
				MauerErzeugen(132.0, 568.0, horizontal, 456.0);

				//Panzer positionieren
				Spieler1.positionieren(76.0, 76.0, 90.0);
				Spieler2.positionieren(644.0, 644.0, 270.0);
				this->zustand = Spielen;
			}
			if (this->level == 3)
			{
				//Rahmen
				MauerErzeugen(0.0, 0.0, horizontal, 720.0);
				MauerErzeugen(0.0, 710.0, horizontal, 720.0);
				MauerErzeugen(0.0, 0.0, vertikal, 720.0);
				MauerErzeugen(710.0, 0.0, vertikal, 720.0);

				//Rest
				MauerErzeugen(147.0, 0.0, vertikal, 71.0);
				MauerErzeugen(284.0, 71.0, vertikal, 71.0);

				MauerErzeugen(563.0, 0.0, vertikal, 71.0);
				MauerErzeugen(426.0, 71.0, vertikal, 71.0);

				MauerErzeugen(132.0, 497.0, vertikal, 71.0);
				MauerErzeugen(578.0, 497.0, vertikal, 71.0);

				MauerErzeugen(284.0, 426.0, vertikal, 71.0);
				MauerErzeugen(426.0, 426.0, vertikal, 71.0);

				MauerErzeugen(0.0, 142.0, horizontal, 294.0);
				MauerErzeugen(426.0, 142.0, horizontal, 294.0);
				MauerErzeugen(132.0, 284.0, horizontal, 456.0);
				MauerErzeugen(0.0, 426.0, horizontal, 294.0);
				MauerErzeugen(426.0, 426.0, horizontal, 294.0);
				MauerErzeugen(132.0, 568.0, horizontal, 456.0);

				

				//Panzer positionieren
				Spieler1.positionieren(76.0, 76.0, 90.0);
				Spieler2.positionieren(644.0, 644.0, 270.0);
				this->zustand = Spielen;
			}
			if (this->level == 4)
			{
				//Rahmen
				MauerErzeugen(0.0, 0.0, horizontal, 720.0);
				MauerErzeugen(0.0, 710.0, horizontal, 720.0);
				MauerErzeugen(0.0, 0.0, vertikal, 720.0);
				MauerErzeugen(710.0, 0.0, vertikal, 720.0);

				//Rest
				MauerErzeugen(147.0, 0.0, vertikal, 71.0);
				MauerErzeugen(284.0, 71.0, vertikal, 71.0);

				MauerErzeugen(563.0, 0.0, vertikal, 71.0);
				MauerErzeugen(426.0, 71.0, vertikal, 71.0);

				MauerErzeugen(132.0, 497.0, vertikal, 71.0);
				MauerErzeugen(578.0, 497.0, vertikal, 71.0);

				MauerErzeugen(284.0, 426.0, vertikal, 71.0);
				MauerErzeugen(426.0, 426.0, vertikal, 71.0);

				//TAKTIK MAUERN: Panzer kommt nicht durch, Kugeln schon. Sollen auf der 2. und 3. horiz. Ebene liegen
				
				MauerErzeugen(578.0, 167.0, vertikal, 102.0);
				MauerErzeugen(132.0, 309.0, vertikal, 102.0);


				//Mauern für Durchschlupf
				MauerErzeugen(132.0, 142.0, vertikal, 102.0);
				MauerErzeugen(578.0, 334.0, vertikal, 102.0);


				MauerErzeugen(0.0, 142.0, horizontal, 294.0);
				MauerErzeugen(426.0, 142.0, horizontal, 294.0);
				MauerErzeugen(132.0, 284.0, horizontal, 456.0);
				MauerErzeugen(0.0, 426.0, horizontal, 294.0);
				MauerErzeugen(426.0, 426.0, horizontal, 294.0);
				MauerErzeugen(132.0, 568.0, horizontal, 456.0);



				//Panzer positionieren
				Spieler1.positionieren(76.0, 76.0, 90.0);
				Spieler2.positionieren(644.0, 644.0, 270.0);
				this->zustand = Spielen;
			}

		}


		if (this->zustand == Spielen) {

			Spieler1.act();
			Spieler2.act();
			
			vector<Geschoss> GeschossListeAlt = this->GeschossListe;
			int anzahlGeschosse = this->GeschossListe.size();
			for (int i = 0; i < anzahlGeschosse; i++) {
				this->GeschossListe.pop_back();
			}
			for (Geschoss geschoss : GeschossListeAlt) {
				if (geschoss.is_alive()) {
					this->GeschossListe.push_back(geschoss);
				}
			}
			for (Geschoss& geschoss : this->GeschossListe) {
				geschoss.act();
			}
		}
		if (this->zustand == Spielende) {
			//Alle Geschosse löschen
			int anzahlGeschosse = this->GeschossListe.size();
			for (int i = 0; i < anzahlGeschosse; i++) {
				this->GeschossListe.pop_back();
			}

			//Alle Mauern löschen
			int anzahlMauern = this->MauernListe.size();
			for (int i = 0; i < anzahlMauern; i++) {
				this->MauernListe.pop_back();
			}
			if (this->input().down(Gosu::KB_N)) {
				this->zustand = SpielendeEnde;
			}
		}
		if (this->zustand == SpielendeEnde) {
			if (!this->input().down(Gosu::KB_N)) {
				this->level++;
				if (this->level == 5) {
					this->level = 1;
				}
				this->zustand = SpielfeldAufbauen;
			}
		}
	}

	void draw() override
	{
		if (this->zustand == Start||this->zustand==StartEnde) {
			this->Startbildschirm.draw_rot(360, 360, 0.0, 0.0);
		}

		if (this->zustand == Spielen) {
			//Hintergrund zeichnen
			Gosu::Graphics::draw_rect(0.0, 0.0, this->width(), this->height(), Gosu::Color::WHITE, 0.0);

			//Mauern zeichnen
			for (Mauer& mauer : this->MauernListe) {
				Gosu::Graphics::draw_rect(mauer.get_x(), mauer.get_y(), mauer.get_laenge(), mauer.get_hoehe(), Gosu::Color::BLACK, 2.0);
			}

			//Panzer zeichnen
			Spieler1.get_bild().draw_rot(Spieler1.get_x(), Spieler1.get_y(), 1.0, Spieler1.get_angle());
			Spieler2.get_bild().draw_rot(Spieler2.get_x(), Spieler2.get_y(), 1.0, Spieler2.get_angle());

			//Geschosse zeichnen
			for (Geschoss& geschoss : this->GeschossListe) {
				geschoss.get_bild().draw_rot(geschoss.get_x(), geschoss.get_y(), 1.0, 0.0);		
			}

			//Rahmen von Panzern zeichnen zeichnen (für Entwicklungszwecke)
			/*for (Panzer panzer : this->PanzerListe) {
				vector<Position> Rahmen = panzer.erzeuge_Rahmen();
				for (Position pos : Rahmen) {
					Gosu::Graphics::draw_rect(pos.x, pos.y, 2, 2, Gosu::Color::YELLOW, 3.0);
				}
			}*/
		}
		if (this->zustand == Spielende|| this->zustand == SpielendeEnde) {
			this->Zwischenbildschirm.draw_rot(360, 360, 0.0, 0.0);
			this->PunkteStandSpieler1.draw(to_string(this->Spieler1.get_punkte()), 290.0, 305.0, 2.0, 1.0, 1.0, Gosu::Color::BLACK);
			this->PunkteStandSpieler1.draw(to_string(this->Spieler2.get_punkte()), 400.0, 305.0, 2.0, 1.0, 1.0, Gosu::Color::BLACK);
		}
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

	bool beruehrtAnderenPanzer(Panzer& frager) {
		double toleranz = 1;
		vector<Position> fragerRahmen = frager.erzeuge_Rahmen();
		vector<Position> andererRahmen;
		if (frager.get_spielernr() == 1) {
			andererRahmen = this->Spieler2.erzeuge_Rahmen();
		}
		else if (frager.get_spielernr() == 2) {
			andererRahmen = this->Spieler1.erzeuge_Rahmen();
		}
		for (Position posF : fragerRahmen) {
			double xf = posF.x;
			double yf = posF.y;
			for (Position posA : andererRahmen) {
				if (posA.x <= xf + toleranz && posA.x >= xf - toleranz && posA.y <= yf + toleranz && posA.y >= yf - toleranz) {
					return true;
				}
			}
		}
		return false;
	}

	void spielende(Panzer& verlierer) {
		if (verlierer.get_spielernr() == 1) {
			Spieler2.sieg();
		}
		else if (verlierer.get_spielernr() == 2) {
			Spieler1.sieg();
		}

		//Geschosse löschen
		int anzahlGeschosse = this->GeschossListe.size();
		for (int i = 0; i < anzahlGeschosse; i++) {
			this->GeschossListe.pop_back();
		}

		//Mauern löschen
		int anzahlMauern = this->MauernListe.size();
		for (int i = 0; i < anzahlMauern; i++) {
			this->MauernListe.pop_back();
		}

		this->zustand = Spielende;
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
			this->alive = false;
			this->Fenster.spielende(*this);
		}

		//Spielerspezifischer Teil
		//Spieler 1 (grün)
		if (this->spielernr == 1) {
			if (this->Fenster.input().down(Gosu::KB_W)) {
				this->x += sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y += -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer()||this->Fenster.beruehrtAnderenPanzer(*this)) {
					this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
					this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_S)) {
				this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
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
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
					this->angle = angle_old;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_D)) {
				double angle_old = this->angle;
				this->angle += this->vrot;
				if (this->angle > 360.0) {
					this->angle = this->angle - 360.0;
				}
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
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
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
					this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
					this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_DOWN)) {
				this->x -= sin(Gosu::degrees_to_radians(this->angle))*this->vg;
				this->y -= -cos(Gosu::degrees_to_radians(this->angle))*this->vg;
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
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
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
					this->angle = angle_old;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_RIGHT)) {
				double angle_old = this->angle;
				this->angle += this->vrot;
				if (this->angle > 360.0) {
					this->angle = this->angle - 360.0;
				}
				if (this->touchiertMauer() || this->Fenster.beruehrtAnderenPanzer(*this)) {
					this->angle = angle_old;
				}
			}
			if (this->Fenster.input().down(Gosu::KB_ENTER)) {
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
	if (this->schussfreigabe == 0 && this->munition>0) {
		this->schussSound.play();
		double hoehe = this->bild.height();
		double breite = this->bild.width();
		this->Fenster.erzeuge_Geschoss(this->x + sin(Gosu::degrees_to_radians(this->angle))*hoehe / 2.0, this->y - cos(Gosu::degrees_to_radians(-this->angle))*hoehe / 2.0, this->angle);
		this->schussfreigabe = this->nachladezeit;
		this->munition--;
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

//Funktionsdefinitionen für Geschoss
void Geschoss::act() {
	double radius = this->bild.height() / 2;

	//Bewegungsrichtung anpassen
	Position Oben = { this->x, this->y - radius };
	Position Unten = { this->x, this->y + radius };
	Position Links = { this->x - radius, this->y };
	Position Rechts = { this->x + radius, this->y };	

	if (this->Fenster.istPositionInMauer(Oben) != this->Fenster.istPositionInMauer(Unten)) {
		this->vy = this->vy *(-1.0);
	}
	if (this->Fenster.istPositionInMauer(Links) != this->Fenster.istPositionInMauer(Rechts)) {
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