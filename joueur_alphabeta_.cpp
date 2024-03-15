#include "joueur_alphabeta_.h"

u64 EtatJeux::etatOccupation() const noexcept
{
	return joueurs[0] | joueurs[1];
}

bool EtatJeux::coupLicite(const Coup coup) const noexcept
{
	return !(etatOccupation() & maskCoup[coup]);
}

// a ameliorer, voire faire directement dans le alpha beta pour pas passer par un vecteur inutile
// Pas ouf la table de lookup remplacer par constantes
u8 EtatJeux::coupsPossiblesIf(Coup coupsPossibles[nCoups]) const noexcept
{
	u8 len = 0;

	static Coup toutPossible[nCoups] = { Coup1, Coup2, Coup3, Coup4, Coup5, Coup6, Coup7 };

	u8 premiereLigne = (etatOccupation() & 0x7f);
	if (premiereLigne == 0) {
		coupsPossibles = toutPossible;
		return nCoups;
	}

	premiereLigne = ~premiereLigne;
	if (premiereLigne & maskCoup[Coup1]) coupsPossibles[len++] = Coup1;
	if (premiereLigne & maskCoup[Coup2]) coupsPossibles[len++] = Coup2;
	if (premiereLigne & maskCoup[Coup3]) coupsPossibles[len++] = Coup3;
	if (premiereLigne & maskCoup[Coup4]) coupsPossibles[len++] = Coup4;
	if (premiereLigne & maskCoup[Coup5]) coupsPossibles[len++] = Coup5;
	if (premiereLigne & maskCoup[Coup6]) coupsPossibles[len++] = Coup6;
	if (premiereLigne & maskCoup[Coup7]) coupsPossibles[len++] = Coup7;

	return len;
}

// Devrait etre meilleurs sur les anciens cpus avec un moins bon predicteur de branche
u8 EtatJeux::coupsPossibles(Coup coupsPossibles[nCoups]) const noexcept
{
	u8 len = 0;
	u8 premiereLigne = ~(etatOccupation() & 0x7f);

	while (premiereLigne != 0) {
		const Coup coup = static_cast<Coup>(__builtin_ctz(premiereLigne));
		coupsPossibles[len++] = coup;
		premiereLigne &= ~(1UL << coup);
	}

	return len;
}

void EtatJeux::jouer(const NumeroJouer numeroJouer, const Coup coup) noexcept
{
	const u64 etat = etatOccupation() & maskColonne[coup];

	for (u8 ligne = nLignes - 1; ligne >= 0; --ligne) {
		if ((etat & maskLigne[ligne]) == 0) {
			joueurs[numeroJouer] |= 1UL << (coup + ligne * 7);
			break;
		}
	}

	joueurCourant = static_cast<NumeroJouer>(joueurCourant ^ 1);
}

void EtatJeux::afficherBits(const u64 val) const noexcept
{
	printf("%zu\t:\t", val);
	u64 mask = 0x0000'0200'0000'0000;

	for (u8 ligne = 0; ligne < nLignes; ++ligne) {
		for (u8 col = 0; col < nColonnes; ++col) {
				printf("%d", val & mask ? 1 : 0);
				mask >>= 1;
		}
		printf(" ");
	}

	printf("\n");
}

void EtatJeux::afficher() const noexcept
{
	for (u8 ligne = 0; ligne < nLignes; ++ligne) {
		printf("|");
		for (u8 col = 0; col < 7; ++col) {
			u64 mask = 0b0000001UL << (col + ligne * nColonnes);
			if (joueurs[0] & mask) {
				printf(" X ");
			} else if (joueurs[1] & mask) {
				printf(" O ");
			} else {
				printf("   ");
			}
		}
		printf("|\n");
	}
	printf("\n");
}

void EtatJeux::test() noexcept
{

	afficher();
	afficherBits(joueurs[0]);
	afficherBits(joueurs[1]);
	afficherBits(etatOccupation());

	for (u8 i = 0; i < nColonnes * nLignes; ++i) {
		Coup c;
		do {
			c = static_cast<Coup>(rand()%7);
		} while (!coupLicite(c));

		printf("-------------------------------------------------------\n\n");
		printf("Jouer %d joue en %zu\n\n", joueurCourant, c + 1);
		jouer(joueurCourant, c);
		afficher();
		afficherBits(joueurs[0]);
		afficherBits(joueurs[1]);
		afficherBits(etatOccupation());
	}

	printf("\n");
}

Joueur_AlphaBeta_::Joueur_AlphaBeta_(std::string nom, bool joueur)
    :Joueur(nom,joueur)
{}



char Joueur_AlphaBeta_::nom_abbrege() const
{
    return 'A';
}

void Joueur_AlphaBeta_::recherche_coup(Jeu jeu, int &coup)
{

}
