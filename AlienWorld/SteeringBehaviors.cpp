#include "SteeringBehaviors.h"
#include "Vehicle.h"
#include "OpenGl.h"

Vector2D visualAidVector;

//------------------------- ctor -----------------------------------------
//
//------------------------------------------------------------------------
SteeringBehavior::SteeringBehavior(Vehicle* agent) :


	m_pVehicle(agent),
	m_iFlags(0),
	m_dWeightObstacleAvoidance(1.0),
	m_dWeightWallAvoidance(5.0),
	m_dWeightWander(1.0),
	m_Deceleration(normal),
	m_pTargetAgent1(NULL),
	m_pTargetAgent2(NULL),
	m_dApproachRadius(aproachRadius),
	m_dWaypointSeekDistSq(wayPointSeekDistance),
	m_dWanderDistance(wanderRingDistance),
	m_dWanderRadius(wanderRingRadius),
	m_dWeightPatrol(0.05),
	m_dWeightSeek(1.0),
	m_dWeightFlee(1.0),
	m_dWeightArrive(1.0),
	m_dWeightPursuit(1.0),
	m_dWeightOffsetPursuit(0.5),
	m_dWeightInterpose(1.0),
	m_dWeightSeparation(50.0),
	m_dWeightAlignment(1.0),
	m_dWeightCohesion(1.0),
	m_dWeightHide(1.0),
	m_dWeightEvade(0.01),
	m_dWeightFollowPath(0.05)

{
	//create a Path
	m_pPath = new Path();
	m_pPath->LoopOn();
}

//---------------------------------dtor ----------------------------------
SteeringBehavior::~SteeringBehavior() { delete m_pPath; }


///////////////////////////////////////////////////////////////////////////// CALCULATE METHODS 


//----------------------- Calculate --------------------------------------
//
//  calculates the accumulated steering force according to the method set
//  in m_SummingMethod
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Calculate()
{
	if (RenderAidsFlag)
		RenderAids();
	//reset the steering force
	m_vSteeringForce.Zero();

	if (On(separation) || On(alignment) || On(cohesion))
	{
		m_pVehicle->GetWorld()->TagVehiclesWithinViewrange(m_pVehicle, m_dViewDistance);
	}

	m_vSteeringForce = CalculatePrioritized();

	return m_vSteeringForce;
}

//------------------------- ForwardComponent -----------------------------
//
//  returns the forward oomponent of the steering force
//------------------------------------------------------------------------
double SteeringBehavior::ForwardComponent()
{
	return m_pVehicle->GetHeading().Dot(m_vSteeringForce);
}

//--------------------------- SideComponent ------------------------------
//  returns the side component of the steering force
//------------------------------------------------------------------------
double SteeringBehavior::SideComponent()
{
	return m_pVehicle->GetSide().Dot(m_vSteeringForce);
}


//--------------------- AccumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool SteeringBehavior::AccumulateForce(Vector2D &RunningTot, Vector2D ForceToAdd)
{
	//calculate how much steering force the vehicle has used so far
	double MagnitudeSoFar = RunningTot.Length();
	//calculate how much steering force remains to be used by this vehicle
	double MagnitudeRemaining = m_pVehicle->GetMaxForce() - MagnitudeSoFar;
	//return false if there is no more force left to use
	if (MagnitudeRemaining <= 0.0) return false;

	//calculate the magnitude of the force we want to add
	double MagnitudeToAdd = ForceToAdd.Length();

	//if the magnitude of the sum of ForceToAdd and the running total
	//does not exceed the maximum force available to this vehicle, just
	//add together. Otherwise add as much of the ForceToAdd vector is
	//possible without going over the max.
	if (MagnitudeToAdd < MagnitudeRemaining)
	{
		RunningTot += ForceToAdd;
	}

	else
	{
		//add it to the steering force
		RunningTot += (Vec2DNormalize(ForceToAdd) * MagnitudeRemaining);
	}

	return true;
}


