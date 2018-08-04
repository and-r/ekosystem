#include "ekosystem.h"
using namespace std;
CProgram* CProgram::pProgram = nullptr;
CMapa* COrganizm::pMapa = nullptr;
CMagazyn* COrganizm::pMagazyn = nullptr;
CONSTS_T* COrganizm::pCONSTS = nullptr;
CONSTS_T* CMagazyn::pCONSTS = nullptr;
int main()
{
	CProgram::Obiekt()->Otworz();
	delete CProgram::Obiekt();
	return 0;
}
