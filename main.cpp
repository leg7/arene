
#include <iostream>
#include <mutex>

#include "arbitre.h"
#include "objects/jeu.h"

using namespace std;

int main()
{
    //initialise la graine du générateur aléatoire
    std::srand(std::time(nullptr));
    //
    // // création de l'Arbitre (graine , joueur 1, joueur 2 , nombre de parties)
    // Arbitre a (9999, player::RAND, player::BRUTAL_,100);
    // // commence le challenge
    // a.challenge();

    Jeu j(9999);



    return 0;
}