//---------------------- CalculatePrioritized ----------------------------
//
//  this method calls each active steering behavior in order of priority
//  and acumulates their forces until the max steering force magnitude
//  is reached, at which time the function returns the steering force 
//  accumulated to that  point
//------------------------------------------------------------------------
Vector2D SteeringBehavior::CalculatePrioritized()
{
	Vector2D force;

	if (On(obstacle_avoidance))
	{
		force = ObstacleAvoidance(m_pVehicle->GetWorld()->GetObstacles()) *
			m_dWeightObstacleAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(wall_avoidance))
	{
		force = WallAvoidance() * m_dWeightWallAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(evade))
	{
		assert(m_pTargetAgent1 && "Evade target not assigned");

		force = Evade(m_pTargetAgent1) * m_dWeightEvade;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(flee))
	{
		force = Flee(m_vTarget) * m_dWeightFlee;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(separation))
	{
		force = Separation(m_pVehicle->GetWorld()->GetNeighbours()) * m_dWeightSeparation;

		if (!AccumulateForce(m_vSteeringForce, force))
		{
			return m_vSteeringForce;
		}
	}

	if (On(alignment))
	{
		force = Alignment(m_pVehicle->GetWorld()->GetNeighbours()) * m_dWeightAlignment;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(cohesion))
	{
		force = Cohesion(m_pVehicle->GetWorld()->GetNeighbours()) * m_dWeightCohesion;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(seek))
	{
		force = Seek(m_vTarget) * m_dWeightSeek;

		if (!AccumulateForce(m_vSteeringForce, force))
		{
			return m_vSteeringForce;
		}
	}

	if (On(arrive))
	{
		force = Arrive(m_vTarget, m_Deceleration) * m_dWeightArrive;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(wander))
	{
		force = Wander() * m_dWeightWander;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(pursuit))
	{
		assert(m_pTargetAgent1 && "pursuit target not assigned");

		force = Pursuit(m_pTargetAgent1) * m_dWeightPursuit;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(offset_pursuit))
	{
		assert(m_pTargetAgent1 && "pursuit target not assigned");
		assert(!m_vOffset.isZero() && "No offset assigned");

		force = OffsetPursuit(m_pTargetAgent1, m_vOffset);

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(interpose))
	{
		assert(m_pTargetAgent1 && m_pTargetAgent2 && "Interpose agents not assigned");

		force = Interpose(m_pTargetAgent1, m_pTargetAgent2) * m_dWeightInterpose;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(hide))
	{
		assert(m_pTargetAgent1 && "Hide target not assigned");

		force = Hide(m_pTargetAgent1, m_pVehicle->GetWorld()->GetObstacles()) * m_dWeightHide;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}


	if (On(follow_path))
	{
		force = FollowPath() * m_dWeightFollowPath;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(patrol))
	{
		force = Patrol() * m_dWeightPatrol;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	return m_vSteeringForce;
}


//////////////////////////////////////////////////////////////////// START OF BEHAVIORS

//------------------------------- Seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  direct the agent towards the target
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Seek(Vector2D targetPos)
{
	Vector2D desiredVelocity = Vec2DNormalize(targetPos - m_pVehicle->GetPos())
		* m_pVehicle->GetMaxSpeed();

	visualAidVector = desiredVelocity;
	return (desiredVelocity - m_pVehicle->GetVelocity());
}

//----------------------------- Flee -------------------------------------
//
//  Does the opposite of Seek
//------------------------------------------------------------------------5
Vector2D SteeringBehavior::Flee(Vector2D targetPos)
{
	//only flee if the target is within 'panic distance'.
	if (Vec2DDistanceSq(m_pVehicle->GetPos(), targetPos) > m_dApproachRadius * m_dApproachRadius)
	{
		return Vector2D(0, 0);
	}

	Vector2D desiredVelocity = Vec2DNormalize(m_pVehicle->GetPos() - targetPos) * m_pVehicle->GetMaxSpeed();
	return (desiredVelocity - m_pVehicle->GetVelocity());
}

//--------------------------- Arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Arrive(Vector2D targetPos, Deceleration deceleration)
{
	Vector2D toTarget = targetPos - m_pVehicle->GetPos();
	visualAidVector = toTarget;
	//calculate the distance to the target
	double dist = toTarget.Length();

	if (dist > 0)
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const double decelerationTweaker = 5.3;

		//calculate the speed required to reach the target given the desired
		//deceleration
		double speed = dist / ((double)deceleration * decelerationTweaker);

		//make sure the velocity does not exceed the max
		speed = min(speed, m_pVehicle->GetMaxSpeed());

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		Vector2D desiredVelocity = toTarget * speed / dist;

		return (desiredVelocity - m_pVehicle->GetVelocity());
	}

	return Vector2D(0, 0);
}

