#include "../include/game.hpp"
#include "../include/map.hpp"
#include "../include/render.hpp"

#include <iostream>
#include <cstdlib>

namespace
{
    /*
     * Durée du flash noir affiché au moment de l'impact.
     */
    const Uint32 FLASH_DURATION_MS = 200;

    /*
     * Durée pendant laquelle le joueur clignote
     * après être réapparu au point de départ.
     */
    const Uint32 BLINK_DURATION_MS = 1200;
    const Uint32 BLINK_INTERVAL_MS = 150;

    /*
     * Durée pour glisser d'une case à la case voisine.
     */
    const Uint32 MOVE_DURATION_MS = 220;
}

Game::Game(Map &map, Render &render, SDL_Renderer *renderer, int lives)
    : _map(map),
      _render(render),
      _renderer(renderer),
      _running(true),
      _won(false),
      _quit(false),
      _lives(lives),
      _trapStartTime(SDL_GetTicks()),
      _trapState(TRAP_RETRACTED),
      _hitTime(0),
      _blinkUntil(0),
      _isMoving(false),
      _moveStartTime(0),
      _facing(DIR_DOWN),
      _idleFrame(0),
      _moveStartFrame(0),
      _moveSwingFrame(1)
{
    Position start = _map.getPlayerPosition();

    _moveFrom = start;
    _moveTo = start;
}

Game::~Game()
{
}

void Game::run()
{
    while (_running)
    {
        handleEvents();
        updateTraps();
        checkTrapCollision();

        Uint32 now = SDL_GetTicks();

        /*
         * Flash noir juste après l'impact.
         */
        bool showFlash = (now - _hitTime) < FLASH_DURATION_MS;

        /*
         * Clignotement du joueur pendant qu'il vient
         * de réapparaître au point de départ.
         */
        bool playerVisible = true;

        if (now < _blinkUntil)
            playerVisible = ((now / BLINK_INTERVAL_MS) % 2) == 0;

        PlayerDirection dir;
        int frame;
        int pixelX;
        int pixelY;

        getRenderState(dir, frame, pixelX, pixelY);

        SDL_SetRenderDrawColor(
            _renderer,
            0,
            0,
            0,
            255
        );

        SDL_RenderClear(_renderer);

        _render.renderMap(_map, _trapState, showFlash, playerVisible,
                           dir, frame, pixelX, pixelY);

        SDL_RenderPresent(_renderer);

        SDL_Delay(16);
    }
}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            _running = false;
            _quit = true;
        }

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                _running = false;
                _quit = true;
            }
            else if (!_isMoving &&
                     (event.key.keysym.sym == SDLK_UP ||
                      event.key.keysym.sym == SDLK_z))
            {
                movePlayer(0, -1);
            }
            else if (!_isMoving &&
                     (event.key.keysym.sym == SDLK_DOWN ||
                      event.key.keysym.sym == SDLK_s))
            {
                movePlayer(0, 1);
            }
            else if (!_isMoving &&
                     (event.key.keysym.sym == SDLK_LEFT ||
                      event.key.keysym.sym == SDLK_q))
            {
                movePlayer(-1, 0);
            }
            else if (!_isMoving &&
                     (event.key.keysym.sym == SDLK_RIGHT ||
                      event.key.keysym.sym == SDLK_d))
            {
                movePlayer(1, 0);
            }
        }
    }
}

