#include <string>
#include <conio.h>
#include <iostream>
#include <memory.h>//potrzebne do memcpy()
#include <ctime>//potrzebne do generatora liczb losowych
#include "ekosystem.h"
using namespace std;
CProgram::CProgram()
{
	pProgram = this;
	
}
void CProgram::Otworz()
{
	ReadConsts();//czyta ustawienia z pliku i tworzy obiekt CONSTS_T, ustawia na niego wskaŸnik
	if (pCONSTS == nullptr)//nie uda³o siê odczytaæ ustawieñ z pliku
	{
		pCONSTS = new CONSTS_T;
	}
	COrganizm::pCONSTS = pCONSTS;//ustawienie analogicznego wskaŸnika w organizmach
	CMagazyn::pCONSTS = pCONSTS;//ustawienie analogicznego wskaŸnika w magazynie
	if (!pMagazyn)
	{
		pMagazyn = new CMagazyn;//tworzy nowy obiekt CMagazyn i ustawia na niego wskaŸnik
		COrganizm::pMagazyn = pMagazyn;
	}
	if (!pMapa)
	{
		pMapa = new CMapa(pCONSTS->MAP_Y,pCONSTS->MAP_X);//tworzy nowy obiekt CMapa i ustawia na niego wskaŸnik
		COrganizm::pMapa = pMapa;
	}
	cout << "**********          Ekosystem          **********" << endl << endl;
	DanePoczatkowe();
	if (static_cast<bool>(pCONSTS->WRITE))
	{
		if (!OtworzPlikZapisu())
		{
			cout << "File for data recording could not be opened." << endl;
		}
	}
	RozstawObiekty();
	AktualizacjaMapy();
	Sleep(1000);//czas na pokazanie ewentualnych komunikatów


	//----------------------------------podstawowa petla - cykl symulacji--------------------------------------
	for (;;)
	{
		pMapa->Pokaz();
		cout << "\nTotal number of plants : " << (pMagazyn->iIloscRoslin+pMagazyn->iIloscSkal);
		cout << "\nNumber of herbivores   : " << pMagazyn->iIloscRzercow;
		cout << "\nNumber of predators    : " << pMagazyn->iIloscMzercow << endl;
		if (static_cast<bool>(pCONSTS->WRITE))
		{
			if (Zapis() == false) { cout << "\nData recording did not succeed."; }
		}
		Sleep(pCONSTS->DELAY);
		pMagazyn->Cykl();
		char Znak;
		int iZnak=0;
		while (kbhit())
		{
			Znak = _getch();
			iZnak = static_cast<int>(Znak);
		}
		if (iZnak == 27) { break; }//wcisnieto ESC - wyjscie
		if (iZnak == 32)//test
		{
			int b = 0;
			/*pMagazyn->Usun(5);
			for (int i = 0; i <= pMapa->RozmiarY() - 1; ++i)
			{
				for (int j = 0; j <= pMapa->RozmiarX() - 1; ++j)
				{
					pMapa->Ustaw(NIC, j, i);
				}
			}
			AktualizacjaMapy();*/
			//koniec testu
		}
		++LicznikPetli;
	}
	system("cls");
	cout << "**********          by Andrzej Rzoska 4-08-2018          **********" << endl;
	Sleep(2000);
	

	
	//_getch();
	//TEST
	/*pMagazyn->Usun(3);
	for (int i = 0; i <= pMapa->RozmiarY() - 1; ++i)
	{
		for (int j = 0; j <= pMapa->RozmiarX() - 1; ++j)
		{
			pMapa->Ustaw(NIC, j, i);
		}
	}
	AktualizacjaMapy();
	pMapa->Pokaz();
	_getch();*/
	//koniec TESTU
}
bool CProgram::CheckStr(string CheckedStr)
{
	for (int i = 0; i <= CheckedStr.length() - 1; ++i)
	{
		if ((static_cast<int>(CheckedStr[i]) < 48) || (static_cast<int>(CheckedStr[i]) > 57))
			return false;
	}
	return true;
}
void CProgram::DanePoczatkowe()
{
	for (;;)
	{
		string input;
		cout << "Type initial number of plants on rocks [ " << static_cast<char>(SKALA) << " ]: ";
		cin >> input;
		if (!CheckStr(input)) { cout << "typed text is not a number!" << endl; }
		else
		{
			pMagazyn->iIloscSkal = atoi(input.c_str());
			break;
		}
	}
	for(;;)
	{
		string input;
		cout << "Type initial number of remaining plants [ "<<static_cast<char>(ROSL)<<" ]: ";
		cin >> input;
		if (!CheckStr(input)) { cout << "typed text is not a number!" << endl; }
		else 
		{
			pMagazyn->iIloscRoslin = atoi(input.c_str());
			break;
		}
	}
	for (;;)
	{
		string input;
		cout << "Type initial number of herbivores [ " << static_cast<char>(RZER) << " ]: ";
		cin >> input;
		if (!CheckStr(input)) { cout << "typed text is not a number!" << endl; }
		else
		{
			pMagazyn->iIloscRzercow = atoi(input.c_str());
			break;
		}
	}
	for (;;)
	{
		string input;
		cout << "Type initial number of predators [ " << static_cast<char>(MZER) << " ]: ";
		cin >> input;
		if (!CheckStr(input)) { cout << "typed text is not a number!" << endl; }
		else
		{
			pMagazyn->iIloscMzercow = atoi(input.c_str());
			break;
		}
	}
}
void CProgram::AktualizacjaMapy()
{
	for (unsigned i = 0; i < pMagazyn->JakiRozmiar(); ++i)
	{
		pMapa->Ustaw(pMagazyn->Element(i)->Ident(), pMagazyn->Element(i)->X(), pMagazyn->Element(i)->Y());
	}
}
void CProgram::RozstawObiekty()
{
	int skaly = pMagazyn->iIloscSkal;
	pMagazyn->iIloscSkal = 0;
	srand(static_cast<unsigned>(time(NULL)));//zainicjowanie generatora liczb losowych
											 //dodawanie roslin
	while (pMagazyn->iIloscSkal<skaly)
	{
		//sprawdzenie,czy nowy obiekt zmiesci sie na mapie
		if (pMagazyn->Ilosc()+1 >(pMapa->RozmiarX()*pMapa->RozmiarY())) { break; }
		//losowanie wspó³rzêdnych
		int strzalX;
		int strzalY;
		do
		{
			strzalX = (rand() % (pMapa->RozmiarX()));
			strzalY = (rand() % (pMapa->RozmiarY()));
		} while ((pMapa->Element(strzalX, strzalY) != NIC));
		pMagazyn->Dodaj(SKALA, strzalX, strzalY);
		if (!(pMapa->Ustaw(SKALA, strzalX, strzalY)))
		{ 
			cout << "drawn coordinates are out of map" << endl;
		}
	}
	int rosliny = pMagazyn->iIloscRoslin;
	pMagazyn->iIloscRoslin = 0;
	srand(static_cast<unsigned>(time(NULL)));//zainicjowanie generatora liczb losowych
	//dodawanie roslin
	while ( pMagazyn->iIloscRoslin<rosliny)
	{
		//sprawdzenie,czy nowy obiekt zmiesci sie na mapie
		if (pMagazyn->Ilosc() + 1 > (pMapa->RozmiarX()*pMapa->RozmiarY())) { break; }
		//losowanie wspó³rzêdnych
		int strzalX;
		int strzalY;
		do
		{
			strzalX = (rand() % (pMapa->RozmiarX()));
			strzalY = (rand() % (pMapa->RozmiarY()));
		} while ((pMapa->Element(strzalX, strzalY) != NIC));
		pMagazyn->Dodaj(ROSL, strzalX, strzalY);
		if (!(pMapa->Ustaw(ROSL, strzalX, strzalY))) { cout << "drawn coordinates are out of map" << endl; }
	}
	//dodawanie roslinozercow
	int roslinozercy = pMagazyn->iIloscRzercow;
		pMagazyn->iIloscRzercow = 0;
	while(pMagazyn->iIloscRzercow<roslinozercy)
	{
		//sprawdzenie,czy nowy obiekt zmiesci sie na mapie
		if (pMagazyn->Ilosc() + 1 >(pMapa->RozmiarX()*pMapa->RozmiarY())) { break; }
		//losowanie wspó³rzêdnych
		int strzalX;
		int strzalY;
		do
		{
			strzalX = (rand() % (pMapa->RozmiarX()));
			strzalY = (rand() % (pMapa->RozmiarY()));
		} while ((pMapa->Element(strzalX, strzalY) != NIC));
		pMagazyn->Dodaj(RZER, strzalX, strzalY);
		if (!(pMapa->Ustaw(RZER, strzalX, strzalY))) { cout << "drawn coordinates are out of map" << endl; }
	
	}
	//dodawanie drapieznikow
	int miesozercy = pMagazyn->iIloscMzercow;
	pMagazyn->iIloscMzercow = 0;
	while (pMagazyn->iIloscMzercow < miesozercy)
	{
		//sprawdzenie,czy nowy obiekt zmiesci sie na mapie
		if (pMagazyn->Ilosc() + 1 >(pMapa->RozmiarX()*pMapa->RozmiarY())) { break; }
		//losowanie wspó³rzêdnych
		int strzalX;
		int strzalY;
		do
		{
			strzalX = (rand() % (pMapa->RozmiarX()));
			strzalY = (rand() % (pMapa->RozmiarY()));
		} while ((pMapa->Element(strzalX, strzalY) != NIC));
		pMagazyn->Dodaj(MZER, strzalX, strzalY);
		if (!(pMapa->Ustaw(MZER, strzalX, strzalY))) { cout << "drawn coordinates are out of map" << endl; }
	}
}
bool CProgram::OtworzPlikZapisu()
{
	ifstream sprawdzacz;
	ostringstream nazwapliku;
	for (int i = 1;; ++i)
	{
		nazwapliku.str("data");//nowa tresc
		nazwapliku.seekp(0, ios::end);
		nazwapliku << setw(2)<<setfill('0') << i << ".csv";
		sprawdzacz.open(nazwapliku.str());//sprawdzamy, czy ten plik ju¿ istnieje
		if (!sprawdzacz.good())
		{
			break;//nie ma jeszcze takiego pliku - nazwa jest ok, wychodzimy
		}
		else//plik ju¿ istnieje i jest teraz otwarty
		{
			sprawdzacz.close();
		}
	}
	//otwieramy teraz plik strumieniem wychodz¹cym
	StrumZapis.open(nazwapliku.str());
	if (!StrumZapis) { return false; }//b³¹d otwarcia pliku
	StrumZapis << Naglowek;
	return true;
}
bool CProgram::Zapis()
{

	if (StrumZapis.is_open())
	{
		StrumZapis<<'\n'<<LicznikPetli<<';'<<(pMagazyn->iIloscSkal+pMagazyn->iIloscRoslin)<<';'<<pMagazyn->iIloscRzercow<<';'<<pMagazyn->iIloscMzercow;
		if (StrumZapis) { return true; }//zapis uda³ siê
	}
	return false;//zapis siê nie uda³
}
CONSTS_T::CONSTS_T()
{
}
CONSTS_T::CONSTS_T(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10,
	int v11, int v12, int v13, int v14, int v15, int v16, int v17, int v18, int v19,int v20):
	MAP_Y(v1),MAP_X(v2),DELAY(v3),ROCK_1(v4),PLAN_1(v5),HERB_1(v6),
	PRED_1(v7),PLAN_2(v8),HERB_3(v9),PRED_3(v10),PLAN_4(v11),HERB_4(v12),
	PRED_4(v13),PLAN_5(v14),HERB_5(v15),PRED_5(v16),PLAN_6(v17),HERB_6(v18),PRED_6(v19),WRITE(v20)
{}
bool CProgram::ReadConsts()
{
	ifstream ifstrum("settings.txt");
	if (!ifstrum) { return false; }//jeœli nie uda³o siê otwarcie pliku - zwracamy false
	//tworzymy vector stringów
	vector<string> slowa;
	string slowo;
	do
	{
		ifstrum >> slowo;
		if ((!ifstrum)||(slowo.empty())) { break; }//jeœli dotar³ do koñca, pojawi siê EOF, lub jesli nic nie wyczyta³, wychodzimy z pêtli
		slowa.push_back(slowo);
	} while (1);//czyli zawsze
	CONSTS_T stale;//tymczasowe stale programu - jeœli wartoœci nie zostan¹ znalezione w pliku, u¿yjemy domyœlnych
	int zmienna;//tymczasowa zmienna int
	int map_y = (FindVal("MAP_Y",slowa,zmienna) ? zmienna : stale.MAP_Y);
	int map_x = (FindVal("MAP_X", slowa, zmienna) ? zmienna : stale.MAP_X);
	int delay = (FindVal("DELAY", slowa, zmienna) ? zmienna : stale.DELAY);
	int rock_1 = (FindVal("ROCK_1", slowa, zmienna) ? zmienna : stale.ROCK_1);
	int plan_1 = (FindVal("PLAN_1", slowa, zmienna) ? zmienna : stale.PLAN_1);
	int herb_1 = (FindVal("HERB_1", slowa, zmienna) ? zmienna : stale.HERB_1);
	int pred_1 = (FindVal("PRED_1", slowa, zmienna) ? zmienna : stale.PRED_1);
	int plan_2 = (FindVal("PLAN_2", slowa, zmienna) ? zmienna : stale.PLAN_2);
	int herb_3 = (FindVal("HERB_3", slowa, zmienna) ? zmienna : stale.HERB_3);
	int pred_3 = (FindVal("PRED_3", slowa, zmienna) ? zmienna : stale.PRED_3);
	int plan_4 = (FindVal("PLAN_4", slowa, zmienna) ? zmienna : stale.PLAN_4);
	int herb_4 = (FindVal("HERB_4", slowa, zmienna) ? zmienna : stale.HERB_4);
	int pred_4 = (FindVal("PRED_4", slowa, zmienna) ? zmienna : stale.PRED_4);
	int plan_5 = (FindVal("PLAN_5", slowa, zmienna) ? zmienna : stale.PLAN_5);
	int herb_5 = (FindVal("HERB_5", slowa, zmienna) ? zmienna : stale.HERB_5);
	int pred_5 = (FindVal("PRED_5", slowa, zmienna) ? zmienna : stale.PRED_5);
	int plan_6 = (FindVal("PLAN_6", slowa, zmienna) ? zmienna : stale.PLAN_6);
	int herb_6 = (FindVal("HERB_6", slowa, zmienna) ? zmienna : stale.HERB_6);
	int pred_6 = (FindVal("PRED_6", slowa, zmienna) ? zmienna : stale.PRED_6);
	int write = (FindVal("WRITE", slowa, zmienna) ? zmienna : stale.WRITE);
	//mamy ju¿ wartoœci z pliku albo domyœlne, tworzymy w³aœciwy obiekt sta³ych
	pCONSTS = new CONSTS_T(map_y, map_x, delay, rock_1, plan_1, herb_1, pred_1, plan_2, herb_3, pred_3, plan_4, herb_4, pred_4, plan_5, herb_5, pred_5, plan_6, herb_6, pred_6,write);
	return true;
}
bool CProgram::FindVal(string etykieta, vector<string>& slowa, int& val)
{
	for (int i = 0; i < slowa.size(); ++i)
	{
		if ((slowa[i] == etykieta) && (i < slowa.size() - 1))
		{
			try 
			{ 
				val = stoi(slowa[i + 1]);//funkcja ryzykowna, mo¿liwoœæ nie przekonwertowania na int
				return true;
			}
			catch (invalid_argument) 
			{ 
				cout << "Error of reading constant " << etykieta << " default value will be used" <<endl; 
				return false;
			}
		}
	}
	cout << "Constant " << etykieta << "could not be found in file, default value will be used" << endl;
	return false;
}
CProgram::~CProgram()
{
	if (pMapa) { delete pMapa; }//najpierw niszczy obiekt wewnetrzny CMapa
	delete pMagazyn;//niszczy obiekt wewnêtrzny CMagazyn
	pMagazyn = NULL;
	pProgram = NULL;
}
CMapa::CMapa(int sizeY,int sizeX):iRozmY(sizeY),iRozmX(sizeX)
{
	ppTablica = new POLE*[iRozmY];//utworzenie pierwszego wymiaru tablicy
	for (int i = 0; i <= iRozmY - 1; ++i) { ppTablica[i] = new POLE[iRozmX]; }//utworzenie drugiego wymiaru tablicy
	for (int i = 0; i <iRozmY; ++i)
	{
		for (int j = 0; j < iRozmX; ++j)
		{
			ppTablica[i][j] = NIC;//zape³nienie tablicy
		}
	}
}