//------------------------------ Pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the 
//  evader
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Pursuit(const Vehicle* evader)
{
	//if the evader is ahead and facing the agent then we can just seek
	//for the evader's current position.
	Vector2D toEvader = evader->GetPos() - m_pVehicle->GetPos();

	double relativeHeading = m_pVehicle->GetHeading().Dot(evader->GetHeading());

	if ((toEvader.Dot(m_pVehicle->GetHeading()) > 0) &&
		(relativeHeading < -0.95))  //acos(0.95)=18 degs
	{
		return Seek(evader->GetPos());
	}

	//Not considered ahead so we predict where the evader will be.

	//the lookahead time is propotional to the distance between the evader
	//and the pursuer; and is inversely proportional to the sum of the
	//agent's velocities
	double lookAheadTime = toEvader.Length() /
		(m_pVehicle->GetMaxSpeed() + evader->GetSpeed());

	//now seek to the predicted future position of the evader
	return Seek(evader->GetPos() + evader->GetVelocity() * lookAheadTime);
}


//----------------------------- Evade ------------------------------------
//
//  similar to pursuit except the agent Flees from the estimated future
//  position of the pursuer
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Evade(const Vehicle* pursuer)
{
	/* Not necessary to include the check for facing direction this time */

	Vector2D toPursuer = pursuer->GetPos() - m_pVehicle->GetPos();

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	double lookAheadTime = toPursuer.Length() /
		(m_pVehicle->GetMaxSpeed() + pursuer->GetSpeed());

	//now flee away from predicted future position of the pursuer
	return Flee(pursuer->GetPos() + pursuer->GetVelocity() * lookAheadTime);
}


//--------------------------- Wander -------------------------------------
//
//  This behavior makes the agent wander about randomly
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Wander()
{
	Vector2D circle_pos = m_pVehicle->GetPos() +
		Vec2DNormalize(m_pVehicle->GetVelocity()) * wanderRingDistance;
	Vector2D displacement = Vector2D(wanderRingRadius, 0);

	//and steer towards it
	return Seek(circle_pos + Vec2DRotate(displacement, RandInt(0, 360)));
}


//---------------------- ObstacleAvoidance -------------------------------
//
//  Given a vector of CObstacles, this method returns a steering force
//  that will prevent the agent colliding with the closest obstacle
//------------------------------------------------------------------------
Vector2D SteeringBehavior::ObstacleAvoidance(const std::vector<BaseGameEntity*>& obstacles)
{
	Vector2D avoidance(0, 0);
	//this will keep track of the closest intersecting obstacle (CIB)
	BaseGameEntity* closestIntersectingObstacle = NULL;

	double dynamic_length = Vec2DLength(m_pVehicle->GetVelocity())
		/ m_pVehicle->GetMaxSpeed() + m_pVehicle->GetBRadius();

	Vector2D ahead = m_pVehicle->GetPos()
		+ Vec2DNormalize(m_pVehicle->GetVelocity()) * dynamic_length;
	Vector2D ahead2 = m_pVehicle->GetPos()
		+ Vec2DNormalize(m_pVehicle->GetVelocity()) * dynamic_length * 0.5;

	std::vector<BaseGameEntity*>::const_iterator curOb = obstacles.begin();

	bool collision;

	while (curOb != obstacles.end())
	{
		collision = LineIntersectsCircle(&ahead, &ahead2, *curOb);

		if (collision && (closestIntersectingObstacle == NULL ||
			Vec2DDistance(m_pVehicle->GetPos(), (*curOb)->GetPos()) <
			Vec2DDistance(m_pVehicle->GetPos(), closestIntersectingObstacle->GetPos())))
		{
			closestIntersectingObstacle = *curOb;
		}

		++curOb;
	}

	if (closestIntersectingObstacle)
	{
		m_pVehicle->probableJitter = true;
		double x = ahead.x - closestIntersectingObstacle->GetPos().x;
		double y = ahead.y - closestIntersectingObstacle->GetPos().y;
		avoidance = Vector2D(x * 2, y * 2);
		return avoidance;
	}
	m_pVehicle->probableJitter = false;
	return avoidance;
}