void Game::movePlayer(int dx, int dy)
{
    Position current = _map.getPlayerPosition();

    int newX = current.x + dx;
    int newY = current.y + dy;

    if (!canMove(newX, newY))
        return;

    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    /*
     * Chaque ligne de la map contient :
     *
     * largeur caractères + '\n'
     *
     * Exemple :
     *
     * 111111
     * 1P00E1
     * 1C0001
     * 111111
     *
     * Pour trouver une position :
     *
     * index = y * (width + 1) + x
     */

    int oldIndex = current.y * (width + 1) + current.x;
    int newIndex = newY * (width + 1) + newX;

    char destination = mapData[newIndex];

    /*
     * Le joueur quitte son ancienne case.
     * Si c'était un piège, il doit rester un piège.
     */
    if (_map.isTrap(current.x, current.y))
        mapData[oldIndex] = 'T';
    else
        mapData[oldIndex] = '0';

    /*
     * Le joueur arrive sur la nouvelle case.
     */
    if (destination == 'C')
    {
        collectItem(newX, newY);
        mapData[newIndex] = 'P';
    }
    else if (destination == 'E')
    {
        /*
         * La sortie devient accessible seulement
         * lorsque tous les collectibles sont récupérés.
         */
        if (checkWin(newX, newY))
        {
            mapData[newIndex] = 'P';
            _map.setMap(mapData);
            _map.setPlayerPosition(newX, newY);

            std::cout << "========================" << std::endl;
            std::cout << "       VICTOIRE !       " << std::endl;
            std::cout << "========================" << std::endl;

            _won = true;
            _running = false;
            return;
        }

        /*
         * Si tous les collectibles n'ont pas été récupérés,
         * le joueur ne peut pas entrer dans la sortie.
         */
        mapData[oldIndex] = 'P';
        return;
    }
    else
    {
        mapData[newIndex] = 'P';
    }

    _map.setMap(mapData);
    _map.setPlayerPosition(newX, newY);

    startMoveAnimation(current, newX, newY, dx, dy);
}

bool Game::canMove(int x, int y)
{
    /*
     * Vérification des limites de la map.
     */
    if (x < 0 || y < 0)
        return false;

    if (x >= _map.getWidth())
        return false;

    if (y >= _map.getLength())
        return false;

    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    /*
     * +1 car chaque ligne possède '\n'.
     */
    int index = y * (width + 1) + x;

    /*
     * Vérification de sécurité.
     */
    if (index < 0 ||
        index >= static_cast<int>(mapData.length()))
    {
        return false;
    }

    /*
     * Le joueur ne peut pas traverser un mur.
     */
    if (mapData[index] == '1')
        return false;

    /*
     * Le joueur ne peut pas entrer dans la sortie
     * tant que tous les collectibles ne sont pas récupérés.
     */
    if (mapData[index] == 'E' &&
        _map.getCollectibles() > 0)
    {
        std::cout << "Il reste "
                  << _map.getCollectibles()
                  << " collectible(s)."
                  << std::endl;

        return false;
    }

    return true;
}

void Game::collectItem(int x, int y)
{
    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    int index = y * (width + 1) + x;

    if (mapData[index] == 'C')
    {
        int remaining = _map.getCollectibles();

        if (remaining > 0)
            remaining--;

        _map.setCollectibles(remaining);

        std::cout << "Collectible récupéré ! "
                  << "Il reste : "
                  << remaining
                  << std::endl;
    }
}

bool Game::checkWin(int x, int y)
{
    Position exitPosition = _map.getExitPosition();

    /*
     * Vérifie que le joueur est bien
     * sur la position de la sortie.
     */
    if (x != exitPosition.x ||
        y != exitPosition.y)
    {
        return false;
    }

    /*
     * Tous les collectibles doivent avoir
     * été récupérés.
     */
    if (_map.getCollectibles() != 0)
        return false;

    return true;
}

bool Game::hasWon() const
{
    return _won;
}

bool Game::hasQuit() const
{
    return _quit;
}

int Game::getLives() const
{
    return _lives;
}

/*
 * L'état des pièges dépend uniquement du temps écoulé
 * depuis le début du niveau, pas du nombre de frames.
 */
void Game::updateTraps()
{
    _trapState = getTrapState(SDL_GetTicks() - _trapStartTime);
}

/*
 * Appelée à chaque frame : le joueur est touché aussi bien
 * s'il marche sur un piège dangereux que si les épines sortent
 * pendant qu'il est dessus.
 */
void Game::checkTrapCollision()
{
    if (!isTrapDangerous(_trapState))
        return;

    Position player = _map.getPlayerPosition();

    if (_map.isTrap(player.x, player.y))
        hurtPlayer();
}

