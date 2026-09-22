#ifndef PLAYER_HPP
#define PLAYER_HPP

/*
 * Correspondance avec les fichiers du dossier image/Player :
 * DIR_DOWN  -> P_F_x.png (de face, vu par le joueur)
 * DIR_UP    -> P_B_x.png (de dos)
 * DIR_LEFT  -> P_L_x.png
 * DIR_RIGHT -> P_R_x.png
 *
 * DIR_COUNT n'est pas une direction : c'est le nombre de directions.
 */
enum PlayerDirection
{
    DIR_DOWN,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_COUNT
};

/*
 * Déduit la direction à partir d'un déplacement (dx, dy).
 * Un seul des deux doit être non nul.
 */
PlayerDirection directionFromDelta(int dx, int dy);

#endif