bool SteeringBehavior::LineIntersectsCircle(Vector2D* ahead, Vector2D* ahead2, BaseGameEntity* obstacle)
{
	if ((Vec2DDistance(obstacle->GetPos(), *ahead) <= obstacle->GetBRadius() / 1.2 + m_pVehicle->GetBRadius() / 2) ||
		(Vec2DDistance(obstacle->GetPos(), *ahead2) <= obstacle->GetBRadius() / 1.2 + m_pVehicle->GetBRadius() / 2))
	{
		return true;
	}
	else
		return false;

}

Vector2D SteeringBehavior::WallAvoidance()
{
	if (m_pVehicle->GetPos().x <= m_pVehicle->GetBRadius())
	{
		return Flee(Vector2D(0, m_pVehicle->GetPos().y));
	}
	else if (m_pVehicle->GetPos().x >= MAP_WIDTH - m_pVehicle->GetBRadius())
	{
		return Flee(Vector2D(MAP_WIDTH, m_pVehicle->GetPos().y));
	}
	else if (m_pVehicle->GetPos().y <= m_pVehicle->GetBRadius())
	{
		return Flee(Vector2D(m_pVehicle->GetPos().x, 0));
	}
	else if (m_pVehicle->GetPos().y >= MAP_HEIGHT - m_pVehicle->GetBRadius())
	{
		return Flee(Vector2D(m_pVehicle->GetPos().x, MAP_HEIGHT));
	}
	else
		return Vector2D(0, 0);
}

//--------------------------- Interpose ----------------------------------
//
//  Given two agents, this method returns a force that attempts to 
//  position the vehicle between them
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Interpose(const Vehicle* agentA, const Vehicle* agentB)
{
	//first we need to figure out where the two agents are going to be at 
	//time T in the future. This is approximated by determining the time
	//taken to reach the mid way point at the current time at at max speed.
	Vector2D midPoint = (agentA->GetPos() + agentB->GetPos()) / 2.0;

	double timeToReachMidPoint = Vec2DDistance(m_pVehicle->GetPos(), midPoint) /
		m_pVehicle->GetMaxSpeed();

	//now we have T, we assume that agent A and agent B will continue on a
	//straight trajectory and extrapolate to get their future positions
	Vector2D aPos = agentA->GetPos() + agentA->GetVelocity() * timeToReachMidPoint;
	Vector2D bPos = agentB->GetPos() + agentB->GetVelocity() * timeToReachMidPoint;

	//calculate the mid point of these predicted positions
	midPoint = (aPos + bPos) / 2.0;

	//then steer to Arrive at it
	return Arrive(midPoint, fast);
}

