
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
    Arbitre a (9999, player::RAND, player::A_,100);
    a.challenge();

    // EtatJeux e;
    // e.test();

    // Jeu j(9999);
    //
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
