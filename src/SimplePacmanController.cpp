#include "SimplePacmanController.h"
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

SimplePacmanController::SimplePacmanController(std::shared_ptr<Character> character):
	Controller(character){
}

SimplePacmanController::~SimplePacmanController() {
	// TODO Auto-generated destructor stub
}


Move SimplePacmanController::getClosestMove(const GameState& game, std::pair<int,int> target)const{
	int minDist=10000000;
	Move minMove=character->getDirection();
	std::vector<Move> moves=game.getMaze().getPossibleMoves(character->getPos());
	for(Move m:moves){
		int vecino = game.getMaze().getNeighbour(character->getPos(),m);
		if(vecino<0)continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int sqDist=euclid2(vecinoCoords,target);
		if(sqDist<minDist){
			minDist=sqDist;
			minMove=m;
		}
	}
	return minMove;
}

Move SimplePacmanController::getFarthestMove(const GameState& game, std::pair<int,int> target)const{
	int maxDist=-1;
	Move maxMove=character->getDirection();
	std::vector<Move> moves=game.getMaze().getPossibleMoves(character->getPos());
	for(Move m:moves){
		int vecino = game.getMaze().getNeighbour(character->getPos(),m);
		if(vecino<0)continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int sqDist=euclid2(vecinoCoords,target);
		if(sqDist>maxDist){
			maxDist=sqDist;
			maxMove=m;
		}
	}
	return maxMove;
}

float SimplePacmanController::getDistanceToGhost(const GameState& game, int g)const{
	return sqrt(euclid2(
		game.getMaze().getNodePos(character->getPos()),
		game.getMaze().getNodePos(game.getGhostsPos(g))));
}
std::pair<int,int>
getClosestPellet(
	const GameState& game,
	int pacmanNode)
{
	auto pacmanPos =
		game.getMaze().getNodePos(pacmanNode);

	auto pills =
		game.getMaze().getPillPositions();

	float minDist = 999999;

	std::pair<int,int> best = pacmanPos;

	for(auto p : pills){

		float d = euclid2(pacmanPos, p);

		if(d < minDist){
			minDist = d;
			best = p;
		}
	}

	return best;
}
Move SimplePacmanController::getMove(const GameState& game){

	//para cerrar la ventana
	SDL_Event e;
	if( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT || 
			(e.type == SDL_KEYDOWN && 
				(e.key.keysym.sym==SDLK_ESCAPE || 
				e.key.keysym.sym==SDLK_q) ))
		{
			SDL_Quit();
			exit(0);
		}
	}
	
	int pacmanNode = character->getPos();
	auto pacmanCoords = game.getMaze().getNodePos(pacmanNode);
	
	std::vector<std::pair<int,int>> ghostPositions;
	for(int i=0;i<4;i++){
		ghostPositions.push_back(game.getMaze().getNodePos(game.getGhostsPos(i)));
	}

	std::vector<bool> ghostsEdible;
	for(int i=0;i<4;i++){
		ghostsEdible.push_back(game.isGhostEdible(i));
	}

	auto powerPillPositions=game.getMaze().getPowerPillPositions();	


	float fear=0.0f;
	Move escapeMove=PASS;
	float hunger=0.0f;
	Move eatGhostMove=PASS;

	for(int i=0;i<4;i++){
		if((!ghostsEdible[i])){
			float tempFear=1.0f-1.0f/(1.0f+pow(2.718f * 0.45f,-getDistanceToGhost(game,i)+32.0f));//logistica
			if(tempFear>fear){
				fear=tempFear;
				escapeMove=getFarthestMove(game,ghostPositions[i]);
			}
		}
		
	}

	//perseguir fantasmas azules
	for(int i=0;i<4;i++){
		if(ghostsEdible[i]){
			float tempHunger=pow(100.0f-getDistanceToGhost(game,i),2)/pow(100.0,2);//cuadratica
			if(tempHunger>hunger){
				hunger=tempHunger;
				eatGhostMove=getClosestMove(game,ghostPositions[i]);;
			}
		}
	}
	//std::cout<<"fear="<<fear<<std::endl;
	//std::cout<<"hunger="<<hunger<<std::endl;

	// PRIORIDAD 1: escapar
	if(fear > 0.6f){
		return escapeMove;
	}

	// PRIORIDAD 2: comer fantasmas azules
	if(hunger > 0.3f){
		return eatGhostMove;
	}

	// PRIORIDAD 3: moverse normalmente
	std::vector<Move> moves =
		game.getMaze().getPossibleMoves(character->getPos());

	if(!moves.empty()){

		for(auto m : moves){
			if(m == character->getDirection()){
				return m;
			}
		}

		return moves[rand() % moves.size()];
	}

	return PASS;
	
}
