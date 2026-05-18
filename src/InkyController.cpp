#include "InkyController.h"
#include "Ghost.h"

#include <cmath>

InkyController::InkyController(
    std::shared_ptr<Character> character
) : Controller(character) {

}

InkyController::~InkyController() {

}

float InkyController::euclid2(
    std::pair<int,int> a,
    std::pair<int,int> b
) const {

    int dx = a.first - b.first;
    int dy = a.second - b.second;

    return dx * dx + dy * dy;
}

Move InkyController::getClosestMove(
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

Move InkyController::getFarthestMove(
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
InkyController::getPredictedTarget(
    const GameState& game
) const {

    auto pacmanPos =
        game.getMaze().getNodePos(
            game.getPacmanPos()
        );

    auto blinkyPos =
        game.getMaze().getNodePos(
            game.getGhostsPos(0)
        );

    Move dir = static_cast<Move>(game.getPacmanDir());

    int offsetX = 0;
    int offsetY = 0;

    switch(dir) {

        case UP:
            offsetY = -4;
            break;

        case DOWN:
            offsetY = 4;
            break;

        case LEFT:
            offsetX = -4;
            break;

        case RIGHT:
            offsetX = 4;
            break;

        default:
            break;
    }

    std::pair<int,int> predicted = {

        pacmanPos.first + offsetX,
        pacmanPos.second + offsetY
    };

    std::pair<int,int> finalTarget = {

        predicted.first +
        (predicted.first - blinkyPos.first),

        predicted.second +
        (predicted.second - blinkyPos.second)
    };

    return finalTarget;
}

float InkyController::evaluateEscape(
    const GameState& game
) const {

    Ghost* ghost =
        dynamic_cast<Ghost*>(character.get());

    if(!ghost->isEdible()) {
        return 0.0f;
    }

    auto ghostPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    auto pacmanPos =
        game.getMaze().getNodePos(
            game.getPacmanPos()
        );

    float dist =
        euclid2(ghostPos, pacmanPos);

    return 10000.0f / (dist + 1.0f);
}

float InkyController::evaluatePredictChase(
    const GameState& game
) const {

    auto ghostPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    auto target =
        getPredictedTarget(game);

    float dist =
        euclid2(ghostPos, target);

    return 5000.0f / (dist + 1.0f);
}

float InkyController::evaluateScatter(
    const GameState& game
) const {

    auto ghostPos =
        game.getMaze().getNodePos(
            character->getPos()
        );

    auto pacmanPos =
        game.getMaze().getNodePos(
            game.getPacmanPos()
        );

    float dist =
        euclid2(ghostPos, pacmanPos);

    return dist * 0.005f;
}

Move InkyController::getMove(
    const GameState& game
) {

    float escapeUtility =
        evaluateEscape(game);

    float chaseUtility =
        evaluatePredictChase(game);

    float scatterUtility =
        evaluateScatter(game);

    if(
        escapeUtility > chaseUtility &&
        escapeUtility > scatterUtility
    ) {

        auto pacmanPos =
            game.getMaze().getNodePos(
                game.getPacmanPos()
            );

        return getFarthestMove(
            game,
            pacmanPos
        );
    }

    if(
        chaseUtility > scatterUtility
    ) {

        auto target =
            getPredictedTarget(game);

        return getClosestMove(
            game,
            target
        );
    }

	auto blinkyPos =
    	game.getMaze().getNodePos(
        	game.getGhostsPos(0)
    	);

	std::pair<int,int> scatterTarget = {

    	blinkyPos.first + 6,
    	blinkyPos.second - 6
	};

    return getClosestMove(
        game,
        scatterTarget
    );
}