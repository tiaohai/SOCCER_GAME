// This is the class that handls the plaeyrs and goalkeepers, basically each team.
#pragma once
#include <vector>
#include "StateMachine.h"
#include "Players.h"
#include "State.h"

class FieldPlayers;
class GoalKeeper;
class GoalPosts;
class SupportSpotCalculator;

class Teams
{
public:
   Teams( TEAM::id myTeam, GoalPosts* pMyGoalPost );
   ~Teams( void );
   void update();
   void draw();
   const char* getTeamName() const;
   void sendPlayersHome();
   void sendFieldPlayersHome();
   void sendGoalKeeperHome();
   void changeState( State< Teams >* pToState ) const ;

   void setPlayerWithBall     ( Players* pPlayer );
   void setSupportingPlayer   ( Players* pPlayer ) { m_pSupportingPlayer = pPlayer; }
   void setReceivingPlayer    ( Players* pPlayer ) { m_pPlayerReceivingPass = pPlayer; }
   void setClosestPlayerToBall();

   Players* getPlayerWithBall() const { return m_pPlayerWithBall; }
   Players* getSupportingPlayer() const { return m_pSupportingPlayer; }
   Players* getReceivingPlayer() const { return m_pPlayerReceivingPass; }
   Players* getClosestPlayerToBall() const { return m_pPlayerClosestToBall; }

   std::vector< Players * > getPlayersOnTeam() const ;
   std::vector< FieldPlayers * > getFieldPlayersOnTeam() const { return m_playersOnTeam; }
   GoalPosts* getGoalPost() const { return m_pMyGoalPost; }

   void setOpponent( Teams* pOpponentTeam ){ m_pOpponent = pOpponentTeam; }
   void setHasControl( bool hasControl ){ m_hasBall = hasControl; }
   void requestPass( Players* pPlayerRequesting );
   
   bool hasControl() const { return m_hasBall; };
   bool arePlayersHome() ;
   bool doesGoalKeeperHaveBall() const ;

   bool isPassSafeFromAllOpponent( glm::vec2 from, glm::vec2 to,  const Players* const receiver, float force ) const ;
   bool isPassSafeFromOpponent( glm::vec2 from, glm::vec2 to, const Players* const receiver, Players* const opponent, float force ) const ;
   bool findPass( const Players* const passer, Players*& receiver, glm::vec2& passTarget, float force, float minPassingDistance ) const;
   bool getBestPassToReceiver( const Players* const passer, const Players* const receiver, glm::vec2& passTarget, float force ) const;

   bool canShoot( glm::vec2 ballPos, float force, glm::vec2& shotTarget ) const ;

   Players* determineBestSupportingPlayer();
   Teams* getOpponent() const { return m_pOpponent; }
   TEAM::id getTeamColor() const { return m_myTeam; }
   SupportSpotCalculator* getSupportSpot() const { return m_pSupportSpotCalculator; }
   glm::vec2 getBestSupportSpot();

private:
   TEAM::id m_myTeam;
   std::vector< FieldPlayers * > m_playersOnTeam;
   GoalKeeper* m_pGoalkeeper;

   Players* m_pPlayerWithBall;
   Players* m_pSupportingPlayer;
   Players* m_pPlayerClosestToBall;
   Players* m_pPlayerReceivingPass;
   Teams* m_pOpponent;
   GoalPosts* m_pMyGoalPost;
   SupportSpotCalculator* m_pSupportSpotCalculator;

   bool m_hasBall;
   
   StateMachine< Teams >* m_pMyStateMachine;
};

