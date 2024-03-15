#pragma once

#include "joueur.h"
#include <cstdint>
#include <cmath>

using u64 = uint64_t;
using u8 = uint8_t;
using i32 = int32_t;
using usize = size_t;

struct EtatJeux
{
	/*
	 * L'Etat du jeux est represente par un bitmap u64 pour chaque joeur ou les 42 premiers
	 * bits servent de flags boolean pour dire se le jouer occupe la position ou non (1 et 0 respectivement)
	 * tous les 7 bits representent une ligne de la matrice 6x7. On commence par la ligne du haut
	 *
	 * | 0  1  2  3  4  5  6  |
	 * | 7  8  9  10 11 12 13 |
	 * | 14 15 16 17 18 19 20 |
	 * | 21 22 23 24 25 26 27 |
	 * | 28 29 30 31 32 33 34 |
	 * | 35 36 37 38 39 40 41 |
	 * ------------------------
	 *
	 *  Les chiffres representent le numero du bit utilise pour representer la position ou
	 *  0 est le bit le plus faible
	 *
	 */

	// index pour les lookup tables maskCoup et maskColonnes
	enum Coup: u64
	{
		Coup1,
		Coup2,
		Coup3,
		Coup4,
		Coup5,
		Coup6,
		Coup7,
	};
	/* Fonctionne comme un mask pour extraire la valeure de la premiere ligne
	 * a la colonne n pour verifier si le coup est licite
	 */
	static constexpr const u64 nCoups = 7;
	static constexpr const u64 maskCoup[nCoups] = {
		1,
		1 << 1,
		1 << 2,
		1 << 3,
		1 << 4,
		1 << 5,
		1 << 6,
	};
	// Sert a garder une colonne
	static constexpr const u64 nColonnes = 7;
	static constexpr const u64 maskColonne[nColonnes] = {
		34'630'287'489, // (2^0 + 2^7 + 2^14 + 2^21 + 2^28 + 2^35)
		34'630'287'489 << 1,
		34'630'287'489 << 2,
		34'630'287'489 << 3,
		34'630'287'489 << 4,
		34'630'287'489 << 5,
		34'630'287'489 << 6,
	};
	// Sert a garder une ligne
	static constexpr const u64 nLignes = 6;
	static constexpr const u64 maskLigne[nLignes] = {
		127,          // (2^0 + 2^1 + 2^2 + 2^3 + 2^4 + 2^5 + 2^6) Voire exemple ci-dessus
		16256,        // (2^7 + 2^8 + 2^9 + 2^10 + 2^11 + 2^12 + 2^13)
		2080768,
		266338304,
		34091302912,
		4363686772736,
	};


	// index de jouers
	enum NumeroJouer: u8
	{
		j0 = 0,
		j1 = 1,
	};
	u64 joueurs[2] = { 0, 0 };
	NumeroJouer joueurCourant = j0;

	EtatJeux();
	EtatJeux(const EtatJeux &j);
	EtatJeux& operator=(const EtatJeux &j);

	// retourne un bitmap des cases occupes par les deux jouers (1), et les cases libres (0)
	u64 etatOccupation() const noexcept;

	bool coupLicite(const Coup coup) const noexcept;
	u8 coupsPossiblesIf(Coup coupsPossibles[nCoups]) const noexcept;
	u8 coupsPossibles(Coup coupsPossibles[nCoups]) const noexcept;

	// Suppose que le coup est licite
	void jouer(const Coup coup) noexcept;

	void afficher() const noexcept;
	void afficherBits(const u64 joueur) const noexcept;
	void test() noexcept;

	i32 valeurCoup(const Coup coup) const noexcept;
	i32 estimation() const noexcept;
};

class Joueur_AlphaBeta_ : public Joueur
{
public:
  Joueur_AlphaBeta_(std::string nom, bool joueur);
  char nom_abbrege() const override;

  void recherche_coup(Jeu, int & coup) override;
  i32 alpha_beta(const u8 profondeur, const EtatJeux &etat_jeu, i32 alpha, i32 beta, const bool isMax);
};
