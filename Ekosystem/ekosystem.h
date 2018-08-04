#pragma once
#include <string>
#include <fstream>//do operacji na plikach
#include <sstream>//sk³adanie nazwy pliku w stringu
#include <iomanip>//dla manipulatorów z parametrami - sk³adanie nazwy pliku w stringu
#include <vector>//do wczytywania sta³ych z pliku 
#include <windows.h>//potrzebne do funkcji sleep
using namespace std;
enum POLE{NIC=0,ROSL=177,SKALA=219,RZER=79,UKRYTY=111,MZER=42,ZWLOKI=197,ERR=32};
enum REZULTAT{rNIC=0,rZJEDZENIE,rNOWY,rSMIERC};
enum STAN_T{sNORMAL=0,sSTOP=1,sMLODE=2};
struct EFEKT
{
	REZULTAT enRezultat;
	int X;
	int Y;
};
struct CONSTS_T
{
	const int MAP_Y = 20; //MROZMY = 20;//MAP-Y
	const int MAP_X = 20; // MROZMX = 30;//MAP-X
	const int DELAY = 200; // PRZERWA = 200;//DELAY
	const int ROCK_1 = 10; // SKALAPRZYROSTKOND = 10;//przyrost kondycji rosliny na skalach---------ROCK-1
	const int PLAN_1 = 10; // ROSLINAPRZYROSTKOND = 10;//przyrost kondycji rosliny od fotosyntezy (czasu)-----------PLAN-1
	const int HERB_1 = 2; // RZERCASPADEKKOND = 2;//spadek kondycji roslinozercy od czasu-------------HERB-1
	const int PRED_1 = 2; // MZERCASPADEKKOND = 2;//spadek kondycji miêso¿ercy od czasu--------------PRED-1
	const int PLAN_2 = 20; // ROSLINAPROCLIMIT = 20;//ograniczenie rozrastania sie roœlin - procent powierzchni mapy-------PLAN-2
	const int HERB_3 = 40; // ROSLINAKALORIE = 40;//przyrost kondycji roslinozercy po zjedzeniu rosliny---------HERB-3
	const int PRED_3 = 20; // RZERCAKALORIE = 20;//przyrost kondycji drapieznika po zjedzeniu roslinozercy--------PRED-3
	const int PLAN_4 = 10; // ROSLINASTARTKONDYCJA = 10;//PLAN-4
	const int HERB_4 = 20; // RZERCASTARTKONDYCJA = 20;//HERB-4
	const int PRED_4 = 20; // MZERCASTARTKONDYCJA = 20;//PRED-4
	const int PLAN_5 = 0; // ROSLROZMNSPADEKKOND = 0;//spadek kondycji rosliny zwiazany z rozmnazaniem-----------PLAN-5
	const int HERB_5 = 0; // RZERROZMNSPADEKKOND = 0;//spadek kondycji roslinozercy zwiazany z porodem-----------HERB-5
	const int PRED_5 = 0; // MZERROZMNSPADEKKOND = 0;//spadek kondycji drapieznika zwiazany z porodem------------PRED-5
	const int PLAN_6 = 10; //ROSLCZASDOROZM = 10;//odstep czasowy miedzy owocowaniem----------------PLAN_6
	const int HERB_6 = 10; //RZERCZASDOROZM = 10;//odstep czasowy miêdzy urodzeniami----------------HERB-6
	const int PRED_6 = 10; //MZERCZASDOROZM = 10;//odstep czasowy miêdzy urodzeniami----------------PRED-6
	const int WRITE = 0;//ZAPISDANYCH = true;//zapis do pliku--------------------------WRITE
	//konstruktor bezparametrowy:
	CONSTS_T();
	//konstruktor parametrowy:
	CONSTS_T(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int,int);
};

