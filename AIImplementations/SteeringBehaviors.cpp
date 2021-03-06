#include "SteeringBehaviors.h"
#include <math.h>
#include <iostream>
#include "BaseGameEntity.h"
#include "MovingEntity.h"
#include "SoccerBall.h"
#include <allegro5\allegro_primitives.h>

static const float DECELERATION = 2;
static const float TURN_AROUND_SPEED = 0.5;
static const float SLOW_DOWN_RADIUS = 35.0f;

float sqrmag( glm::vec2 currvector )
{
	return ( ( currvector.x * currvector.x ) + ( currvector.y * currvector.y ) );
}

SteeringBehaviors::SteeringBehaviors( MovingEntity* pOwner )
   : m_pOwner( pOwner )
   , m_pTarget( NULL )
   , m_pInterposeTarget( NULL )
   , m_staticInterposeTarget( glm::vec2() )
   , m_arriveTarget( glm::vec2() )
   , m_steeringBehaviorsFlag( 0 )
   , m_interposePercent( 0.5 )
{
}

void SteeringBehaviors::calculateForce()
{
	glm::vec2 totalVelocity = glm::vec2();
	glm::vec2 totalForce = glm::vec2();
 //  if( isSeekOn() && m_pTarget != NULL )
	//{
 //     totalVelocity += calcSeekForce( m_pTarget->getPosition() );
	//}
	if( isArriveOn() )
	{
		totalVelocity += calcArriveVelocity( m_arriveTarget );
	}
   if( isInterposeOn() )
	{
      if( m_pInterposeTarget != NULL )
      {
         totalVelocity += calcInterposeVelocity( m_pInterposeTarget, m_pTarget );
      }
      else
      {
         totalVelocity += calcInterposeVelocityStatic( m_pTarget, m_staticInterposeTarget );
      }
	}
	if( isPursuitOn() )
	{
		totalVelocity += calcPursuitVelocity( SoccerBall::getSoccerBallInstance() );
	}
   // debug remove
   
  /* al_draw_line( m_pOwner->getPosition().x, 
                 m_pOwner->getPosition().y, 
                 m_pOwner->getPosition().x + totalVelocity.x * 20, 
                 m_pOwner->getPosition().y + totalVelocity.y * 20, 
                 al_map_rgb( 122.0f, 122.0f, 0.0f ), 5.0f ); */ // DRAWS line to indicate player velocity
   m_pOwner->setVelocity( totalVelocity );
   //m_pOwner->setForce( totalForce );
}

glm::vec2 SteeringBehaviors::calcArriveVelocity( glm::vec2 target ) 
{
   //al_draw_circle( target.x, target.y, 10.0f, al_map_rgb( 0.0f, 0.0f, 255.0f ), 3.0f ); // DRAWS players arrive target
   glm::vec2 velocityToTarget = /*m_pTarget->getPosition()*/target - m_pOwner->getPosition();
   float distanceToTarget = sqrt( sqrmag( velocityToTarget ) );
   float speed = 0.0f;
   
   if( distanceToTarget > SLOW_DOWN_RADIUS )
   {
      speed = m_pOwner->getMaxSpeed();
   }
   else
   {
      speed = m_pOwner->getMaxSpeed() * distanceToTarget/ ( SLOW_DOWN_RADIUS ); 
   }
   return( glm::normalize( velocityToTarget ) * speed );
}

glm::vec2 SteeringBehaviors::calcSeekForce( glm::vec2 target )// this should be set as a force. Why? 
{
   glm::vec2 velocityToTarget = target - m_pOwner->getPosition();
   glm::vec2 desiredVelocity = m_pOwner->getMaxSpeed() * glm::normalize( velocityToTarget ) ;
   return ( desiredVelocity - m_pOwner->getVelocity() );
}

