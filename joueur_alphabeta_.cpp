#include "joueur_alphabeta_.h"

void afficher64Bits(const u64 val)
{
	printf("%zu\t:\t", val);
	u64 mask = 0x8000'0000'0000'0000;

	for (u8 i = 0; i < 16; ++i) {
		for (u8 j = 0; j < 4; ++j) {
			printf("%d", val & mask ? 1 : 0);
			mask >>= 1;
		}
		printf(" ");
	}

	printf("\n");
}

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
u8 EtatJeux::coupsPossibles(Coup coupsPossibles[nCoups]) const noexcept
{
	u8 len = 0;

	// u8 premiereLigne = ~(etatOccupation() & 0x7f);

	// if (premiereLigne == 127) {
	// 	coupsPossibles = { Coup1, Coup2, Coup3, Coup4, Coup5, Coup6, Coup7 };
	// 	return nCoups;
	// } else if (premiereLigne == 0) {
	// 	return 0;
	// }

	u8 premiereLigne = ~etatOccupation();
	if ((premiereLigne & maskCoup[Coup1]) == maskCoup[Coup1]) coupsPossibles[len++] = Coup1;
	if ((premiereLigne & maskCoup[Coup2]) == maskCoup[Coup2]) coupsPossibles[len++] = Coup2;
	if ((premiereLigne & maskCoup[Coup3]) == maskCoup[Coup3]) coupsPossibles[len++] = Coup3;
	if ((premiereLigne & maskCoup[Coup4]) == maskCoup[Coup4]) coupsPossibles[len++] = Coup4;
	if ((premiereLigne & maskCoup[Coup5]) == maskCoup[Coup5]) coupsPossibles[len++] = Coup5;
	if ((premiereLigne & maskCoup[Coup6]) == maskCoup[Coup6]) coupsPossibles[len++] = Coup6;
	if ((premiereLigne & maskCoup[Coup7]) == maskCoup[Coup7]) coupsPossibles[len++] = Coup7;

	return len;
}

// Devrait etre meilleurs sur les anciens cpus avec un moins bon predicteur de branche
// u8 EtatJeux::coupsPossibles(Coup coupsPossibles[nCoups]) const noexcept
// {
// 	u8 len = 0;
// 	u8 premiereLigne = ~(etatOccupation() & 0x7f);
// 	afficherBits(premiereLigne);
//
// 	while (premiereLigne != 0) {
// 		const Coup coup = static_cast<Coup>(__builtin_ctz(premiereLigne));
// 		printf("tz: %zu", coup);
// 		coupsPossibles[len++] = coup;
// 		premiereLigne &= ~maskCoup[coup];
// 	}
//
// 	return len;
// }

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

void EtatJeux::jouerDebug(const Coup coup) noexcept
{
	printf("-------------------------------------------------------\n\n");
	jouer(coup);
	afficher();
	std::cout << "Estimation\t:\t" << estimation(j0) << std::endl;
	std::cout << "Fourchette de j0\t:\t" << std::boolalpha << fourchettePour(j0) << std::endl;
	std::cout << "Fourchette de j1\t:\t" << std::boolalpha << fourchettePour(j1) << std::endl;
}