class CMapa;//deklaracja zapowiadaj¹ca
class CMagazyn;//deklaracja zapowiadaj¹ca
class CProgram
{
private:
	static CProgram* pProgram;
	CProgram();//prywatny konstruktor - wywo³ywany przez publiczn¹, statyczn¹ metodê
	CONSTS_T* pCONSTS;
	CMapa* pMapa=nullptr;
	CMagazyn* pMagazyn = nullptr;
	ofstream StrumZapis;
	string Naglowek = "time;plants;herbivores;predators";
	int LicznikPetli = 0;
	bool FindVal(string, vector<string>&, int&);//metoda szukaj¹ca zmiennej int w vectorze stringów
public:
	~CProgram();//destruktor - deklaracja
	static CProgram* Obiekt()
	{
		if (pProgram == nullptr){pProgram=new CProgram;}//jesli NULL, to utwórz nowy obiekt
		return pProgram;
	}
	void Otworz();//zasadnicza czesc programu - tutaj tylko deklaracja
	bool CheckStr(std::string CheckedStr);//sprawdza, czy wprowadzony tekst jest liczb¹
	void DanePoczatkowe();
	void AktualizacjaMapy();//poczatkowe ustawienie obiektow na mapie
	void RozstawObiekty();
	bool OtworzPlikZapisu();//otwarcie pliku i nazwanie kolumn
	bool Zapis();//zapis danych do pliku
	bool ReadConsts();//odczyt sta³ych z pliku
};
class CMapa
{
private:
	POLE** ppTablica;
	int iRozmY;
	int iRozmX;
public:
	CMapa(int sizeY, int sizeX);//konstruktor parametrowy
	~CMapa();//destruktor
	void Pokaz();
	int RozmiarX();
	int RozmiarY();
	bool Ustaw(POLE typ,int destX, int destY);
	POLE Element(int indX, int indY);
};
class COrganizm;//deklaracja zapowiadaj¹ca
class CMagazyn
{
private:
	COrganizm** pTablica;
	static CONSTS_T* pCONSTS;
	unsigned uIlosc;
public:
	int iIloscSkal = 0;
	int iIloscRoslin = 0;
	int iIloscRzercow = 0;
	int iIloscMzercow = 0;
	friend class CProgram;//aby *pProgram móg³ ustawiæ magazynowi wskaŸnik do sta³ych
	CMagazyn();
	~CMagazyn();
	void Dodaj(POLE typ, unsigned destX, unsigned destY);
	void Usun(unsigned index);
	unsigned Ilosc() { return uIlosc; }//INLINE
	unsigned JakiRozmiar();
	COrganizm* Element(unsigned index);
	void Cykl();
};
class COrganizm
{
protected:
	POLE ident;
	int iPozX;
	int iPozY;
	unsigned uKierunek;//liczba od 0 do 7, 0==do góry,1==do góry i w prawo itd.
	unsigned uKondycja;
	STAN_T stan;
	int iDorozm;//czas pozosta³y do rozmno¿enia
	bool Szukaj(POLE typ, int& ZnalX,int& ZnalY);
	static CMapa* pMapa;
	static CMagazyn* pMagazyn;
	static CONSTS_T* pCONSTS;
public:
	POLE Ident() { return ident; }//INLINE
	int X() { return iPozX; }//INLINE
	int Y() { return iPozY; }//INLINE
	unsigned Dir() { return uKierunek; }//INLINE
	unsigned Kondycja() { return uKondycja; }//INLINE
	STAN_T Stan() { return stan; }//INLINE
	void UstawDir(unsigned dir);
	void UstawXY(int destX, int destY);
	void Ruch();
	friend class CProgram;//aby *pProgram móg³ ustawiæ organizmom wskaŸniki do mapy i do magazynu
	COrganizm(POLE id,unsigned kond):ident(id),uKondycja(kond),stan(sMLODE){}
	virtual ~COrganizm() {}
	virtual EFEKT Akcja()=0;//metoda czysto wirtualna - COrganizm to klasa abstrakcyjna
};
class CSkala :public COrganizm
{
public:
	CSkala(POLE id, unsigned kond) :COrganizm(id, kond)
	{
		++pMagazyn->iIloscSkal;
		iDorozm = pCONSTS->PLAN_6;
	}
	~CSkala()
	{
		--pMagazyn->iIloscSkal;
	}
	EFEKT Akcja()override;
};
class CRoslina :public COrganizm
{
public:
	CRoslina(POLE id, unsigned kond):COrganizm(id,kond)
	{ 
		//ident = ROSL;
		//uKondycja = ROSLINASTARTKONDYCJA;
		iDorozm = pCONSTS->PLAN_6;
		++pMagazyn->iIloscRoslin;
	}
	~CRoslina()
	{
		--pMagazyn->iIloscRoslin;
	}
	EFEKT Akcja()override;
};
class CRzerca : public COrganizm
{
public:
	CRzerca(POLE id, unsigned kond) :COrganizm(id, kond)
	{
		//ident = RZER;
		//uKondycja = RZERCASTARTKONDYCJA;
		iDorozm =pCONSTS->HERB_6;
		++pMagazyn->iIloscRzercow;
	}
	~CRzerca()
	{
		--pMagazyn->iIloscRzercow;
	}
	EFEKT Akcja()override;
};
class CMzerca :public COrganizm
{
public:
	CMzerca(POLE id, unsigned kond):COrganizm(id,kond)
	{ 
		//ident = MZER;
		//uKondycja = MZERCASTARTKONDYCJA;
		iDorozm =pCONSTS->PRED_6;
		++pMagazyn->iIloscMzercow;
	}
	~CMzerca()
	{
		--pMagazyn->iIloscMzercow;
	}
	EFEKT Akcja()override;
};
