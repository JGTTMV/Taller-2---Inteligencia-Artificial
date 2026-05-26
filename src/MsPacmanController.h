#pragma once

#include "Controller.h"

class MsPacmanController : public Controller {

private:

    enum DecisionType
    {
        DECISION_ESCAPE,
        DECISION_PELLET,
        DECISION_HUNT
    };

    int decisionCount[3];

    bool statsPrinted;

    float getDistance(
        std::pair<int,int> a,
        std::pair<int,int> b
    ) const;

    void printStats();

    Move getClosestMove(
        const GameState& game,
        std::pair<int,int> target
    ) const;

    Move getFarthestMove(
        const GameState& game,
        std::pair<int,int> target
    ) const;

    std::pair<int,int> getClosestPellet(
        const GameState& game
    ) const;

    std::pair<int,int> getClosestEdibleGhost(
        const GameState& game
    ) const;

    float evaluateEscape(
        const GameState& game
    ) const;

    float evaluateEatPellet(
        const GameState& game
    ) const;

    float evaluateHuntGhost(
        const GameState& game
    ) const;


public:

    MsPacmanController(
        std::shared_ptr<Character> character
    );

    virtual ~MsPacmanController();

    virtual Move getMove(
        const GameState& game
    ) override;
};