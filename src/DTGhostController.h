#pragma once
#include "Controller.h"
#include "Ghost.h"

class DTGhostController : public Controller 
{
public:
    DTGhostController(std::shared_ptr<Character> character);
    virtual ~DTGhostController();
    virtual Move getMove(const GameState& game) override;
};