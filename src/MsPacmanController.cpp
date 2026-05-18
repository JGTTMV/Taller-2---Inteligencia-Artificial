#include "MsPacmanController.h"
#include "Ghost.h"

#include <cmath>

MsPacmanController::MsPacmanController(
    std::shared_ptr<Character> character
) : Controller(character) {

}

MsPacmanController::~MsPacmanController() {

}

Move MsPacmanController::getMove(
    const GameState& game
) {

    float escapeUtility =
        evaluateEscape(game);

    float pelletUtility =
        evaluateEatPellet(game);

    float huntUtility =
        evaluateHuntGhost(game);

    if(
        escapeUtility > pelletUtility &&
        escapeUtility > huntUtility
    ) {

        auto pacmanPos =
            game.getMaze().getNodePos(
                character->getPos()
            );

        float minDist = 9999999;

        std::pair<int,int> closestGhost =
            pacmanPos;

        for(int i = 0; i < 4; i++) {

            auto ghost =
                game.getGhost(i);

            if(ghost->isEdible()) {
                continue;
            }

            auto ghostPos =
                game.getMaze().getNodePos(
                    ghost->getPos()
                );

            float dist =
                euclid2(
                    pacmanPos,
                    ghostPos
                );

            if(dist < minDist) {

                minDist = dist;
                closestGhost = ghostPos;
            }
        }

        return getFarthestMove(
            game,
            closestGhost
        );
    }

    if(
        huntUtility > pelletUtility
    ) {

        auto target =
            getClosestEdibleGhost(game);

        return getClosestMove(
            game,
            target
        );
    }

    auto pellet =
        getClosestPellet(game);

    return getClosestMove(
        game,
        pellet
    );
}

float MsPacmanController::euclid2(
    std::pair<int,int> a,
    std::pair<int,int> b
) const {

    int dx = a.first - b.first;
    int dy = a.second - b.second;

    return dx * dx + dy * dy;
}

Move MsPacmanController::getClosestMove(
    const GameState& game,
    std::pair<int,int> target
) const {

    float minDist = 9999999;
    Move bestMove = PASS;

    std::vector<Move> moves;

    if(character->getDirection() == PASS) {

        moves =
            game.getMaze().getPossibleMoves(
                character->getPos()
            );

    } else {

        moves =
            game.getMaze().getGhostLegalMoves(
                character->getPos(),
                character->getDirection()
            );
    }

    for(Move move : moves) {

        if(move == PASS) {
            continue;
        }

        int nextNode =
            game.getMaze().getNeighbour(
                character->getPos(),
                move
            );

        if(nextNode < 0) {
            continue;
        }

        auto nextPos =
            game.getMaze().getNodePos(
                nextNode
            );

        float dist =
            euclid2(nextPos, target);

        if(dist < minDist) {

            minDist = dist;
            bestMove = move;
        }
    }

    return bestMove;
}

Move MsPacmanController::getFarthestMove(
    const GameState& game,
    std::pair<int,int> target
) const {

    float maxDist = -1;
    Move bestMove = PASS;

    std::vector<Move> moves;

    if(character->getDirection() == PASS) {

        moves =
            game.getMaze().getPossibleMoves(
                character->getPos()
            );

    } else {

        moves =
            game.getMaze().getGhostLegalMoves(
                character->getPos(),
                character->getDirection()
            );
    }

    for(Move move : moves) {

        if(move == PASS) {
            continue;
        }

        int nextNode =
            game.getMaze().getNeighbour(
                character->getPos(),
                move
            );

        if(nextNode < 0) {
            continue;
        }

        auto nextPos =
            game.getMaze().getNodePos(
                nextNode
            );

        float dist =
            euclid2(nextPos, target);

        if(dist > maxDist) {

            maxDist = dist;
            bestMove = move;
        }
    }

    return bestMove;
}

std::pair<int,int>
MsPacmanController::getClosestPellet(
    const GameState& game
) const {

    auto pacmanPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    float minDist = 9999999;

    std::pair<int,int> bestTarget =
        pacmanPos;

    for(int i = 0; i < 1300; i++) {

        if(
            game.getMaze().hasPill(i)
        ) {

            auto pillPos =
                game.getMaze().getNodePos(i);

            float dist =
                euclid2(
                    pacmanPos,
                    pillPos
                );

            if(dist < minDist) {

                minDist = dist;
                bestTarget = pillPos;
            }
        }
    }

    return bestTarget;
}

std::pair<int,int>
MsPacmanController::getClosestEdibleGhost(
    const GameState& game
) const {

    auto pacmanPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    float minDist = 9999999;

    std::pair<int,int> bestTarget =
        pacmanPos;

    bool found = false;

    for(int i = 0; i < 4; i++) {

        auto ghost =
            game.getGhost(i);

        if(!ghost->isEdible()) {
            continue;
        }

        auto ghostPos =
            game.getMaze().getNodePos(
                ghost->getPos()
            );

        float dist =
            euclid2(
                pacmanPos,
                ghostPos
            );

        if(dist < minDist) {

            minDist = dist;
            bestTarget = ghostPos;

            found = true;
        }
    }

    if(!found) {
        return pacmanPos;
    }

    return bestTarget;
}

float MsPacmanController::evaluateEscape(
    const GameState& game
) const {

    auto pacmanPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    float minDist = 9999999;

    bool danger = false;

    for(int i = 0; i < 4; i++) {

        auto ghost =
            game.getGhost(i);

        if(ghost->isEdible()) {
            continue;
        }

        auto ghostPos =
            game.getMaze().getNodePos(
                ghost->getPos()
            );

        float dist =
            euclid2(
                pacmanPos,
                ghostPos
            );

        if(dist < minDist) {

            minDist = dist;
            danger = true;
        }
    }

    if(!danger) {
        return 0.0f;
    }

    return 7000.0f / (minDist + 1.0f);
}

float MsPacmanController::evaluateEatPellet(
    const GameState& game
) const {

    auto pacmanPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    auto pelletPos =
        getClosestPellet(game);

    float dist =
        euclid2(
            pacmanPos,
            pelletPos
        );

    return 5000.0f / (dist + 1.0f);
}

float MsPacmanController::evaluateHuntGhost(
    const GameState& game
) const {

    auto pacmanPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    bool found = false;

    float minDist = 9999999;

    for(int i = 0; i < 4; i++) {

        auto ghost =
            game.getGhost(i);

        if(!ghost->isEdible()) {
            continue;
        }

        found = true;

        auto ghostPos =
            game.getMaze().getNodePos(
                ghost->getPos()
            );

        float dist =
            euclid2(
                pacmanPos,
                ghostPos
            );

        if(dist < minDist) {
            minDist = dist;
        }
    }

    if(!found) {
        return 0.0f;
    }

    return 9000.0f / (minDist + 1.0f);
}

