#ifndef STEERINGBEHAVIORS_H
#define STEERINGBEHAVIORS_H

#include "Path.h"

class Vehicle;
class BaseGameEntity;

//--------------------------- Constants ----------------------------------

// The radius of the constraining circle for the wander behavior
const double wanderRingRadius = 50;
// Distance the wander circle is projected in front of the agent
const double wanderRingDistance = 500;
// If agent is close enough to current point seek next (double agent radius)
const double wayPointSeekDistance = 50 * 2;
// When to flee radius
const double aproachRadius = 200;
// How far agent can 'see'
const double m_dViewDistance = 150;

//------------------------------------------------------------------------

class SteeringBehavior
{
private:

	enum behavior_type
	{
		none = 0x00000,
		seek = 0x00002,
		flee = 0x00004,
		arrive = 0x00008,
		wander = 0x00010,
		cohesion = 0x00020,
		separation = 0x00040,
		alignment = 0x00080,
		obstacle_avoidance = 0x00100,
		wall_avoidance = 0x00200,
		follow_path = 0x00400,
		pursuit = 0x00800,
		evade = 0x01000,
		interpose = 0x02000,
		hide = 0x04000,
		flock = 0x08000,
		offset_pursuit = 0x10000,
		patrol = 0x20000,
	};

	// A pointer to the owner of this instance
	Vehicle* m_pVehicle;


	// The steering force created by the combined effect of all the selected behaviors
	Vector2D m_vSteeringForce;

	// These can be used to keep track of friends, pursuers, or prey
	Vehicle* m_pTargetAgent1;
	Vehicle* m_pTargetAgent2;

	// The current target
	Vector2D m_vTarget;

	// Explained above
	double m_dWanderRadius;
	double m_dWanderDistance;

	// Multipliers. These can be adjusted to effect strength of the  
	//appropriate behavior. Useful to get flocking the way you require
	//for example.
	double m_dWeightWander;
	double m_dWeightObstacleAvoidance;
	double m_dWeightWallAvoidance;
	double m_dWeightSeek;
	double m_dWeightFlee;
	double m_dWeightArrive;
	double m_dWeightPursuit;
	double m_dWeightOffsetPursuit;
	double m_dWeightInterpose;
	double m_dWeightSeparation;
	double m_dWeightAlignment;
	double m_dWeightCohesion;
	double m_dWeightHide;
	double m_dWeightEvade;
	double m_dWeightFollowPath;
	double m_dWeightPatrol;

	// Pointer to any current path
	Path* m_pPath;

	// The distance (squared) a vehicle has to be from a path waypoint before
	// it starts seeking to the next waypoint
	double m_dWaypointSeekDistSq;

	// For flee
	float m_dApproachRadius;

	// Any offset used for formations or offset pursuit
	Vector2D m_vOffset;

	// Binary flags to indicate whether or not a behavior should be active
	int m_iFlags;

	// Arrive makes use of these to determine how quickly a vehicle
	// should decelerate to its target
	enum Deceleration { slow = 3, normal = 2, fast = 1 };

	// Default
	Deceleration m_Deceleration;

	// This function tests if a specific bit of m_iFlags is set
	bool On(behavior_type bt) { return (m_iFlags & bt) == bt; }

	// Thus function accumulates all forces 
	bool AccumulateForce(Vector2D &sf, Vector2D ForceToAdd);

	bool RenderAidsFlag = false;
	/* .......................................................

					 BEGIN BEHAVIOR DECLARATIONS

	   .......................................................*/

	// This behavior moves the agent towards a target position
	Vector2D Seek(Vector2D TargetPos);

	// This behavior returns a vector that moves the agent away from a target position
	Vector2D Flee(Vector2D TargetPos);

	// This behavior is similar to seek but it attempts to arrive  at the target position with a zero velocity
	Vector2D Arrive(Vector2D TargetPos, Deceleration deceleration);