//--------------------------- Hide ---------------------------------------
//
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Hide(const Vehicle* hunter, const std::vector<BaseGameEntity*>& obstacles)
{
	double    DistToClosest = MaxDouble;
	Vector2D BestHidingSpot;

	std::vector<BaseGameEntity*>::const_iterator curOb = obstacles.begin();
	std::vector<BaseGameEntity*>::const_iterator closest;

	while (curOb != obstacles.end())
	{
		//calculate the position of the hiding spot for this obstacle
		Vector2D HidingSpot = GetHidingPosition((*curOb)->GetPos(),
			(*curOb)->GetBRadius(),
			hunter->GetPos());

		//work in distance-squared space to find the closest hiding
		//spot to the agent
		double dist = Vec2DDistanceSq(HidingSpot, m_pVehicle->GetPos());

		if (dist < DistToClosest)
		{
			DistToClosest = dist;

			BestHidingSpot = HidingSpot;

			closest = curOb;
		}

		++curOb;

	}//end while

	//if no suitable obstacles found then Evade the hunter
	if (DistToClosest == MaxFloat)
	{
		return Evade(hunter);
	}

	//else use Arrive on the hiding spot
	return Arrive(BestHidingSpot, fast);
}

//------------------------- GetHidingPosition ----------------------------
//
//  Given the position of a hunter, and the position and radius of
//  an obstacle, this method calculates a position DistanceFromBoundary 
//  away from its bounding radius and directly opposite the hunter
//------------------------------------------------------------------------
Vector2D SteeringBehavior::GetHidingPosition(const Vector2D& posOb, const double radiusOb,
	const Vector2D& posHunter)
{
	//calculate how far away the agent is to be from the chosen obstacle's
	//bounding radius
	const double distanceFromBoundary = 30.0;
	double distAway = radiusOb + distanceFromBoundary;

	//calculate the heading toward the object from the hunter
	Vector2D toOb = Vec2DNormalize(posOb - posHunter);

	//scale it to size and add to the obstacles position to get
	//the hiding spot.
	return (toOb * distAway) + posOb;
}


//------------------------------- FollowPath -----------------------------
//
//  Given a series of Vector2Ds, this method produces a force that will
//  move the agent along the waypoints in order. The agent uses the
// 'Seek' behavior to move to the next waypoint - unless it is the last
//  waypoint, in which case it 'Arrives'
//------------------------------------------------------------------------
Vector2D SteeringBehavior::FollowPath()
{
	if (!m_pPath->IsFinished())
	{
		if (m_pPath->IsLooped())
		{
			// Move to next target if close enough to current target
			if (Vec2DDistanceSq(m_pPath->GetCurrentWaypoint(), m_pVehicle->GetPos()) < m_dWaypointSeekDistSq)
			{
				m_pPath->SetNextWaypoint();
			}
			return Seek(m_pPath->GetCurrentWaypoint());
		}
		else
		{
			if (!m_pPath->IsLastPoint())
			{
				// Move to next target if close enough to current target
				if (Vec2DDistanceSq(m_pPath->GetCurrentWaypoint(), m_pVehicle->GetPos()) < m_dWaypointSeekDistSq)
				{
					m_pPath->SetNextWaypoint();
				}
				return Seek(m_pPath->GetCurrentWaypoint());
			}
			else
			{
				return Arrive(m_pPath->GetCurrentWaypoint(), normal);
			}
		}
	}
	return Vector2D(0, 0);
}

//------------------------------- Patrol -----------------------------
//
//  Given a series of Vector2Ds, this method produces a force that will
//  move the agent along the waypoints in order. The agent uses the
// 'Seek' behavior to move to the next waypoint.
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Patrol()
{
	if (!m_pPath->IsLastPoint())
	{
		// Move to next target if close enough to current target
		if (Vec2DDistanceSq(m_pPath->GetCurrentWaypoint(), m_pVehicle->GetPos()) < m_dWaypointSeekDistSq)
		{
			m_pPath->SetNextWaypoint();
		}
		return Seek(m_pPath->GetCurrentWaypoint());
	}
	else
	{
		//return Arrive(m_pPath->GetCurrentWaypoint(), normal);
		m_pPath->ReversePath();
	}
	return Vector2D(0, 0);
}

