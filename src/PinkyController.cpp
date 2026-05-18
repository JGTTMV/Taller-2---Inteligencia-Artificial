#include "PinkyController.h"

PinkyController::PinkyController(
    std::shared_ptr<Character> character
) :
Controller(character),
root(std::make_shared<Selector>())
{

    auto filter = std::make_shared<Filter>();
    filter->addCondition(std::make_shared<Powerpill>());
    filter->addAction(std::make_shared<Frightened>());
    root->addChild(filter);

    auto filter2 = std::make_shared<Filter>();
    filter2->addCondition(std::make_shared<TimeOut>());
    filter2->addAction(std::make_shared<Scatter>());
    root->addChild(filter2);

    root->addChild(std::make_shared<PinkyChase>());
}

PinkyController::~PinkyController(){

}

Move PinkyController::getMove(const GameState& gs){

    Info::getInfo()->in_character = character;
    Info::getInfo()->in_gamestate = &gs;

    root->tick();

    return Info::getInfo()->out_move;
}

Status PinkyChase::update(){

    auto character = Info::getInfo()->in_character;
    auto gs = Info::getInfo()->in_gamestate;

    auto target =
        gs->getMaze().getNodePos(gs->getPacmanPos());

    target.first += 4;
    target.second -= 4;

    float min = 1000000000;
    Move minMove = PASS;

    std::vector<Move> moves;

    if(character->getDirection()==PASS) {

        moves = gs->getMaze().getPossibleMoves(
            character->getPos());

    } else {

        moves = gs->getMaze().getGhostLegalMoves(
            character->getPos(),
            character->getDirection());
    }

    for(auto move : moves){

        if(move == PASS){
            continue;
        }

        float dist = euclid2(
            target,
            gs->getMaze().getNodePos(
                gs->getMaze().getNeighbour(
                    character->getPos(),
                    move
                )
            )
        );

        if(dist < min){
            min = dist;
            minMove = move;
        }
    }

    Info::getInfo()->out_move = minMove;

    return BH_SUCCESS;
}