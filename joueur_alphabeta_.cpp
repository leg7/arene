#include "joueur_alphabeta_.h"

EtatJeux::EtatJeux():
	joueurCourant(j0)
{
	joueurs[0] = 0;
	joueurs[1] = 0;
}

EtatJeux::EtatJeux(const EtatJeux &j)
{
	joueurs[0] = j.joueurs[0];
	joueurs[1] = j.joueurs[1];
	joueurCourant = j.joueurCourant;
}

// EtatJeux::EtatJeux& operator=(const EtatJeux &j)
// {
// 	joueurs[0] = j.joueurs[0];
// 	joueurs[1] = j.joueurs[1];
// 	joueurCourant = j.joueurCourant;
// 	return *this;
// }

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
	} else if (premiereLigne == 127) {
		return 0;
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

void EtatJeux::jouer(const Coup coup) noexcept
{
	const u64 etat = etatOccupation() & maskColonne[coup];

	for (u8 ligne = nLignes - 1; ligne >= 0; --ligne) {
		if ((etat & maskLigne[ligne]) == 0) {
			joueurs[joueurCourant] |= 1UL << (coup + ligne * 7);
			break;
		}
	}

	joueurCourant = static_cast<NumeroJoueur>(joueurCourant ^ 1);
}

bool EtatJeux::estGagnant(NumeroJoueur num)const{
	u64 sauvegarde=joueurs[num]<<__builtin_clzl(joueurs[num]);
	u64 etat=joueurs[num]<<__builtin_clzl(joueurs[num]);
	for (int i=0;i<7;i++){
		if ((etat & 0xF0'00'00'00'00'00'00'00)==0xF0'00'00'00'00'00'00'00) return true;
		else if ((etat & 0x78'00'00'00'00'00'00'00)==0x78'00'00'00'00'00'00'00) return true;
		else if ((etat & 0x3C'00'00'00'00'00'00'00)==0x3C'00'00'00'00'00'00'00) return true;
		else if ((etat & 0x1E'00'00'00'00'00'00'00)==0x1E'00'00'00'00'00'00'00) return true;

		etat<<=7;
	}
		u64 masque1=2113665;
		u64 masque2=270549120;
		u64 masque3=34630287360;

		for (int i=0;i<6;i++){
			if ((sauvegarde&masque1)==masque1) return true;
			else if ((sauvegarde&masque2)==masque2) return true;
			else if ((sauvegarde&masque3)==masque3) return true;
			masque1<<=1;
			masque2<<=1;
			masque3<<=1;
		}

		return 0;
}



i32 EtatJeux::valeurCoup() const noexcept{
	if (estGagnant(joueurCourant)) return 10000;
	else if (estGagnant(static_cast<NumeroJoueur>(joueurCourant ^1))) return -10000;
	else{

		return 14;
	}

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
		jouer(c);
		afficher();
		afficherBits(joueurs[0]);
		afficherBits(joueurs[1]);
		afficherBits(etatOccupation());
	}

	printf("\n");
}

i32 EtatJeux::estimation() const noexcept
{

}


Joueur_AlphaBeta_::Joueur_AlphaBeta_(std::string nom, bool joueur)
    :Joueur(nom,joueur)
{}



char Joueur_AlphaBeta_::nom_abbrege() const
{
    return 'A';
}

void Joueur_AlphaBeta_::recherche_coup(Jeu j, int &coup)
{
	u8 profondeur = 4;
	i32 meilleur_score = INT32_MIN;
	i32 score;
	int meilleur_coup;
	EtatJeux etat_jeux;
	EtatJeux::Coup listCoupsPossibles[EtatJeux::nCoups];
	u8 taille = etat_jeux.coupsPossibles(listCoupsPossibles);

	for (u8 i = 0; i < taille; ++i) {
		EtatJeux tmp(etat_jeux);
		tmp.jouer(listCoupsPossibles[i]);
		score = alpha_beta(profondeur, tmp, INT32_MIN, INT32_MAX, true);
		if (meilleur_score < score) {
			meilleur_score = score;
			meilleur_coup = listCoupsPossibles[i];
		}
	}
	coup = meilleur_coup;
}

i32 Joueur_AlphaBeta_::alpha_beta(const u8 profondeur, const EtatJeux &etat_jeux, i32 alpha, i32 beta, const bool isMax) {
	//TODO : faire l'estimateur pour l'etat du jeu
	if (profondeur <= 0) return etat_jeux.estimation();

 	i32 score;

	EtatJeux::Coup listCoupsPossibles[EtatJeux::nColonnes];
	u8 taille = etat_jeux.coupsPossibles(listCoupsPossibles);

	// TODO : faire la fonction de test si le jeu est fini.
	bool jeuxTermine = taille == 0;
	if (jeuxTermine) {
		return etat_jeux.estimation();
	}

	if (isMax) {
		score = INT32_MIN;

		for (u8 i = 0; i < taille; ++i) {
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[i]);
			score = std::max(score, alpha_beta(profondeur-1, tmp, alpha, beta, false));

			// Beta coupure
			if (score >= beta) {
				break;
			}

			if (score > alpha) {
				alpha = score;
			}
		}
	} else {
		score = INT32_MAX;

		for (u8 i = 0; i < taille; ++i) {
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[i]);
			score = std::min(score, alpha_beta(profondeur-1, etat_jeux, alpha, beta, true));

			// Alpha coupure
			if (score <= alpha) {
				break;
			}

			if (score < beta) {
				beta = score;
			}
		}
	}

	return score;
}