CMapa::~CMapa()
{
	for (int i = 0; i < iRozmY; ++i) { delete[] ppTablica[i]; }
	delete[] ppTablica;
	ppTablica = NULL;
}
void CMapa::Pokaz()
{
	system("cls");
	//for (int j = 0; j < iRozmX+2; ++j) { cout << '-'; }//górna linia ograniczaj¹ca mapê - zwyk³a
	cout << '-'; for (int j = 1; j < iRozmX + 2; ++j) { cout << ((j - 1) % 10); }//górna linia ograniczaj¹ca mapê - do debuggowania
	cout<<'\n';
	for (int i = 0; i < iRozmY; ++i)
	{
		//std::cout << '|';//lewa linia ograniczaj¹ca-zwyk³a
		std::cout << (i%10);//lewa linia ograniczaj¹ca-do debuggowania
		for (int j = 0; j < iRozmX; ++j)
		{
			cout<<static_cast<char>(ppTablica[i][j]);//wype³nienie pola zgodnia z zawartoœci¹ aTablica[][]
			if (ppTablica[i][j] == ZWLOKI)ppTablica[i][j] = NIC;//zwloki polezaly, mo¿na je usun¹æ
		}
		std::cout << '|' << '\n';//prawa linia ograniczaj¹ca
	}
	for (int j = 0; j < iRozmX+2; ++j) { cout << '-'; }//dolna linia ograniczaj¹ca
	cout<<endl;

	//rozmiary typów
	//cout << "rozmiar typu COrganizm: " << sizeof(COrganizm) << endl;
	//cout << "rozmiar typu CRoslina: " << sizeof(CRoslina) << endl;
	cout << "ESC - exit\n";
}
int CMapa::RozmiarX()
{
	return iRozmX;
}
int CMapa::RozmiarY()
{
	return iRozmY;
}
bool CMapa::Ustaw(POLE typ,int destX, int destY)
{
	if ((destX < 0) || (destX >= iRozmX)) { return false; }
	if ((destY < 0) || (destY >= iRozmY)) { return false; }
	ppTablica[destY][destX] = typ;
	return true;
}
POLE CMapa::Element(int indX, int indY)
{
	if ((indX<0) || (indX > iRozmX - 1)) { return ERR; }
	if ((indY<0) || (indY > iRozmY - 1)) { return ERR; }
	return ppTablica[indY][indX];
}
void COrganizm::Ruch()
{
	int NowyX;
	int NowyY;
	do
	{	
		NowyX = iPozX;
		NowyY = iPozY;
		if ((iPozX == 0) || (iPozX == pMapa->RozmiarX() - 1) || (iPozY == 0) || (iPozY == pMapa->RozmiarY() - 1))//jesli organizm znajduje sie na któryms z brzegów mapy
		{
			UstawDir(uKierunek + 4);//odwracamy kierunek o 180 stopni
		}
		//teraz zmiana kierunku o 0 lub 1 lub -1 (losowanie)
		int DeltaDir=((rand()%3)-1);
		UstawDir(uKierunek + DeltaDir);
		//okreslenie nowych wspó³rzêdnych na podstawie kierunku
		if (uKierunek==7||uKierunek==0||uKierunek==1)//jesli uKierunek wskazuje do góry
			{--NowyY;}

		if (uKierunek >= 3 && uKierunek <= 5)//jesli uKierunek wskazuje do do³u
			{++NowyY;}

		if (uKierunek >= 1 && uKierunek <= 3)//jesli uKierunek wskazuje w prawo
			{++NowyX;}

		if (uKierunek >= 5 && uKierunek <= 7)//jesli uKierunek wskazuje w lewo
			{--NowyX;}
		if (pMapa->Element(NowyX, NowyY) != NIC)//jesli nowe pole jest zajête, zatrzymaj sie
		{
			NowyX = iPozX;
			NowyY = iPozY;
		}
	} while (!((NowyX>=0)&&(NowyX<=pMapa->RozmiarX()-1))|| !((NowyY >= 0) && (NowyY <= pMapa->RozmiarY() - 1)));//nowe po³o¿enie musi byæ w obrêbie mapy
	pMapa->Ustaw(NIC, iPozX, iPozY);//ustaw puste pole w dotychczasowym miejscu organizmu
	iPozX = NowyX;//ustawienie nowych wspó³rzêdnych dla obiektu
	iPozY = NowyY;//----------//------------
	pMapa->Ustaw(ident, iPozX, iPozY);//ustawienie symbolu organizmu w nowym miejscu
}
bool COrganizm::Szukaj(POLE typ, int & ZnalX, int & ZnalY)
{
	//int Przeszukanych=0;
	//ustalenie zakresów szukania - zeby nie wyjechaæ poza mapê
	int Xmin = iPozX - 1;
	Xmin = (Xmin < 0 ? iPozX : Xmin);

	int Xmax = iPozX + 1;
	Xmax = (Xmax > pMapa->RozmiarX() - 1 ? iPozX : Xmax);

	int Ymin = iPozY - 1;
	Ymin = (Ymin < 0 ? iPozY : Ymin);

	int Ymax = iPozY + 1;
	Ymax = (Ymax > pMapa->RozmiarY() - 1 ? iPozY : Ymax);

	int signX = rand() % 2;//losowanie, mo¿e wypaœæ 0 lub 1
	int signY = rand() % 2;//losowanie, mo¿e wypaœæ 0 lub 1
	if (signY == 0)//przeszukujemy y od wiêkszego do mniejszego
	{
		for (int i = Ymax; i >= Ymin; --i)
		{
			if (signX == 0)//przeszukujemy x od wiêkszego do mniejszego
			{
				for (int j = Xmax; j >= Xmin; --j)
				{
					if (pMapa->Element(j, i) == typ)//znaleŸliœmy to, czego szukaliœmy
					{
						ZnalX = j;
						ZnalY = i;
						return true;
					}
				}
			}
			else//przeszukujemy x normalnie, od mniejszego do wiêkszego
			{
				for (int j = Xmin; j <= Xmax; ++j)
				{
					if (pMapa->Element(j, i) == typ)//znaleŸliœmy to, czego szukaliœmy
					{
						ZnalX = j;
						ZnalY = i;
						return true;
					}
				}
			}
		}
	}
	else//przeszukujemy y normalnie - od mniejszego do wiekszego
	{
		for (int i = Ymin; i <= Ymax; ++i)
		{
			if (signX == 0)//przeszukujemy x od wiêkszego do mniejszego
			{
				for (int j = Xmax; j >= Xmin; --j)
				{
					if (pMapa->Element(j, i) == typ)//znaleŸliœmy to, czego szukaliœmy
					{
						ZnalX = j;
						ZnalY = i;
						return true;
					}
				}
			}
			else//przeszukujemy x normalnie od mniejszego do wiêkszego
			{
				for (int j = Xmin; j <= Xmax; ++j)
				{
					if (pMapa->Element(j, i) == typ)//znaleŸliœmy to, czego szukaliœmy
					{
						ZnalX = j;
						ZnalY = i;
						return true;
					}
				}
			}
		}
	}
	return false;
}
void COrganizm::UstawDir(unsigned dir)
{

	uKierunek = (dir % 8);//niech uKierunek to bedzie liczba od 0 do 7, jezeli  w parametrze jest za du¿a
}
void COrganizm::UstawXY(int destX, int destY)
{
	iPozX = destX;
	iPozY = destY;
}
//EFEKT COrganizm::Akcja() metoda czysto wirtualna - nie musi mieæ implementacji
//{
//	/*EFEKT stEfekt;
//	return stEfekt;*/
//}
EFEKT CSkala::Akcja()
{
	EFEKT stEfekt;
	stEfekt.enRezultat = rNIC;
	stEfekt.X = 0;
	stEfekt.Y = 0;
	
	if (iDorozm > 0)//zmniejsza sie czas do kolejnego owocowania
	{
		--iDorozm;
	}
	if (uKondycja<100)
	{
		uKondycja += pCONSTS->ROCK_1;
	}
	else//rozmnazamy sie
	{
		if (Szukaj(NIC, stEfekt.X, stEfekt.Y)&&(iDorozm<=0))//jesli w poblizu jest wolne miejsce i up³yn¹³ czas miêdzy owocowaniem
		{
			//CSkala a = *this;
			pMapa->Ustaw(ROSL, stEfekt.X, stEfekt.Y);//nowa roslina zajmuje miejsce na mapie
			//uKondycja -= ROSLROZMNSPADEKKOND;//roslina-rodzic traci kondycje
			iDorozm = pCONSTS->PLAN_6;//ustawiamy czas do kolejnego owocowania na zadan¹ wartoœæ
			stEfekt.enRezultat = rNOWY;
		}
	}
	return stEfekt;
}
EFEKT CRoslina::Akcja()
{
	EFEKT stEfekt;
	stEfekt.enRezultat = rNIC;
	stEfekt.X = 0;
	stEfekt.Y = 0;
	
	if (iDorozm > 0)//zmniejsza sie czas do kolejnego owocowania
	{
		--iDorozm;
	}
	if (uKondycja<100)
	{
		uKondycja +=pCONSTS->PLAN_1;
	}
	else//rozmnazamy sie o ile losowanie wypadnie pomyœlnie
	{
		int losowa = (rand() % pCONSTS->PLAN_2 + 1);
		if ((pMagazyn->iIloscSkal + pMagazyn->iIloscRoslin) > 240)
		{
			//int a=5;
			//int b;
			//int c=a+b;
		}
		if ( losowa <= (100 * static_cast<double>(pMagazyn->iIloscSkal + pMagazyn->iIloscRoslin) / (pCONSTS->MAP_X *pCONSTS->MAP_Y)))//warunek losowy
		{
			return stEfekt;//losowanie nieudane - wygra³y paso¿yty i choroby roœlin, wyjœcie
		}
			if (Szukaj(NIC, stEfekt.X, stEfekt.Y) && (iDorozm <= 0))//jesli w poblizu jest wolne miejsce i up³yn¹³ czas miêdzy owocowaniem
			{
				pMapa->Ustaw(ROSL, stEfekt.X, stEfekt.Y);//nowa roslina zajmuje miejsce na mapie
				//uKondycja -= ROSLROZMNSPADEKKOND;//roslina-rodzic traci kondycje
				iDorozm =pCONSTS->PLAN_6;//ustawiamy czas do kolejnego owocowania na zadan¹ wartoœæ
				stEfekt.enRezultat = rNOWY;
			}
	
	}
	return stEfekt;
}
EFEKT CRzerca::Akcja()
{
	EFEKT stEfekt;
	stEfekt.enRezultat = rNIC;
	stEfekt.X = 0;
	stEfekt.Y = 0;
	pMapa->Ustaw(RZER, iPozX, iPozY);//jeœli wczeœniej by³ ukryty, to teraz jest widoczny
	switch (stan)
	{
	case sSTOP:
		stan = sNORMAL;//jak nic siê nie wydarzy, organizm mo¿e siê poruszaæ
		break;
	case sMLODE:
		stan = sSTOP;//m³ode nie rusza siê w pierwszej turze
	}
	if (iDorozm > 0)//zmniejsza sie czas do kolejnego porodu
	{
		--iDorozm;
	}
	if (uKondycja > 0)//strata sil zwiazana z czasem
	{
		uKondycja-=pCONSTS->HERB_1;
	}
	else//trzeba zdychac
	{
		stEfekt.enRezultat = rSMIERC;
		pMapa->Ustaw(ZWLOKI, iPozX, iPozY);
		return stEfekt;
	}
	int JedzenieX;
	int JedzenieY;
	if ((uKondycja >= 100))//organizm jest najedzony
	{
		stan = sSTOP;//najedzony albo rozmna¿a siê wiêc zostanie w miejscu
		pMapa->Ustaw(UKRYTY, iPozX, iPozY);//nie musi siê ruszaæ wiêc siê ukrywa
		if ((Szukaj(NIC, stEfekt.X, stEfekt.Y)) && (iDorozm <= 0))//rozmazanie jest najwazniejsze - jesli jest miejsce i up³yn¹³ odpowiedni czas miêdzy porodami
		{
			pMapa->Ustaw(RZER, stEfekt.X, stEfekt.Y);//nowy roslinozerca zajmuje miejsce na mapie
			//uKondycja -= RZERROZMNSPADEKKOND;//roslinozerca-rodzic traci kondycje
			iDorozm = pCONSTS->HERB_6;//ustawiamy czas do kolejnego porodu na zadan¹ wartoœæ
			stan = sSTOP;//roslinozerca-rodzic-nie rusza sie przez jedna ture
			stEfekt.enRezultat = rNOWY;
		}
	}
	else//trzeba poszukaæ czegoœ do jedzenia
	{
		if (Szukaj(ROSL, stEfekt.X, stEfekt.Y))//jeœli w pobli¿u jest roslina
		{
			pMapa->Ustaw(NIC, stEfekt.X, stEfekt.Y);//roslina znika
			stan = sSTOP;//nie odchodzimy od sto³u podczas jedzenia
			stEfekt.enRezultat = rZJEDZENIE;
			uKondycja = (uKondycja < 100 ? uKondycja + pCONSTS->HERB_3 : uKondycja);//jesli kondycja jest<100 to jej przybywa
		}
	}
	return stEfekt;
}
EFEKT CMzerca::Akcja()
{
	EFEKT stEfekt;
	stEfekt.enRezultat = rNIC;
	stEfekt.X = 0;
	stEfekt.Y = 0;
	switch (stan)
	{
	case sSTOP:
		stan = sNORMAL;//jak nic siê nie wydarzy, organizm mo¿e siê poruszaæ
		break;
	case sMLODE:
		stan = sSTOP;//m³ode nie rusza siê w pierwszej turze
	}
	if (iDorozm > 0)//zmniejsza sie czas do kolejnego porodu
	{
		--iDorozm;
	}
	if (uKondycja > 0)//strata sil zwiazana z czasem
	{
		uKondycja-=pCONSTS->PRED_1;
	}
	else//trzeba zdychac
	{
		stEfekt.enRezultat = rSMIERC;
		pMapa->Ustaw(ZWLOKI, iPozX, iPozY);
		return stEfekt;
	}
	int JedzenieX;
	int JedzenieY;
	if ((uKondycja >= 100))//organizm jest najedzony
	{
		if ((Szukaj(NIC, stEfekt.X, stEfekt.Y)) && (iDorozm <= 0))//rozmazanie jest najwazniejsze - jesli jest miejsce i up³yn¹³ odpowiedni czas miêdzy narodzinami
		{
			pMapa->Ustaw(MZER, stEfekt.X, stEfekt.Y);//nowy drapieznik zajmuje miejsce na mapie
			//uKondycja -= RZERROZMNSPADEKKOND;//drapieznik-rodzic traci kondycje
			iDorozm =pCONSTS->PRED_6;//ustawiamy czas do kolejnego porodu na zadan¹ wartoœæ
			stan = sSTOP;//miesozerca-rodzic-nie rusza sie przez jedna ture
			stEfekt.enRezultat = rNOWY;
		}
	}
	else//trzeba poszukaæ czegoœ do jedzenia
	{
		if (Szukaj(RZER, stEfekt.X, stEfekt.Y))//jeœli w pobli¿u jest roslinozerca
		{
			pMapa->Ustaw(ZWLOKI, stEfekt.X, stEfekt.Y);
			stan = sSTOP;//nie odchodzimy od sto³u podczas jedzenia
			stEfekt.enRezultat = rZJEDZENIE;
			uKondycja = (uKondycja < 100 ? uKondycja + pCONSTS->PRED_3 : uKondycja);
		}
	}
	return stEfekt;
}
CMagazyn::CMagazyn()
{
	pTablica = nullptr;
	uIlosc = 0;
}
CMagazyn::~CMagazyn()
{
	if (pTablica) 
	{
		for (unsigned i = 0; i < uIlosc; ++i)
		{
			delete pTablica[i];
		}
		delete[] pTablica;
		pTablica = NULL;
	}
}
void CMagazyn::Dodaj(POLE typ, unsigned destX, unsigned destY)
{
	if (uIlosc == 0)//nie ma jeszcze zadnego organizmu -nie trzeba kopiowac
	{
		pTablica = new COrganizm*[1];
		switch (typ)
		{
		case SKALA:
			pTablica[0] = new CSkala(SKALA, pCONSTS->PLAN_4);
			break;
		case ROSL:
			pTablica[0] = new CRoslina(ROSL,pCONSTS->PLAN_4);
			break;
		case RZER:
			pTablica[0] = new CRzerca(RZER,pCONSTS->HERB_4);
			break;
		case MZER:
			pTablica[0] = new CMzerca(MZER,pCONSTS->PRED_4);
		}
	}
	else//trzeba kopiowac stara tablice - przypadek ogólny
	{
		COrganizm** nowapTablica=new COrganizm*[uIlosc+1];
		memcpy(nowapTablica, pTablica, uIlosc * sizeof(COrganizm*));
		delete[]pTablica;
		pTablica = nowapTablica;
		//teraz nowy organizm
		switch (typ)
		{
		case SKALA:
			pTablica[uIlosc] = new CSkala(SKALA, pCONSTS->PLAN_4);
			break;
		case ROSL:
			pTablica[uIlosc] = new CRoslina(ROSL, pCONSTS->PLAN_4);
			break;
		case RZER:
			pTablica[uIlosc] = new CRzerca(RZER, pCONSTS->HERB_4);
			break;
		case MZER:
			pTablica[uIlosc] = new CMzerca(MZER, pCONSTS->PRED_4);
		}
	}
	++uIlosc;
	pTablica[uIlosc - 1]->UstawXY(destX, destY);
	pTablica[uIlosc - 1]->UstawDir(rand() % 8);
}
void CMagazyn::Usun(unsigned index)
{
	if ((uIlosc <= 0)||(index > uIlosc-1)) { return; }//ignoruj, jesli nie ma organizmow albo index przekracza ich ilosc
	delete pTablica[index];//usuniêcie organizmu
	if (uIlosc == 1)
	{
		delete[] pTablica;//usuniêcie tablicy adresów
		pTablica = NULL;
	}
	else
	{
		COrganizm** nowapTablica = new COrganizm*[uIlosc - 1];
		if (index >= 1)//najpierw kopiowanie wczesniejszej niz index czesci tablicy
		{
			memcpy(nowapTablica, pTablica, sizeof(COrganizm*)*(index));
		}
		if (uIlosc - 1 > index)//pozniej kopiowanie dalszej niz index czesci tablicy
		{
			memcpy(nowapTablica + index, pTablica + index + 1, sizeof(COrganizm*)*(uIlosc - 1 - index));
			delete[] pTablica;//usuniêcie starej tablicy adresów
			pTablica = nowapTablica;
		}
	}
	--uIlosc;
}
unsigned CMagazyn::JakiRozmiar()
{
	return uIlosc;
}
COrganizm* CMagazyn::Element(unsigned index)
{
	if ((index >= 0) && (index < uIlosc))
		{ return pTablica[index]; }
	else
		{ return NULL; }
}

