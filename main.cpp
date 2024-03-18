
#include <iostream>
#include <mutex>

#include "arbitre.h"
#include "objects/jeu.h"

using namespace std;

int main()
{
    //initialise la graine du générateur aléatoire
    std::srand(std::time(nullptr));

    // création de l'Arbitre (graine , joueur 1, joueur 2 , nombre de parties)
    // Arbitre a (9999, player::RAND, player::BRUTAL_,100);
    // Arbitre a (9999, player::BRUTAL_, player::A_,1);
    Arbitre a (9999, player::A_, player::RAND, 1);
    // Arbitre a (9999, player::RAND, player::A_, 1);
    a.challenge();

    EtatJeux e;
    // e.test();
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup2);
    // e.jouerDebug(EtatJeux::Coup3);
    // e.jouerDebug(EtatJeux::Coup4);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup2);
    // e.jouerDebug(EtatJeux::Coup3);
    // e.jouerDebug(EtatJeux::Coup4);
    //
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup2);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup3);
    // e.jouerDebug(EtatJeux::Coup3);
    // e.jouerDebug(EtatJeux::Coup4);
    // e.jouerDebug(EtatJeux::Coup5);
    // e.jouerDebug(EtatJeux::Coup4);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup3);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup4);
    // e.jouerDebug(EtatJeux::Coup4);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup5);
    //
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup7);
    // e.jouerDebug(EtatJeux::Coup7);
    //
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup1);
    // e.jouerDebug(EtatJeux::Coup1);

    // Jeu j(9999);

    // srand(time(NULL));
    // while (!j.fini()) {
	   //  int coup = rand()%14 - 14/2;
	   //  j.joue(coup);
	   //  j.plateau()->afficher(std::cout);
    // }
    // for (i32 i = -7; i <= 7; ++i) {
	   //  std::cout << "Coup licite : " << i << " " << std::boolalpha << j.coup_licite(i) << std::endl;
	   //  j.joue(i);
	   //  j.plateau()->afficher(std::cout);
    // }

    return 0;
}
