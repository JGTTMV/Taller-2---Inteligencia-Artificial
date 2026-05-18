/*
 * FSMController.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#include "FSMController.h"
#include <iostream>

FSMController::FSMController(
    std::shared_ptr<Character> character)
	: Controller(character)
	{
    fsm = std::make_shared<BlinkyStateMachine>(character);
}

FSMController::~FSMController() {
	// TODO Auto-generated destructor stub
}

Move FSMController::getMove(const GameState& gs){

    auto ghost =
        std::dynamic_pointer_cast<Ghost>(character);

    if(ghost->isEdible()){

        FrightenedState frightened(character);

        return frightened.onUpdate(gs);
    }

    return fsm->update(gs);
}


///////////////////////////////////PillTransition///////////////////////////////
PillTransition::PillTransition(std::shared_ptr<FSMState> next):last(0),_next(next){

}

bool PillTransition::isValid(const GameState& gs){
	int quedan=gs.getMaze().getPillPositions().size();
	if(last!=quedan && quedan%20==0){
		last =quedan;
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> PillTransition::getNextState(){
	return _next;
}

///////////////////////////////TimeTransition///////////////////////////////////////

TimeTransition::TimeTransition(
    std::shared_ptr<FSMState> target,
    int seconds
)
: nextState(target),
  duration(seconds)
{
    startTime = std::chrono::steady_clock::now();
}

bool TimeTransition::isValid(const GameState& ){

    auto now = std::chrono::steady_clock::now();

    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(
            now - startTime
        ).count();

    if(elapsed >= duration){

        startTime = std::chrono::steady_clock::now();

        return true;
    }

    return false;
}

std::shared_ptr<FSMState> TimeTransition::getNextState(){

    return nextState;
}

TimeTransition::~TimeTransition(){

}

///////////////////////////////ChaseState///////////////////////////////////////
ChaseState::ChaseState(std::shared_ptr<Character> _character):FSMState(_character){

}
void ChaseState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}
Move ChaseState::onUpdate(const GameState& game){
	std::vector<Move> moves;
	const auto pacmanCoord=game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int minI=0;
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			pacmanCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return moves[minI];
}
ChaseState::~ChaseState(){

}

///////////////////////////////ScatterState///////////////////////////////////////

ScatterState::ScatterState(std::shared_ptr<Character> _character)
: FSMState(_character) {

}

void ScatterState::onEnter(const GameState& ){
    std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move ScatterState::onUpdate(const GameState& game){

    std::pair<int,int> target = std::make_pair(9999,9999);

    std::vector<Move> moves;

    const auto myPos = character->getPos();

    if(character->getDirection() == PASS){
        moves = game.getMaze().getPossibleMoves(myPos);
    } else {
        moves = game.getMaze().getGhostLegalMoves(
            myPos,
            character->getDirection()
        );
    }

    if(moves.empty()){
        return PASS;
    }

    Move bestMove = moves[0];

    float min = euclid2(
        game.getMaze().getNodePos(
            game.getMaze().getNeighbour(myPos, bestMove)
        ),
        target
    );

    for(unsigned int i=1;i<moves.size();i++){

        auto dist = euclid2(
            game.getMaze().getNodePos(
                game.getMaze().getNeighbour(myPos,moves[i])
            ),
            target
        );

        if(dist < min){
            min = dist;
            bestMove = moves[i];
        }
    }

    return bestMove;
}

ScatterState::~ScatterState(){

}

///////////////////////////////FrightenedState///////////////////////////////////////

FrightenedState::FrightenedState(
    std::shared_ptr<Character> _character)
: FSMState(_character) {

}

void FrightenedState::onEnter(const GameState& ){
    std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move FrightenedState::onUpdate(const GameState& game){

    auto pacmanPos =
        game.getMaze().getNodePos(game.getPacmanPos());

    std::vector<Move> moves;

    const auto myPos = character->getPos();

    if(character->getDirection() == PASS){
        moves = game.getMaze().getPossibleMoves(myPos);
    } else {
        moves = game.getMaze().getGhostLegalMoves(
            myPos,
            character->getDirection()
        );
    }

    if(moves.empty()){
        return PASS;
    }

    Move bestMove = moves[0];

    float max = euclid2(
        game.getMaze().getNodePos(
            game.getMaze().getNeighbour(myPos, bestMove)
        ),
        pacmanPos
    );

    for(unsigned int i=1;i<moves.size();i++){

        auto dist = euclid2(
            game.getMaze().getNodePos(
                game.getMaze().getNeighbour(myPos,moves[i])
            ),
            pacmanPos
        );

        if(dist > max){
            max = dist;
            bestMove = moves[i];
        }
    }

    return bestMove;
}

FrightenedState::~FrightenedState(){

}

/////////////////////////////////////BlinkyStateMachine/////////////////////////////
BlinkyStateMachine::BlinkyStateMachine(std::shared_ptr<Character> _character):FiniteStateMachine(_character){
	initialState = std::make_shared<ChaseState>(character);
	activeState=initialState;
	states.push_back(initialState);
	auto chase = std::make_shared<ChaseState>(character);
	auto scatter = std::make_shared<ScatterState>(character);

	// Chase -> Scatter
	chase->addTransition(
    	std::make_shared<TimeTransition>(scatter, 7)
	);

	// Scatter -> Chase
	scatter->addTransition(
    	std::make_shared<TimeTransition>(chase, 7)
	);

	activeState = chase;
}



Move BlinkyStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}


BlinkyStateMachine::~BlinkyStateMachine(){

}



