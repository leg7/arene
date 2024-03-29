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

	const u8 premiereLigne = ~etatOccupation();
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
	std::cout << "Estimation\t:\t" << estimation(j1) << std::endl;
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
			const int pieces = __builtin_clrsbl(masque);
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

		if (((etat & mask1) == mask1 && (etatAdv & maskAdv1) == 0) ||
		    ((etat & mask2) == mask2 && (etatAdv & maskAdv2) == 0) ||
		    ((etat & mask3) == mask3 && (etatAdv & maskAdv3) == 0)) {
			return true;
		}
	}

	return false;
}

i32 EtatJeux::estimation(const NumeroJoueur joueurAEstimer) const noexcept
{
	const NumeroJoueur adversaire = static_cast<NumeroJoueur>(joueurAEstimer ^ 1);
	if (estGagnant(joueurAEstimer)) {
		return 100'000;
	} else if (estGagnant(adversaire)) {
		return -100'000;
	} else if (fourchettePour(joueurAEstimer)) {
		return 10'000;
	} else if (fourchettePour(adversaire)) {
		return -10'000;
	} else {
		const int nb0 = nbPiecesConsecutives(joueurAEstimer);
		const int nb1 = nbPiecesConsecutives(adversaire);
		return (3 * __builtin_popcountl(joueurs[joueurAEstimer] & maskColonne[Coup4])) + (nb0 - nb1);
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
	for (u8 col = 0; col < nColonnes; ++col) {
		printf(" %d |", col + 1);
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
	clock_t top_depart = clock();
	double temps_inter;

	if (coup == -1000 && _firstTimeAround) {
		_joueurAEstimer = EtatJeux::j0;
		_etat_jeux.jouer(EtatJeux::Coup4);
		coup = 3;
		_firstTimeAround = false;
		return;
	} else if (_firstTimeAround) {
		_joueurAEstimer = EtatJeux::j1;
		_firstTimeAround = false;
		_etat_jeux.jouer(static_cast<EtatJeux::Coup>((abs(coup) - 1)));
	} else {
		_etat_jeux.jouer(static_cast<EtatJeux::Coup>((abs(coup) - 1)));
	}


	// std::cout << "joueurs[0] " << _etat_jeux.joueurs[0] << std::endl;
	// std::cout << "joueurs[1] " << _etat_jeux.joueurs[1] << std::endl;
	// _etat_jeux.afficher();


	u8 profondeur = 8;
	i32 meilleur_score = INT32_MIN;
	i32 score;
	EtatJeux::Coup listCoupsPossibles[EtatJeux::nCoups];
	const u8 taille = _etat_jeux.coupsPossibles(listCoupsPossibles);
	EtatJeux::Coup meilleur_coup = listCoupsPossibles[0];

	// std::cout << "recherche::profondeur : " << unsigned(profondeur) << std::endl;
	// std::cout <<"recherche::taille : " <<  unsigned(taille) << std::endl;
	// std::cout << "recherce::listCoupsPossibles : " << std::endl;
	// for (u8 i = 0; i < taille; ++i) {
	// 	std::cout << listCoupsPossibles[i] << " ";
	// }
	// std::cout << std::endl;

	// for (u8 i = 0; i < taille; ++i) {
	// 	EtatJeux tmp(_etat_jeux);
	// 	tmp.jouer(listCoupsPossibles[i]);
	// 	score = alpha_beta(profondeur, tmp, INT32_MIN, INT32_MAX, false);
	// 	std::cout << "recherce::score : " << score << std::endl;
	// 	if (meilleur_score < score) {
	// 		meilleur_score = score;
	// 		meilleur_coup = listCoupsPossibles[i];
	// 	}
	// }

	i32 millieu = (int)taille / 2;
	i32 reste = (int)taille % 2;

	if (unsigned(taille) == 1) {
		coup = static_cast<int>(listCoupsPossibles[0]);
	}

	if (reste) {
		EtatJeux tmp(_etat_jeux);
		tmp.jouer(listCoupsPossibles[millieu]);
		// score = alpha_beta(profondeur, tmp, INT32_MIN, INT32_MAX, false);
		score = alpha_beta(top_depart, profondeur, tmp, INT32_MIN, INT32_MAX, false);
		// std::cout << " coup : " << listCoupsPossibles[millieu] << " score : " << score  << std::endl;
		if (meilleur_score < score) {
			meilleur_score = score;
			meilleur_coup = listCoupsPossibles[millieu];
		}

		for (int i = 1; i <= millieu; ++i) {
			temps_inter = (clock()-top_depart)/ (double)(CLOCKS_PER_SEC / 10000);
			// std::cout << "Chrono : " << temps_inter << " s" << std::endl;
		if (temps_inter >= 0.95) {
			coup = static_cast<int>(meilleur_coup);
			break;
		}

		  EtatJeux tmp2(_etat_jeux);
			tmp2.jouer(listCoupsPossibles[millieu + i]);
			// score = alpha_beta(profondeur, tmp2, INT32_MIN, INT32_MAX, false);
			score = alpha_beta(top_depart, profondeur, tmp2, INT32_MIN, INT32_MAX, false);

		// std::cout << " coup : " << listCoupsPossibles[millieu + i] << " score : " << score  << std::endl;
			if (meilleur_score < score) {
				meilleur_score = score;
				meilleur_coup = listCoupsPossibles[millieu + i];
			}

			temps_inter = (clock()-top_depart)/ (double)(CLOCKS_PER_SEC / 10000);
			// std::cout << "Chrono : " << temps_inter << " s" << std::endl;
		if (temps_inter >= 0.95) {
			coup = static_cast<int>(meilleur_coup);
			break;
		}

		  EtatJeux tmp3(_etat_jeux);
			tmp3.jouer(listCoupsPossibles[millieu - i]);
			// score = alpha_beta(profondeur, tmp3, INT32_MIN, INT32_MAX, false);
			score = alpha_beta(top_depart, profondeur, tmp3, INT32_MIN, INT32_MAX, false);

		// std::cout << " coup : " << listCoupsPossibles[millieu - i] << " score : " << score  << std::endl;
			if (meilleur_score < score) {
				meilleur_score = score;
				meilleur_coup = listCoupsPossibles[millieu - i];
			}
		}

		for (int i = millieu * 2; i <= reste; ++i) {
			temps_inter = (clock()-top_depart)/ (double)(CLOCKS_PER_SEC / 10000);
			// std::cout << "Chrono : " << temps_inter << " s" << std::endl;
		if (temps_inter >= 0.95) {
			coup = static_cast<int>(meilleur_coup);
			break;
		}

		  EtatJeux tmp4(_etat_jeux);
			tmp4.jouer(listCoupsPossibles[i]);
			// score = alpha_beta(profondeur, tmp4, INT32_MIN, INT32_MAX, false);
			score = alpha_beta(top_depart, profondeur, tmp4, INT32_MIN, INT32_MAX, false);

		// std::cout << " coup : " << listCoupsPossibles[i] << " score : " << score  << std::endl;
			if (meilleur_score < score) {
				meilleur_score = score;
				meilleur_coup = listCoupsPossibles[i];
			}
		}
	} else {

		for (int i = 0; i < millieu; ++i) {
			temps_inter = (clock()-top_depart)/ (double)(CLOCKS_PER_SEC / 10000);
			// std::cout << "Chrono : " << temps_inter << " s" << std::endl;
		if (temps_inter >= 0.95) {
			coup = static_cast<int>(meilleur_coup);
			break;
		}

			EtatJeux tmp(_etat_jeux);
			tmp.jouer(listCoupsPossibles[millieu + i]);
			// score = alpha_beta(profondeur, tmp, INT32_MIN, INT32_MAX, false);
			score = alpha_beta(top_depart, profondeur, tmp, INT32_MIN, INT32_MAX, false);
		// std::cout << " coup : " << listCoupsPossibles[millieu + i] << " score : " << score  << std::endl;
			if (meilleur_score < score) {
				meilleur_score = score;
				meilleur_coup = listCoupsPossibles[millieu + i];
			}

			temps_inter = (clock()-top_depart)/ (double)(CLOCKS_PER_SEC / 10000);
			// std::cout << "Chrono : " << temps_inter << " s" << std::endl;
		if (temps_inter >= 0.95) {
			coup = static_cast<int>(meilleur_coup);
			break;
		}

			EtatJeux tmp2(_etat_jeux);
			tmp2.jouer(listCoupsPossibles[millieu -i -1]);
			// score = alpha_beta(profondeur, tmp2, INT32_MIN, INT32_MAX, false);
			score = alpha_beta(top_depart, profondeur, tmp2, INT32_MIN, INT32_MAX, false);
		// std::cout << " coup : " << listCoupsPossibles[millieu - i - 1] << " score : " << score  << std::endl;
			if (meilleur_score < score) {
				meilleur_score = score;
				meilleur_coup = listCoupsPossibles[millieu - i -1];
			}
		}
	}

	// Joue notre coup pour actualiser notre plateau avant de rendre le mutex
	_etat_jeux.jouer(meilleur_coup);
	// _etat_jeux.afficher();
	// std::cout << "meilleur score : " << meilleur_score << std::endl;
	// std::cout << "Notre coup (Struct prof): " << static_cast<int>(meilleur_coup)+1 << std::endl;
	// std::cout << "Notre coup (Struct bitset): " << static_cast<int>(meilleur_coup) << std::endl;
	coup = static_cast<int>(meilleur_coup);
}

i32 Joueur_AlphaBeta_::alpha_beta(const clock_t td, const u8 profondeur, const EtatJeux &etat_jeux, i32 alpha, i32 beta, const bool isMax) {
	// std::cout << "alpha_beta : " << std::endl;
	// std::cout << " 		profondeur : " << unsigned(profondeur) << std::endl;
	// std::cout << " 		Player : " << (isMax? "Max" : "Min") << std::endl;
	// etat_jeux.afficher();

	//TODO : faire l'estimateur pour l'etat du jeu
	if (unsigned(profondeur) == 0) {
		// std::cout << "profondeur break estimation " << std::endl;
		return etat_jeux.estimation(_joueurAEstimer);
	}

	//TODO : chronometre pour finir à 9,5 milisecondre
	double	temps_inter = (clock()-td)/ (double)(CLOCKS_PER_SEC / 10000);
	// std::cout << "Chrono : " << temps_inter << " s" << std::endl;

	if (temps_inter >= 0.95) {
		// std::cout << "temps break" << std::endl;
		return etat_jeux.estimation(_joueurAEstimer);
	}


 	i32 score;

	EtatJeux::Coup listCoupsPossibles[EtatJeux::nColonnes];
	const u8 taille = etat_jeux.coupsPossibles(listCoupsPossibles);

	const bool jeuxTermine = taille == 0;
	if (jeuxTermine) {
		return etat_jeux.estimation(_joueurAEstimer);
	}

	// std::cout << "alpha_beta test fin." << std::endl;

	i32 millieu = (int)taille / 2;
	i32 reste = (int)taille % 2;


	if (isMax) {
		// std::cout << "ab::max" << std::endl;
		score = INT32_MIN;

		// for (u8 i = 0; i < taille; ++i) {
		// 	// std::cout << "ab::max::coup : " << unsigned(i) <<  std::endl;
		// 	EtatJeux tmp(etat_jeux);
		// 	tmp.jouer(listCoupsPossibles[i]);
		// 	// std::cout << "tmp etat" << std::endl;
		// 	// tmp.afficher();
		// 	// std::cout << "--------" << std::endl;
		// 	// std::cout << "===============ab::max avant rec=============" <<  std::endl;
		// 	score = std::max(score, alpha_beta(profondeur-1, tmp, alpha, beta, false));
		// 	// i32 val = alpha_beta(profondeur-1, tmp, alpha, beta, false);
		// 	// score = std::max(score,val);
		// 	// std::cout << "===============ab::max apres rec=============" <<  std::endl;
		// 	// std::cout << "ab::max::score : " << score <<  std::endl;
		// 	// std::cout << "ab::max::val : " << val <<  std::endl;
		// 	// std::cout << "ab::max::score : " << score <<  std::endl;
    //
		// 	// Beta coupure
		// 	if (score >= beta) {
		// 		// std::cout << "Beta coupure" << std::endl;
		// 		return score;
		// 	}
    //
		// 	if (score > alpha) {
		// 		alpha = score;
		// 	}
		// }
		// std::cout << "ab::max::fin" << std::endl;

		if (unsigned(taille) == 1) {
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[0]);
			// score = std::max(score, alpha_beta(profondeur-1, tmp, alpha, beta, false));
			score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, false));
			return score;
		}

		if (reste) {
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[millieu]);
			// score = std::max(score, alpha_beta(profondeur-1, tmp, alpha, beta, false));
			score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, false));

			if (score >= beta) {
				return score;
			}

			if (score > alpha) {
				alpha = score;
			}

			for (int i = 1; i <= millieu; ++i) {
				EtatJeux tmp2(etat_jeux);
				tmp2.jouer(listCoupsPossibles[millieu + i]);
				// score = std::max(score, alpha_beta(profondeur-1, tmp2, alpha, beta, false));
				score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp2, alpha, beta, false));

				if (score >= beta) {
					return score;
				}

				if (score > alpha) {
					alpha = score;
				}

				EtatJeux tmp3(etat_jeux);
				tmp3.jouer(listCoupsPossibles[millieu - i]);
				// score = std::max(score, alpha_beta(profondeur-1, tmp3, alpha, beta, false));
				score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp3, alpha, beta, false));

				if (score >= beta) {
					return score;
				}

				if (score > alpha) {
					alpha = score;
				}
			}

			for (int i = millieu * 2; i <= reste; ++i) {
				EtatJeux tmp4(etat_jeux);
				tmp4.jouer(listCoupsPossibles[i]);
				// score = std::max(score, alpha_beta(profondeur-1, tmp4, alpha, beta, false));
				score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp4, alpha, beta, false));

				if (score >= beta) {
					return score;
				}

				if (score > alpha) {
					alpha = score;
				}
			}

		} else {
			for (int i = 0; i < millieu; ++i) {
				EtatJeux tmp(etat_jeux);
				tmp.jouer(listCoupsPossibles[millieu + i]);
				// score = std::max(score, alpha_beta(profondeur-1, tmp, alpha, beta, false));
				score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, false));

				if (score >= beta) {
					return score;
				}

				if (score > alpha) {
					alpha = score;
				}

				EtatJeux tmp2(etat_jeux);
				tmp2.jouer(listCoupsPossibles[millieu - i -1]);
				// score = std::max(score, alpha_beta(profondeur-1, tmp2, alpha, beta, false));
				score = std::max(score, alpha_beta(temps_inter, profondeur-1, tmp2, alpha, beta, false));

				if (score >= beta) {
					return score;
				}

				if (score > alpha) {
					alpha = score;
				}
			}
		}

	} else {
		// std::cout << "ab::min" << std::endl;
		score = INT32_MAX;

		// for (u8 i = 0; i < taille; ++i) {
		// 	// std::cout << "ab::min::coup : " << unsigned(i) <<  std::endl;
		// 	EtatJeux tmp(etat_jeux);
		// 	tmp.jouer(listCoupsPossibles[i]);
		// 	// std::cout << "tmp etat" << std::endl;
		// 	// tmp.afficher();
		// 	// std::cout << "--------" << std::endl;
		// 	score = std::min(score, alpha_beta(profondeur-1, tmp, alpha, beta, true));
		// 	// i32 val = alpha_beta(profondeur-1, tmp, alpha, beta, true);
		// 	// score = std::min(score,val);
		// 	// std::cout << "ab::min::score : " << score <<  std::endl;
		// 	// std::cout << "ab::min::val : " << val <<  std::endl;
		// 	// std::cout << "ab::min::score : " << score <<  std::endl;
    //
		// 	// Alpha coupure
		// 	if (score <= alpha) {
		// 		// std::cout << "Alpha coupure" << std::endl;
		// 		return score;
		// 	}
    //
		// 	if (score < beta) {
		// 		beta = score;
		// 	}
		// }
		// // std::cout << "ab::min::fin" << std::endl;

		if (unsigned(taille) == 1) {
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[0]);
			// score = std::min(score, alpha_beta(profondeur-1, tmp, alpha, beta, true));
			score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, true));
			return score;
		}

		if (reste) {
			EtatJeux tmp(etat_jeux);
			tmp.jouer(listCoupsPossibles[millieu]);
			// score = std::min(score, alpha_beta(profondeur-1, tmp, alpha, beta, true));
			score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, true));

			if (score <= alpha) {
				return score;
			}
			if (score < beta) {
				beta = score;
			}

			for (int i = 1; i <= millieu; ++i) {
				EtatJeux tmp2(etat_jeux);
				tmp2.jouer(listCoupsPossibles[millieu + i]);
				// score = std::min(score, alpha_beta(profondeur-1, tmp2, alpha, beta, true));
				score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp2, alpha, beta, true));

				if (score <= alpha) {
					return score;
				}
				if (score < beta) {
					beta = score;
				}

				EtatJeux tmp3(etat_jeux);
				tmp3.jouer(listCoupsPossibles[millieu - i]);
				// score = std::min(score, alpha_beta(profondeur-1, tmp3, alpha, beta, true));
				score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp3, alpha, beta, true));

				if (score <= alpha) {
					return score;
				}
				if (score < beta) {
					beta = score;
				}
			}

			for (int i = millieu * 2; i <= reste; ++i) {
				EtatJeux tmp(etat_jeux);
				tmp.jouer(listCoupsPossibles[i]);
				// score = std::min(score, alpha_beta(profondeur-1, tmp, alpha, beta, true));
				score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, true));

				if (score <= alpha) {
					return score;
				}
				if (score < beta) {
					beta = score;
				}
			}
		} else {
			for (int i = 0; i < millieu; ++i) {

				EtatJeux tmp(etat_jeux);
				tmp.jouer(listCoupsPossibles[millieu + i]);
				// score = std::min(score, alpha_beta(profondeur-1, tmp, alpha, beta, true));
				score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp, alpha, beta, true));

				if (score <= alpha) {
					return score;
				}
				if (score < beta) {
					beta = score;
				}

				EtatJeux tmp2(etat_jeux);
				tmp2.jouer(listCoupsPossibles[millieu - i - 1]);
				// score = std::min(score, alpha_beta(profondeur-1, tmp2, alpha, beta, true));
				score = std::min(score, alpha_beta(temps_inter, profondeur-1, tmp2, alpha, beta, true));

				if (score <= alpha) {
					return score;
				}
				if (score < beta) {
					beta = score;
				}
			}
		}
	}

	// std::cout << "alpha_beta fin." << std::endl;
	// std::cout << "ab::score : " << score << std::endl;
	return score;
}