	// This behavior predicts where an agent will be in time T and seeks
	// towards that point to intercept it.
	Vector2D Pursuit(const Vehicle* agent);

	// This behavior maintains a position, in the direction of offset
	// from the target vehicle
	Vector2D OffsetPursuit(const Vehicle* agent, const Vector2D offset);

	// This behavior attempts to evade a pursuer
	Vector2D Evade(const Vehicle* agent);

	// This behavior makes the agent wander about randomly
	Vector2D Wander();

	// This returns a steering force which will attempt to keep the agent 
	// away from any obstacles it may encounter
	Vector2D ObstacleAvoidance(const std::vector<BaseGameEntity*>& obstacles);

	// Does a projected line intersect given circle
	bool LineIntersectsCircle(Vector2D* ahead, Vector2D* ahead2, BaseGameEntity* obstacle);

	Vector2D WallAvoidance();

	// Given a series of Vector2Ds, this method produces a force that will
	// move the agent along the waypoints in order
	Vector2D FollowPath();

	// Same as FollowPath but with no loop just backtrack
	Vector2D Patrol();

	// This results in a steering force that attempts to steer the vehicle
	// to the center of the vector connecting two moving agents.
	Vector2D Interpose(const Vehicle* VehicleA, const Vehicle* VehicleB);

	// Given another agent position to hide from and a list of BaseGameEntitys this
	// method attempts to put an obstacle between itself and its opponent
	Vector2D Hide(const Vehicle* hunter, const std::vector<BaseGameEntity*>& obstacles);

	// Group behaviors
	Vector2D Separation(const std::vector<Vehicle*> &neighbours);
	Vector2D Alignment(const std::vector<Vehicle*> &neighbours);
	Vector2D Cohesion(const std::vector<Vehicle*> &neighbours);

	/* .......................................................

					   END BEHAVIOR DECLARATIONS

	  .......................................................*/

	// Calculates and sums the steering forces from any active behaviors by their priority
	Vector2D CalculatePrioritized();

	// Helper method for Hide. Returns a position located on the other
	// side of an obstacle to the pursuer
	Vector2D GetHidingPosition(const Vector2D& posOb,const double radiusOb, const Vector2D& posHunter);

public:

	SteeringBehavior(Vehicle* agent);

	virtual ~SteeringBehavior();

	// Calculates and sums the steering forces from any active behaviors
	Vector2D Calculate();

	// Calculates the component of the steering force that is parallel with the vehicle heading
	double ForwardComponent();

	// Calculates the component of the steering force that is perpendicuar with the vehicle heading
	double SideComponent();

	// Renders visual aids and info for seeing how each behavior is calculated
	void RenderAids();

	void SetTarget(const Vector2D t) { m_vTarget = t; }
	Vector2D GetTarget()const { return m_vTarget; }

	void SetTargetAgent1(Vehicle* Agent) { m_pTargetAgent1 = Agent; }
	void SetTargetAgent2(Vehicle* Agent) { m_pTargetAgent2 = Agent; }

	Vehicle* GetTargetAgent1()const { return m_pTargetAgent1; }

	void SetOffset(const Vector2D offset) { m_vOffset = offset; }
	Vector2D GetOffset()const { return m_vOffset; }

	void SetPath(std::list<Vector2D> new_path) { m_pPath->Set(new_path); }
	void SetPath(const Path& new_path) { m_pPath->Set(new_path); }

	Path* GetPath()const { return m_pPath; }

	Vector2D Force()const { return m_vSteeringForce; }