void CMagazyn::Cykl()
{
	for (int i = 0; i <= (int)(uIlosc - 1); ++i)//---------------------------PETLA AKCJI, trzeba rzutowaæ na int - mo¿e sie tu pojawic liczba ujemna
	{
		EFEKT stWynik= pTablica[i]->Akcja();//organizm wykonuje akcjê- metoda wirtualna
		switch (stWynik.enRezultat)
		{
		case rSMIERC:
			Usun(i);

			--i;//zeby nie przeskoczyc nastepnego organizmu w nastepnym cyklu pêtli i
			break;
		case rZJEDZENIE:
			//trzeba wyszukac organizm o danych wspó³rzêdnych
			for (unsigned j = 0; j <= uIlosc-1;++j)
			{
				if (((pTablica[j]->X() == stWynik.X) && (pTablica[j]->Y() == stWynik.Y))&&(i!=j))//trzeba tez zadbac aby pêtla nie usunê³a organizmu zjadaj¹cego
				{
					Usun(j);//usuniêcie organizmu zjadanego
					if (j < i)
					{ --i; }//zeby nie przeskoczyc jednego organizmu w nastepnym cyklu pêtli i
					break;
				}
			}
			break;
		case rNOWY:
			if (pTablica[i]->Ident() == SKALA)
			{
				Dodaj(ROSL, stWynik.X, stWynik.Y);//wyj¹tek - ska³a nie mo¿e stworzyæ ska³y, tylko tworzy roœlinê
			}
			else
			{
				Dodaj(pTablica[i]->Ident(), stWynik.X, stWynik.Y);//przypadek ogólny
			}
		}
		//cout << pTablica[i]->Stan() << '_'<<endl;
	}
	for (int i = 0; i < uIlosc; ++i)//---------------------------------PETLA RUCHU-------------------------------------------
	{
		if (pTablica[i]->Stan() == sNORMAL)//wtedy mo¿e siê poruszaæ
		{
			pTablica[i]->Ruch();
		}
	}
	/*for (int i = 0; i < uIlosc; ++i)
	{
		switch (pTablica[i]->Ident())
		{
		case ROSL:
			++uIloscRoslin;
			break;
		case RZER:
			++uIloscRzercow;
			break;
		case MZER:
			++uIloscMzercow;
		}
	}*/
}
