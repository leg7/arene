#include "joueur_alphabeta_.h"

u64 EtatJeux::etatOccupation() const noexcept
{
	return jouers[0] || jouers[1];
}

bool EtatJeux::coupLicite(const MaskCoup coup) const noexcept
{
	return !(etatOccupation() & coup);
}

// a ameliorer, voire faire directement dans le alpha beta pour pas passer par un vecteur inutile
u8 EtatJeux::coupsPossiblesIf(Coup coupsPossibles[nColonnes]) const noexcept
{
	u8 len = 0;

	const u8 premiereLigne = ~(etatOccupation() & 0x7f);
	if (premiereLigne & MaskCoup1) coupsPossibles[len++] = Coup1;
	if (premiereLigne & MaskCoup2) coupsPossibles[len++] = Coup2;
	if (premiereLigne & MaskCoup3) coupsPossibles[len++] = Coup3;
	if (premiereLigne & MaskCoup4) coupsPossibles[len++] = Coup4;
	if (premiereLigne & MaskCoup5) coupsPossibles[len++] = Coup5;
	if (premiereLigne & MaskCoup6) coupsPossibles[len++] = Coup6;
	if (premiereLigne & MaskCoup7) coupsPossibles[len++] = Coup7;

	return len;
}

// Devrait etre meilleurs sur les anciens cpus avec un moins bon predicteur de branche
u8 EtatJeux::coupsPossibles(Coup coupsPossibles[nColonnes]) const noexcept
{
	u8 len = 0;
	u8 premiereLigne = ~(etatOccupation() & 0x7f);

	while (premiereLigne != 0) {
		const Coup coup = static_cast<Coup>(__builtin_ctz(premiereLigne));
		coupsPossibles[len++] = coup;
		premiereLigne &= ~(1 << coup);
	}

	return len;
}

void EtatJeux::jouer(const NumeroJouer numeroJouer, const Coup coup) noexcept
{
	const u64 etat = etatOccupation() & maskColonne[coup];

	for (u8 ligne = nLignes - 1; ligne >= 0; --ligne) {
		if ((etat & maskLigne[ligne]) == 0) {
			jouers[numeroJouer] |= 1 << (coup + ligne * 7);
			break;
		}
	}
}


Joueur_AlphaBeta_::alpha_beta(EtatJeux etat_jeux, i32 alpha, i32 beta, bool isMax) {
	//TODO : faire l'estimateur pour l'etat du jeu
	if (profondeur <= 0) return etat_jeux.estimation();

 	i32 score;

	// TODO : faire la fonction de test si le jeu est fini.
	if (etat_jeux.est_fini()) {
		return etat_jeux.estimation();
	}

	if (isMax) {
		score = INT32_MIN;

		for (auto coup : etat_jeux.coupsPossibles()) {
			etat_jeux.jouer(joueurCourant, coup);
			i32 val = std::max(score, alpha_beta(profondeur-1, etat_jeux, alpha, beta, false));

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

		for (auto coup : etat_jeux.coupsPossibles()) {
			etat_jeux.jouer(joueurCourant, coup)
			score = std::min(score, alpha_beta(profondeur-1, etat_jeux, alpha, beta, false));

			// Alpha coupure
			if (score <= alpha) {
				break;
			}

			if (score < beta) {
				beta = score
			}
		}
	}

	return score;
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
	// TODO : comment passer de Jeu à notre propre structure EtatJeu?
	EtatJeux etat_jeux(j);

	// TODO : Si 1ere ligne 4eme colonne vide, on pose le jeton ?

	u8 profondeur = 10;
	i32 meilleur_score = INT32_MIN;
	i32 score;
	int meilleur_coup;
	Coup listCoupsPossibles[nColonnes];
	u8 taille = etat_jeux.coupsPossibles(listCoupsPossibles);

	for (u8 i = 0; i < taille; ++i) {
		EtatJeux tmp = etat_jeux;
		tmp.jouer(joueurCourant, listCoupsPossibles[i]);
		score = alpha_beta(profondeur, tmp, INT32_MIN, INT32_MAX, true);
		if (meilleur_score < score) {
			meilleur_score = score;
			meilleur_coup = listCoupsPossibles[i];
		}
	}
	coup = meilleur_coup;
}