	void FleeOn() { m_iFlags |= flee; }
	void SeekOn() { m_iFlags |= seek; }
	void ArriveOn() { m_iFlags |= arrive; }
	void WanderOn() { m_iFlags |= wander; }
	void PursuitOn(Vehicle* v) { m_iFlags |= pursuit; m_pTargetAgent1 = v; }
	void EvadeOn(Vehicle* v) { m_iFlags |= evade; m_pTargetAgent1 = v; }
	void CohesionOn() { m_iFlags |= cohesion; }
	void SeparationOn() { m_iFlags |= separation; }
	void AlignmentOn() { m_iFlags |= alignment; }
	void ObstacleAvoidanceOn() { m_iFlags |= obstacle_avoidance; }
	void WallAvoidanceOn() { m_iFlags |= wall_avoidance; }
	void FollowPathOn() { m_iFlags |= follow_path; }
	void PatrolOn() { m_iFlags |= patrol; }
	void InterposeOn(Vehicle* v1, Vehicle* v2) { m_iFlags |= interpose; m_pTargetAgent1 = v1; m_pTargetAgent2 = v2; }
	void HideOn(Vehicle* v) { m_iFlags |= hide; m_pTargetAgent1 = v; }
	void OffsetPursuitOn(Vehicle* v1, const Vector2D offset) { m_iFlags |= offset_pursuit; m_vOffset = offset; m_pTargetAgent1 = v1; }
	void FlockingOn() { CohesionOn(); AlignmentOn(); SeparationOn(); WanderOn(); }

	void FleeOff() { if (On(flee)) m_iFlags ^= flee; }
	void SeekOff() { if (On(seek)) m_iFlags ^= seek; }
	void ArriveOff() { if (On(arrive)) m_iFlags ^= arrive; }
	void WanderOff() { if (On(wander)) m_iFlags ^= wander; }
	void PursuitOff() { if (On(pursuit)) m_iFlags ^= pursuit; }
	void EvadeOff() { if (On(evade)) m_iFlags ^= evade; }
	void CohesionOff() { if (On(cohesion)) m_iFlags ^= cohesion; }
	void SeparationOff() { if (On(separation)) m_iFlags ^= separation; }
	void AlignmentOff() { if (On(alignment)) m_iFlags ^= alignment; }
	void ObstacleAvoidanceOff() { if (On(obstacle_avoidance)) m_iFlags ^= obstacle_avoidance; }
	void WallAvoidanceOff() { if (On(wall_avoidance)) m_iFlags ^= wall_avoidance; }
	void FollowPathOff() { if (On(follow_path)) m_iFlags ^= follow_path; }
	void PatrolOff() { if (On(patrol)) m_iFlags ^= patrol; }
	void InterposeOff() { if (On(interpose)) m_iFlags ^= interpose; }
	void HideOff() { if (On(hide)) m_iFlags ^= hide; }
	void OffsetPursuitOff() { if (On(offset_pursuit)) m_iFlags ^= offset_pursuit; }
	void FlockingOff() { CohesionOff(); AlignmentOff(); SeparationOff(); WanderOff(); }

	bool isFleeOn() { return On(flee); }
	bool isSeekOn() { return On(seek); }
	bool isArriveOn() { return On(arrive); }
	bool isWanderOn() { return On(wander); }
	bool isPursuitOn() { return On(pursuit); }
	bool isEvadeOn() { return On(evade); }
	bool isCohesionOn() { return On(cohesion); }
	bool isSeparationOn() { return On(separation); }
	bool isAlignmentOn() { return On(alignment); }
	bool isObstacleAvoidanceOn() { return On(obstacle_avoidance); }
	bool isWallAvoidanceOn() { return On(wall_avoidance); }
	bool isFollowPathOn() { return On(follow_path); }
	bool isPatrolOn() { return On(patrol); }
	bool isInterposeOn() { return On(interpose); }
	bool isHideOn() { return On(hide); }
	bool isOffsetPursuitOn() { return On(offset_pursuit); }

	double GetWanderDistance()const { return m_dWanderDistance; }
	double GetWanderRadius()const { return m_dWanderRadius; }

	void RenderAidsOn() { RenderAidsFlag = true; }
	void RenderAidsOff() { RenderAidsFlag = false; }
};

#endif