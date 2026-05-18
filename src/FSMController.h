/*
 * FSMController.h
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#ifndef FSMCONTROLLER_H_
#define FSMCONTROLLER_H_

#include "Controller.h"
#include <random>
#include "FSM.h"
#include <chrono>

class BlinkyStateMachine;

class FSMController: public Controller {
	std::mt19937 e;
	std::uniform_int_distribution<int> uniform_dist;
	std::shared_ptr<BlinkyStateMachine> fsm;
public:
	FSMController(std::shared_ptr<Character> character);
	virtual ~FSMController();
	virtual Move getMove(const GameState& game)override;
};

class PillTransition:public FSMTransition{
	int last;
	std::shared_ptr<FSMState> _next;
public:
	PillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class TimeTransition : public FSMTransition {

private:
    std::shared_ptr<FSMState> nextState;

    std::chrono::steady_clock::time_point startTime;

    int duration;

public:

    TimeTransition(
        std::shared_ptr<FSMState> target,
        int seconds
    );

    bool isValid(const GameState& gs) override;

    std::shared_ptr<FSMState> getNextState() override;

    ~TimeTransition();

};

class ChaseState:public FSMState{

public:
	ChaseState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseState();

};

class ScatterState : public FSMState {

public:
    ScatterState(std::shared_ptr<Character> _character);
    Move onUpdate(const GameState& gs) override;
    void onEnter(const GameState& gs) override;
    ~ScatterState();

};

class FrightenedState : public FSMState {

public:

    FrightenedState(std::shared_ptr<Character> _character);

    Move onUpdate(const GameState& gs) override;

    void onEnter(const GameState& gs) override;

    ~FrightenedState();

};

class BlinkyStateMachine: public FiniteStateMachine{

public:
	BlinkyStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~BlinkyStateMachine();

};
#endif /* FSMCONTROLLER_H_ */