//------------------------- Offset Pursuit -------------------------------
//
//  Produces a steering force that keeps a vehicle at a specified offset
//  from a leader vehicle
//------------------------------------------------------------------------
Vector2D SteeringBehavior::OffsetPursuit(const Vehicle* leader, const Vector2D offset)
{
	Vector2D offsetPositon = leader->GetPos() + offset;

	Vector2D toOffset = offsetPositon - m_pVehicle->GetPos();

	//the lookahead time is propotional to the distance between the leader
	//and the pursuer; and is inversely proportional to the sum of both
	//agent's velocities
	double lookAheadTime = toOffset.Length() / (m_pVehicle->GetMaxSpeed() + leader->GetSpeed());

	//now Arrive at the predicted future position of the offset
	return Arrive(offsetPositon + leader->GetVelocity() * lookAheadTime, fast);
}

//---------------------------- Separation --------------------------------
//
// this calculates a force repelling from the other neighbors
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Separation(const std::vector<Vehicle*> &neighbours)
{
	Vector2D SteeringForce;

	for (unsigned int a = 0; a < neighbours.size(); ++a)
	{
		//make sure this agent isn't included in the calculations and that
		//the agent being examined is close enough. ***also make sure it doesn't
		//include the evade target ***
		if ((neighbours[a] != m_pVehicle) && (neighbours[a] != m_pTargetAgent1))
		{
			Vector2D ToAgent = m_pVehicle->GetPos() - neighbours[a]->GetPos();

			//scale the force inversely proportional to the agents distance  
			//from its neighbor.
			SteeringForce += Vec2DNormalize(ToAgent) / ToAgent.Length();
		}
	}

	return SteeringForce;
}

//---------------------------- Alignment  ---------------------------------
//
//  returns a force that attempts to align this agents heading with that
//  of its neighbors
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Alignment(const std::vector<Vehicle*>& neighbors)
{
	//used to record the average heading of the neighbors
	Vector2D AverageHeading;

	//used to count the number of vehicles in the neighborhood
	int    NeighborCount = 0;

	//iterate through all the tagged vehicles and sum their heading vectors  
	for (unsigned int a = 0; a < neighbors.size(); ++a)
	{
		//make sure *this* agent isn't included in the calculations and that
		//the agent being examined  is close enough ***also make sure it doesn't
		//include any evade target ***
		if ((neighbors[a] != m_pVehicle) && (neighbors[a] != m_pTargetAgent1))
		{
			AverageHeading += neighbors[a]->GetHeading();

			++NeighborCount;
		}
	}

	//if the neighborhood contained one or more vehicles, average their
	//heading vectors.
	if (NeighborCount > 0)
	{
		AverageHeading /= (double)NeighborCount;

		AverageHeading -= m_pVehicle->GetHeading();
	}

	return AverageHeading;
}

//-------------------------------- Cohesion ------------------------------
//
//  returns a steering force that attempts to move the agent towards the
//  center of mass of the agents in its immediate area
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Cohesion(const std::vector<Vehicle*> &neighbors)
{
	//first find the center of mass of all the agents
	Vector2D CenterOfMass, SteeringForce;

	int NeighborCount = 0;

	//iterate through the neighbors and sum up all the position vectors
	for (unsigned int a = 0; a < neighbors.size(); ++a)
	{
		//make sure *this* agent isn't included in the calculations and that
		//the agent being examined is close enough ***also make sure it doesn't
		//include the evade target ***
		if ((neighbors[a] != m_pVehicle) && (neighbors[a] != m_pTargetAgent1))
		{
			CenterOfMass += neighbors[a]->GetPos();

			++NeighborCount;
		}
	}

	if (NeighborCount > 0)
	{
		//the center of mass is the average of the sum of positions
		CenterOfMass /= (double)NeighborCount;

		//now seek towards that position
		SteeringForce = Seek(CenterOfMass);
	}

	//the magnitude of cohesion is usually much larger than separation or
	//allignment so it usually helps to normalize it.
	return Vec2DNormalize(SteeringForce);
}

void SteeringBehavior::RenderAids()
{
	drawHollowCircle(m_pVehicle->GetPos().x, m_pVehicle->GetPos().y, 500, 1.0, 0.0, 0.0);
}