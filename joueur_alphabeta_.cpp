#include "joueur_alphabeta_.h"

u64 EtatJeux::etatOccupation() const noexcept
{
	return jouers[0] || jouers[1];
}

bool EtatJeux::coupLicite(const MaskCoup coup) const noexcept
{
	return !(etatOccupation() & coup);
}

// a ameliorer, voire faire directement dans le alpha beta pour pas passer par un vector inutile
std::vector<EtatJeux::Coup> EtatJeux::coupsPossibles() const noexcept
{
	std::vector<Coup> coupsPossibles;
	coupsPossibles.reserve(7);

	const u64 etat = ~etatOccupation();
	if (etat & MaskCoup1) coupsPossibles.push_back(Coup1);
	if (etat & MaskCoup2) coupsPossibles.push_back(Coup2);
	if (etat & MaskCoup3) coupsPossibles.push_back(Coup3);
	if (etat & MaskCoup4) coupsPossibles.push_back(Coup4);
	if (etat & MaskCoup5) coupsPossibles.push_back(Coup5);
	if (etat & MaskCoup6) coupsPossibles.push_back(Coup6);
	if (etat & MaskCoup7) coupsPossibles.push_back(Coup7);

	return coupsPossibles;
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