glm::vec2 SteeringBehaviors::calcInterposeVelocity( MovingEntity* pMovingTargetA, MovingEntity* pMovingTargetB )
{
   glm::vec2 midPoint = ( pMovingTargetA->getPosition() * m_interposePercent + pMovingTargetB->getPosition() * ( 1 - m_interposePercent ) );
   float timeToReachMidPoint = sqrtf( sqrmag( midPoint - m_pOwner->getPosition() ) ) / m_pOwner->getMaxSpeed();

   // TODO FOLLOWING CODE IS ERRONOUS becasue its not velocity, but the position that we are multiplying. 
   glm::vec2 futureTargetAPos = pMovingTargetA->getPosition() + ( pMovingTargetA->getVelocity() * timeToReachMidPoint ) ;
   glm::vec2 futureTargetBPos = pMovingTargetB->getPosition() + ( pMovingTargetB->getVelocity() * timeToReachMidPoint ) ;
   glm::vec2 futureMidPoint = ( futureTargetAPos * m_interposePercent + futureTargetBPos * ( 1 - m_interposePercent ) );

   return calcArriveVelocity( futureMidPoint );
}

glm::vec2 SteeringBehaviors::calcInterposeVelocityStatic( MovingEntity* pMovingTargetA, glm::vec2 staticTargetB )
{
   glm::vec2 midPoint = ( pMovingTargetA->getPosition() * m_interposePercent + staticTargetB * ( 1 - m_interposePercent ) );
   float timeToReachMidPoint = sqrtf( sqrmag( midPoint - m_pOwner->getPosition() ) ) / m_pOwner->getMaxSpeed();

   glm::vec2 futureTargetAPos = pMovingTargetA->getPosition() + ( pMovingTargetA->getVelocity() * timeToReachMidPoint ) ;
   glm::vec2 futureMidPoint = ( futureTargetAPos * m_interposePercent + staticTargetB * ( 1 - m_interposePercent ) );

   return calcArriveVelocity( futureMidPoint );
}

glm::vec2 SteeringBehaviors::calcPursuitVelocity( MovingEntity* pMovingTarget )
{
   glm::vec2 toTarget =  pMovingTarget->getPosition() - m_pOwner->getPosition();

   float relativeHeading = glm::dot( m_pOwner->getHeading(), pMovingTarget->getHeading() );
   if( ( glm::dot( m_pOwner->getHeading(), toTarget ) > 0 ) // this ensures that the vehical is in the same direction as the vector from owner to target
              && relativeHeading < -0.95 ) // this ensures that the owner and target are facing the opposite direction. 
   {
      return calcArriveVelocity( pMovingTarget->getPosition() );
   }

   float turnAroundTime = calcTurnAroundTime( pMovingTarget ) + ( sqrt( sqrmag( toTarget ) ) / ( m_pOwner->getMaxSpeed() + pMovingTarget->getCurrentSpeed() ) );

   return calcArriveVelocity( ( pMovingTarget->getPosition() ) + ( pMovingTarget->getVelocity() /** static_cast< float >( turnAroundTime )*/ ) ); // seeking a point which is at currentPosition + ( time*velocity )
}

float SteeringBehaviors::calcTurnAroundTime( MovingEntity* pMovingTarget )
{
   glm::vec2 toTarget = glm::normalize( pMovingTarget->getPosition() - m_pOwner->getPosition() );
   float dotPro = glm::dot( toTarget, m_pOwner->getHeading() );

   return( ( dotPro - 1 ) * TURN_AROUND_SPEED * -1 );
}

glm::vec2 SteeringBehaviors::calcSeparationForce()
{
   return glm::vec2();
}

bool SteeringBehaviors::isArriveOn()
{
   int temp = m_steeringBehaviorsFlag & FLAG_ARRIVE;
   if( temp != 0 )
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool SteeringBehaviors::isInterposeOn()
{
   int temp = m_steeringBehaviorsFlag & FLAG_INTERPOSE;
   if( temp != 0 )
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool SteeringBehaviors::isSeekOn()
{
   int temp = m_steeringBehaviorsFlag & FLAG_SEEK;
   if( temp != 0 )
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool SteeringBehaviors::isPursuitOn()
{
   int temp = m_steeringBehaviorsFlag & FLAG_PURSUIT;
   if( temp != 0 )
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool SteeringBehaviors::isSeparationOn()
{
   int temp = m_steeringBehaviorsFlag & FLAG_SEPARATION;
   if( temp != 0 )
   {
      return true;
   }
   else
   {
      return false;
   }
}