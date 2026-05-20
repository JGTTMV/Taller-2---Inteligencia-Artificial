#pragma once

#include "Controller.h"
#include "FSM.h"
#include <chrono>

class SueStateMachine;

class SueFSMController : public Controller 
{
    std::shared_ptr<SueStateMachine> fsm;
public:
    SueFSMController(std::shared_ptr<Character> character);
    virtual ~SueFSMController();
    virtual Move getMove(const GameState& game) override;
};

//Estados de Sue

class SueChaseState : public FSMState 
{
public:
    SueChaseState(std::shared_ptr<Character> _character);
    Move onUpdate(const GameState& gs) override;
};

class SueScatterState : public FSMState 
{
public:
    SueScatterState(std::shared_ptr<Character> _character);
    Move onUpdate(const GameState& gs) override;
};

class SueFrightenedState : public FSMState 
{
public:
    SueFrightenedState(std::shared_ptr<Character> _character);
    Move onUpdate(const GameState& gs) override;
};

//Estado jerarquico (Gestiona la distancia de 8 celdas)
class SueNonFrightenedState : public FSMState 
{
    std::shared_ptr<FSMState> activeSubState;
    std::shared_ptr<FSMState> chase;
    std::shared_ptr<FSMState> scatter;
public:
    SueNonFrightenedState(std::shared_ptr<Character> _character);
    Move onUpdate(const GameState& gs) override;
};

//Maquinas de estados principal
class SueStateMachine : public FiniteStateMachine 
{
public:
    SueStateMachine(std::shared_ptr<Character> _character);
    Move update(const GameState& gs) override;
};