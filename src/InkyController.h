#include "Controller.h"

class InkyController : public Controller {

private:
    float euclid2(
        std::pair<int,int> a,
        std::pair<int,int> b
    ) const;

    Move getClosestMove(
        const GameState& game,
        std::pair<int,int> target
    ) const;

    Move getFarthestMove(
        const GameState& game,
        std::pair<int,int> target
    ) const;

    std::pair<int,int> getPredictedTarget(
        const GameState& game
    ) const;

    float evaluateEscape(
        const GameState& game
    ) const;

    float evaluatePredictChase(
        const GameState& game
    ) const;

    float evaluateScatter(
        const GameState& game
    ) const;

public:

    InkyController(
        std::shared_ptr<Character> character
    );

    virtual ~InkyController();

    virtual Move getMove(
        const GameState& game
    ) override;
};