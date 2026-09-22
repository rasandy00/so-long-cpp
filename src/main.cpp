#include "../include/map.hpp"
#include "../include/window.hpp"
#include "../include/render.hpp"
#include "../include/game.hpp"

#include <iostream>
#include <sstream>

int main()
{
    int level = 0;
    int lives = 3;

    while (level <= 9)
    {
        std::stringstream filename;

        filename << "map/" << level << ".ber";

        std::cout << "============================" << std::endl;
        std::cout << "        NIVEAU " << level << std::endl;
        std::cout << "============================" << std::endl;

        Map map;

        if (!map.loadMap(filename.str()))
        {
            std::cerr << "Erreur : impossible de charger "
                      << filename.str() << std::endl;
            return 1;
        }

        Window window(
            map.getWidth() * 32,
            map.getLength() * 32,
            "So Long"
        );

        if (window.getWindow() == NULL ||
            window.getRenderer() == NULL)
        {
            std::cerr << "Erreur : impossible de créer la fenêtre."
                      << std::endl;
            return 1;
        }

        Render render(window.getRenderer());

        if (!render.loadTextures())
        {
            std::cerr << "Erreur : impossible de charger les textures."
                      << std::endl;
            return 1;
        }

        Game game(
            map,
            render,
            window.getRenderer(),
            lives
        );

        game.run();

        lives = game.getLives();

        /*
         * Échap ou la croix de fermeture : on quitte
         * le programme immédiatement, sans message d'échec.
         */
        if (game.hasQuit())
        {
            std::cout << "Jeu quitté." << std::endl;
            return 0;
        }

        /*
         * Plus de vies : le jeu s'arrête complètement,
         * sans repasser par le niveau suivant.
         */
        if (lives <= 0)
        {
            std::cout << "GAME OVER." << std::endl;
            return 0;
        }

        if (!game.hasWon())
        {
            std::cout << "Niveau interrompu." << std::endl;
            return 0;
        }

        /*
         * Le niveau est terminé : une vie bonus,
         * puis on passe immédiatement au suivant.
         */
        lives++;

        std::cout << "Niveau "
                  << level
                  << " terminé ! Vies : "
                  << lives
                  << std::endl;

        level++;
    }

    std::cout << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "          BRAVO !           " << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Tous les niveaux sont terminés !"
              << std::endl;

    return 0;
}

