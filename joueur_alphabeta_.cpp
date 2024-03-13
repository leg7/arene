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
