#pragma once
#include <vector>
#include "StateMachine.h"
#include "Players.h"
#include "State.h"

class FieldPlayers;
class GoalKeeper;

class Teams
{
public:
   Teams( TEAM::id myTeam );
   ~Teams( void );
   void update();
   void draw();
   const char* getTeamName();
   void sendPlayersHome();
   void changeState( State< Teams >* pToState ) const ;

   void setPlayerWithBall     ( Players* pPlayer ) { m_pPlayerWithBall = pPlayer; }
   void setSupportingPlayer   ( Players* pPlayer ) { m_pSupportingPlayer = pPlayer; }
   void setReceivingPlayer    ( Players* pPlayer ) { m_pPlayerReceivingPass = pPlayer; }
   void setClosestPlayerToBall( Players* pPlayer ) { m_pPlayerClosestToBall = pPlayer; }
   
   void setOpponent( Teams* pOpponentTeam ){ m_pOpponent = pOpponentTeam; }
   
   bool hasControl() const { return m_hasBall; };
   bool arePlayersHome() ;

   Teams* getOpponent() const { return m_pOpponent; }

private:
   TEAM::id m_myTeam;
   std::vector< FieldPlayers * > m_playersOnTeam;
   GoalKeeper* m_pGoalkeeper;

   Players* m_pPlayerWithBall;
   Players* m_pSupportingPlayer;
   Players* m_pPlayerClosestToBall;
   Players* m_pPlayerReceivingPass;
   Teams* m_pOpponent;

   bool m_hasBall;
   
   StateMachine< Teams >* m_pMyStateMachine;
};