/*
 * Conséquence d'un contact avec un piège dangereux.
 * Comportement actuel : retour à la case de départ.
 * C'est le seul endroit à modifier pour changer la sanction
 * (perte de vie, game over, ...).
 */
void Game::hurtPlayer()
{
    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    Position current = _map.getPlayerPosition();
    Position start = _map.getStartPosition();

    mapData[current.y * (width + 1) + current.x] = 'T';
    mapData[start.y * (width + 1) + start.x] = 'P';

    _map.setMap(mapData);
    _map.setPlayerPosition(start.x, start.y);

    _lives--;

    Uint32 now = SDL_GetTicks();
    _hitTime = now;
    _blinkUntil = now + BLINK_DURATION_MS;

    /*
     * On interrompt toute marche en cours : le repère visuel
     * est replacé directement sur le point de départ. Le flash
     * noir et le clignotement masquent ce saut.
     */
    _isMoving = false;
    _moveFrom = start;
    _moveTo = start;
    _facing = DIR_DOWN;
    _idleFrame = 0;

    std::cout << "Aïe ! Les épines vous ont touché. Vies restantes : "
              << _lives
              << std::endl;

    if (_lives <= 0)
    {
        std::cout << "Plus de vies : GAME OVER." << std::endl;
        _running = false;
    }
}

/*
 * Démarre le glissement visuel d'une case à la case voisine.
 * La logique (position réelle, collisions) est déjà à jour au
 * moment de l'appel ; seule l'apparence suit encore.
 */
void Game::startMoveAnimation(Position from, int toX, int toY, int dx, int dy)
{
    _moveFrom = from;
    _moveTo.x = toX;
    _moveTo.y = toY;

    _facing = directionFromDelta(dx, dy);

    /*
     * Un pas complet : pieds joints (posture actuelle) -> pied qui
     * avance -> pieds joints (posture opposée). La posture opposée
     * devient l'état "au repos" jusqu'au prochain déplacement.
     */
    _moveStartFrame = _idleFrame;
    _moveSwingFrame = (_idleFrame == 0) ? 1 : 2;
    _idleFrame = (_idleFrame == 0) ? 3 : 0;

    _isMoving = true;
    _moveStartTime = SDL_GetTicks();
}

/*
 * Calcule ce qu'il faut afficher pour le joueur à l'instant présent :
 * direction, image de la marche, et position en pixels. Cette
 * position en pixels est interpolée entre _moveFrom et _moveTo en
 * fonction du temps écoulé, pas du nombre de frames, ce qui donne un
 * glissement régulier quelle que soit la vitesse de la machine.
 */
void Game::getRenderState(PlayerDirection &dir, int &frame,
                           int &pixelX, int &pixelY)
{
    int tileSize = _render.getTileSize();

    dir = _facing;

    if (_isMoving)
    {
        Uint32 elapsed = SDL_GetTicks() - _moveStartTime;

        if (elapsed >= MOVE_DURATION_MS)
        {
            _isMoving = false;
        }
        else
        {
            if (elapsed < MOVE_DURATION_MS / 3)
                frame = _moveStartFrame;
            else if (elapsed < (MOVE_DURATION_MS * 2) / 3)
                frame = _moveSwingFrame;
            else
                frame = _idleFrame;

            int fromX = _moveFrom.x * tileSize;
            int fromY = _moveFrom.y * tileSize;
            int toX = _moveTo.x * tileSize;
            int toY = _moveTo.y * tileSize;

            pixelX = fromX + (toX - fromX)
                            * static_cast<int>(elapsed)
                            / static_cast<int>(MOVE_DURATION_MS);
            pixelY = fromY + (toY - fromY)
                            * static_cast<int>(elapsed)
                            / static_cast<int>(MOVE_DURATION_MS);

            return;
        }
    }

    /*
     * Immobile : le joueur reste sur sa case, pieds joints.
     */
    frame = _idleFrame;
    pixelX = _moveTo.x * tileSize;
    pixelY = _moveTo.y * tileSize;
}
