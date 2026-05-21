#include "SueFSMController.h"
#include "FSMController.h"
#include "Ghost.h"
#include <cmath>
#include <vector>
#include <cstdlib>

SueFSMController::SueFSMController(std::shared_ptr<Character> character) :
    Controller(character),
    fsm(std::make_shared<SueStateMachine>(character)) {}

SueFSMController::~SueFSMController() {}

Move SueFSMController::getMove(const GameState& game) 
{
    return fsm->update(game);
}

//Estado de persecucion/Chase (Persigue a Ms Pac-Man directamente)
SueChaseState::SueChaseState(std::shared_ptr<Character> _character) : FSMState(_character) {}

Move SueChaseState::onUpdate(const GameState& game) 
{
    const auto myPos = character->getPos();

    std::vector<Move> moves;

    if(character->getDirection() == PASS)
    {
        moves = game.getMaze().getPossibleMoves(myPos);
    }
    else
    {
        moves = game.getMaze().getGhostLegalMoves(
            myPos,
            character->getDirection()
        );
    }

    if (moves.empty()) return PASS;

    const auto pacmanCoord =
        game.getMaze().getNodePos(
            game.getPacmanPos()
        );
    
    float min = 1e10;
    int minI = 0;

    for (unsigned int i = 0; i < moves.size(); i++) 
    {
        auto neighbor =
            game.getMaze().getNeighbour(
                myPos,
                moves[i]
            );

        float dist =
            euclid2(
                game.getMaze().getNodePos(neighbor),
                pacmanCoord
            );

        if (dist < min) 
        {
            min = dist;
            minI = i;
        }
    }

    return moves[minI];
}

//Estado de dispersion/Scatter (Huye a la esquina inferior izquierda: 0, 31)
SueScatterState::SueScatterState(std::shared_ptr<Character> _character) : FSMState(_character) {}

Move SueScatterState::onUpdate(const GameState& game) 
{
    const auto myPos = character->getPos();

    std::vector<Move> moves;

    if(character->getDirection() == PASS)
    {
        moves = game.getMaze().getPossibleMoves(myPos);
    }
    else
    {
        moves = game.getMaze().getGhostLegalMoves(
            myPos,
            character->getDirection()
        );
    }

    if (moves.empty()) return PASS;

    std::pair<int, int> safetyCorner = {0, 31};
    
    float min = 1e10;
    int minI = 0;

    for (unsigned int i = 0; i < moves.size(); i++) 
    {
        auto neighbor =
            game.getMaze().getNeighbour(
                myPos,
                moves[i]
            );

        float dist =
            euclid2(
                game.getMaze().getNodePos(neighbor),
                safetyCorner
            );

        if (dist < min) 
        {
            min = dist;
            minI = i;
        }
    }

    return moves[minI];
}

//Estado de miedo/Frightened (Movimiento aleatorio)
SueFrightenedState::SueFrightenedState(std::shared_ptr<Character> _character) : FSMState(_character) {}

Move SueFrightenedState::onUpdate(const GameState& game) 
{
    const auto myPos = character->getPos();

    std::vector<Move> moves;

    if(character->getDirection() == PASS)
    {
        moves = game.getMaze().getPossibleMoves(myPos);
    }
    else
    {
        moves = game.getMaze().getGhostLegalMoves(
            myPos,
            character->getDirection()
        );
    }

    if (!moves.empty()) 
    {
        return moves[rand() % moves.size()];
    }

    return PASS;
}

//Estado jerarquico/Non-Frightened (Gestiona la distancia de 8 celdas)
SueNonFrightenedState::SueNonFrightenedState(std::shared_ptr<Character> _character) : FSMState(_character) 
{
    chase = std::make_shared<SueChaseState>(_character);
    scatter = std::make_shared<SueScatterState>(_character);
    activeSubState = scatter;
}

Move SueNonFrightenedState::onUpdate(const GameState& game) 
{
    //Calcula la distancia euclidiana real a Ms Pac-Man
    float dist = sqrt(euclid2(
        game.getMaze().getNodePos(character->getPos()),
        game.getMaze().getNodePos(game.getPacmanPos())
    ));

    //Si esta a menos de 8 celdas, se dispersa (huye)
    //Si esta lejos, persigue
    if (dist < 8.0f) 
    {
        activeSubState = scatter;
    } else 
    {
        activeSubState = chase;
    }

    return activeSubState->onUpdate(game);
}

//Maquina de estados principal (Ensamblado de transiciones)
SueStateMachine::SueStateMachine(std::shared_ptr<Character> _character) : FiniteStateMachine(_character) 
{
    auto nonFrightened = std::make_shared<SueNonFrightenedState>(character);
    auto frightened = std::make_shared<SueFrightenedState>(character);

    initialState = nonFrightened;
    activeState = initialState;
    states.push_back(nonFrightened);
    states.push_back(frightened);

    //Se reutilizan las clases de transicion de Blinky en FSMController.h
    nonFrightened->addTransition(std::make_shared<PillTransition>(frightened));
    frightened->addTransition(std::make_shared<TimeTransition>(nonFrightened, 10)); //10s de susto
}

Move SueStateMachine::update(const GameState& gs) 
{
    auto t = activeState->getActiveTransition(gs);
    if (t != nullptr) {
        activeState->onExit(gs);
        t->onTransition(gs);
        activeState = t->getNextState();
        activeState->onEnter(gs);
    }
    return activeState->onUpdate(gs);
}