bool EtatJeux::estGagnant(const NumeroJoueur num) const noexcept
{
	u64 etat = joueurs[num] << 22;

	for (u64 i = 0; i < nCoups; i++) {
 		if ((etat & 0xF0'00'00'00'00'00'00'00) == 0xF0'00'00'00'00'00'00'00) {
			return true;
		} else if ((etat & 0x78'00'00'00'00'00'00'00) == 0x78'00'00'00'00'00'00'00) {
			return true;
		} else if ((etat & 0x3C'00'00'00'00'00'00'00) == 0x3C'00'00'00'00'00'00'00) {
			return true;
		} else if ((etat & 0x1E'00'00'00'00'00'00'00) == 0x1E'00'00'00'00'00'00'00) {
			return true;
		}

		etat <<= nCoups;
	}

	u64 masque1 = 2113665;
	u64 masque2 = 270549120;
	u64 masque3 = 34630287360;

	for (u64 i = 0; i < nColonnes; i++) {
		if ((joueurs[num] & masque1) == masque1) return true;
		else if ((joueurs[num] & masque2) == masque2) return true;
		else if ((joueurs[num] & masque3) == masque3) return true;
		masque1 <<= 1;
		masque2 <<= 1;
		masque3 <<= 1;
	}

	return 0;
}

int EtatJeux::nbPiecesConsecutives(const NumeroJoueur num) const noexcept
{
	int nb_pieces_consecutives = 0;

	for (u64 ligne = 0; ligne < nLignes; ligne++){
		u64 masque = (maskLigne[ligne] & joueurs[num]) << 22;
		while (masque != 0) {
			masque <<= __builtin_clzl(masque);
			int pieces = __builtin_clrsbl(masque);
			nb_pieces_consecutives += pieces * 2;
			masque <<= pieces+1;
		}
	}

	for (u64 col = 0; col < nColonnes; col++){
		bool start_seq=false;
		u64 bits = joueurs[num];
		for (u64 i = 0; i < nLignes; i++) {
			if (start_seq == false && (bits & maskCoup[col]) >= 1) {
				start_seq = true;
			} else if (start_seq && (bits & maskCoup[col]) >= 1) {
				nb_pieces_consecutives++;
			} else /* if ((joueurs[num] & maskCoup[j]) == 0) */ {
				start_seq = false;
			}

			bits >>= nCoups;
		}
	}

	return nb_pieces_consecutives;
}

bool EtatJeux::fourchettePour(const NumeroJoueur joueurAEstimer) const noexcept
{
	const NumeroJoueur adversaire = static_cast<NumeroJoueur>(joueurAEstimer ^ 1);
	for (u8 ligne = 0; ligne < nLignes; ++ligne) {
		const u64 etat = joueurs[joueurAEstimer] >> ligne * nColonnes;
		const u64 etatAdv = joueurs[adversaire] >> ligne * nColonnes;

		const u8 mask1    = 0b0'0111000;
		const u8 maskAdv1 = 0b0'1000100;

		const u8 mask2    = 0b0'0011100;
		const u8 maskAdv2 = 0b0'0100010;

		const u8 mask3    = 0b0'0001110;
		const u8 maskAdv3 = 0b0'0010001;

		if ((etat & mask1) == mask1 && (etatAdv & maskAdv1) == 0 ||
		    (etat & mask2) == mask2 && (etatAdv & maskAdv2) == 0 ||
		    (etat & mask3) == mask3 && (etatAdv & maskAdv3) == 0) {
			return true;
		}
	}

	return false;
}

i32 EtatJeux::estimation(const NumeroJoueur joueurAEstimer) const noexcept
{
	const NumeroJoueur adversaire = static_cast<NumeroJoueur>(joueurAEstimer ^ 1);
	if (fourchettePour(joueurAEstimer) || estGagnant(joueurAEstimer)) {
		return INT32_MAX;
	} else if (fourchettePour(adversaire) || estGagnant(adversaire)) {
		return INT32_MIN;
	} else {
		int nb0 = nbPiecesConsecutives(joueurAEstimer);
		int nb1 = nbPiecesConsecutives(adversaire);
		return 3 * __builtin_popcountl(joueurs[joueurAEstimer] & maskColonne[Coup4]) + (nb0 - nb1);
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

void EtatJeux::afficherCoupsPossibles() const noexcept
{
		Coup cp[nCoups];
		int cpLen = coupsPossibles(cp);
		printf("Coups Possibles\t:\t");
		for (u8 i = 0; i < cpLen; ++i) {
			printf("%zu ", cp[i] + 1);
		}
		printf("\n");
}

void EtatJeux::afficher() const noexcept
{
	for (u8 ligne = 0; ligne < nLignes; ++ligne) {
		printf("|");
		for (u8 col = 0; col < 7; ++col) {
			u64 mask = 0b0000001UL << (col + ligne * nColonnes);
			if (joueurs[j0] & mask) {
				printf(" 0 |");
			} else if (joueurs[j1] & mask) {
				printf(" 1 |");
			} else {
				printf("   |");
			}
		}
		printf("\n");
	}
	for (u8 i = 0; i < nColonnes; ++i) {
		printf("----");
	}
	printf("-\n|");
	for (u8 ligne = 0; ligne < nColonnes; ++ligne) {
		printf(" %d |", ligne);
	}
	printf("\n");
	// printf("\n\nJoueur gagnant\t:\t%s\n", estGagnant(j0) ? "j0" : "j1");
}

void EtatJeux::test() noexcept
{

	afficher();
	afficherBits(joueurs[0]);
	afficherBits(joueurs[1]);
	afficherBits(etatOccupation());

	bool fini = false;
	for (u8 i = 0; !fini && i < nColonnes * nLignes; ++i) {
		printf("-------------------------------------------------------\n\n");

		Coup c;
		do {
			c = static_cast<Coup>(rand()%7);
		} while (!coupLicite(c));
		printf("Jouer %d joue en %zu\n\n", joueurCourant, c + 1);
		jouer(c);

		afficher();
		printf("\n");
		afficherBits(joueurs[0]);
		afficherBits(joueurs[1]);
		afficherBits(etatOccupation());
		printf("Estimation\t:\t%d\n", estimation(j0));
		afficherCoupsPossibles();

		fini = estGagnant(j0) || estGagnant(j1);
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

void Joueur_AlphaBeta_::recherche_coup(Jeu j, int &coup)
{
	// Joue le coup de l'adversaire pour actualiser notre plateau

	std::cout << "coup adversaire (struct Prof): " << coup << std::endl;
	std::cout << "coup adversaire (struct bitset): " << abs(coup) - 1 << std::endl;
	_joueurAEstimer = coup == -1000 ? EtatJeux::j0 : EtatJeux::j1;

	if (coup != -1000) {
		_etat_jeux.jouer(static_cast<EtatJeux::Coup>((abs(coup) - 1)));
		coup = 3;
		return;
	}

	std::cout << "joueurs[0] " << _etat_jeux.joueurs[0] << std::endl;
	std::cout << "joueurs[1] " << _etat_jeux.joueurs[1] << std::endl;
	_etat_jeux.afficher();


	u8 profondeur = 4;
	i32 meilleur_score = INT32_MIN;
	i32 score;
	EtatJeux::Coup meilleur_coup;
	EtatJeux::Coup listCoupsPossibles[EtatJeux::nCoups];
	u8 taille = _etat_jeux.coupsPossibles(listCoupsPossibles);
	taille = taille / 2;

	// std::cout << "recherche::profondeur : " << unsigned(profondeur) << std::endl;
	// std::cout <<"recherche::taille : " <<  unsigned(taille) << std::endl;
	// std::cout << "recherce::listCoupsPossibles : " << std::endl;
	// for (u8 i = 0; i < taille; ++i) {
	// 	std::cout << listCoupsPossibles[i] << " ";
	// }
	// std::cout << std::endl;

	// for (u8 i = 0; i < taille; ++i) {
	for (u8 i = 0; i < taille )
		EtatJeux tmp(_etat_jeux);
		tmp.jouer(listCoupsPossibles[i]);
		score = alpha_beta(profondeur, tmp, INT32_MIN, INT32_MAX, true);
		// std::cout << "recherce::score : " << score << std::endl;
		if (meilleur_score < score) {
			meilleur_score = score;
			meilleur_coup = listCoupsPossibles[i];
		}
	}
	// Joue notre coup pour actualiser notre plateau avant de rendre le mutex
	_etat_jeux.jouer(meilleur_coup);
	// std::cout << "score : " << score << std::endl;
	std::cout << "Notre coup (Struct prof): " << static_cast<int>(meilleur_coup)+1 << std::endl;
	std::cout << "Notre coup (Struct bitset): " << static_cast<int>(meilleur_coup) << std::endl;
	_etat_jeux.afficher();
	coup = static_cast<int>(meilleur_coup);
}

i32 Joueur_AlphaBeta_::alpha_beta(const u8 profondeur, const EtatJeux &etat_jeux, i32 alpha, i32 beta, const bool isMax) {
	// std::cout << "alpha_beta : " << std::endl;
	// std::cout << " 		profondeur : " << unsigned(profondeur) << std::endl;
	// std::cout << " 		Player : " << (isMax? "Max" : "Min") << std::endl;
	// etat_jeux.afficher();

	//TODO : chronometre pour finir à 9,5 milisecondre

	//TODO : faire l'estimateur pour l'etat du jeu
	if (unsigned(profondeur) == 0) {
		// return etat_jeux.estimation();
		i32 val = etat_jeux.estimation(_joueurAEstimer);
		// std::cout << "profondeur break estimation : " << val << std::endl;
		return val;
	}

 	i32 score;

	EtatJeux::Coup listCoupsPossibles[EtatJeux::nColonnes];
	u8 taille = etat_jeux.coupsPossibles(listCoupsPossibles);
	// std::cout <<"taille : " <<  unsigned(taille) << std::endl;
	// std::cout << "ab::listCoupsPossibles : " << std::endl;
	// for (u8 i = 0; i < taille; ++i) {
	// 	std::cout << listCoupsPossibles[i] << " ";
	// }
	// std::cout << std::endl;


	// TODO : faire la fonction de test si le jeu est fini.
	bool jeuxTermine = taille == 0;
	if (jeuxTermine) {
		// std::cout << "termine break" << std::endl;
		return etat_jeux.estimation(_joueurAEstimer);
	}

	// std::cout << "alpha_beta test fin." << std::endl;


	if (isMax) {
		// std::cout << "ab::max" << std::endl;
		score = INT32_MIN;

		for (u8 i = 0; i < taille; ++i) {
			// std::cout << "ab::max::coup : " << unsigned(i) <<  std::endl;
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[i]);
			// std::cout << "tmp etat" << std::endl;
			// tmp.afficher();
			// std::cout << "--------" << std::endl;
			// std::cout << "===============ab::max avant rec=============" <<  std::endl;
			score = std::max(score, alpha_beta(profondeur-1, tmp, alpha, beta, false));
			// i32 val = alpha_beta(profondeur-1, tmp, alpha, beta, false);
			// score = std::max(score,val);
			// std::cout << "===============ab::max apres rec=============" <<  std::endl;
			// std::cout << "ab::max::score : " << score <<  std::endl;
			// std::cout << "ab::max::val : " << val <<  std::endl;
			// std::cout << "ab::max::score : " << score <<  std::endl;

			// Beta coupure
			if (score >= beta) {
				// std::cout << "Beta coupure" << std::endl;
				return score;
			}

			if (score > alpha) {
				alpha = score;
			}
		}
		// std::cout << "ab::max::fin" << std::endl;
	} else {
		// std::cout << "ab::min" << std::endl;
		score = INT32_MAX;

		for (u8 i = 0; i < taille; ++i) {
			// std::cout << "ab::min::coup : " << unsigned(i) <<  std::endl;
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[i]);
			// std::cout << "tmp etat" << std::endl;
			// tmp.afficher();
			// std::cout << "--------" << std::endl;
			score = std::min(score, alpha_beta(profondeur-1, tmp, alpha, beta, true));
			// i32 val = alpha_beta(profondeur-1, tmp, alpha, beta, true);
			// score = std::min(score,val);
			// std::cout << "ab::min::score : " << score <<  std::endl;
			// std::cout << "ab::min::val : " << val <<  std::endl;
			// std::cout << "ab::min::score : " << score <<  std::endl;

			// Alpha coupure
			if (score <= alpha) {
				// std::cout << "Alpha coupure" << std::endl;
				return score;
			}

			if (score < beta) {
				beta = score;
			}
		}
		// std::cout << "ab::min::fin" << std::endl;
	}

	// std::cout << "alpha_beta fin." << std::endl;
	// std::cout << "ab::score : " << score << std::endl;
	return score;
}
