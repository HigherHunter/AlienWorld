#ifndef BASE_GAME_ENTITY_H
#define BASE_GAME_ENTITY_H

#include "CollisionBox.h"

class BaseGameEntity
{
public:

	enum { default_entity_type = -1 };

private:

	// Unique ID for each entity
	int m_ID;

	// Esvery entity has a type associated with it (health, troll, ammo etc)
	int m_EntityType;

	// Used by the constructor to give each entity a unique ID
	int NextValidID() { static int NextID = 0; return NextID++; }

protected:

	// Its location in the environment
	Vector2D m_vPos;

	// Its scale
	Vector2D m_vScale;

	// The length of this object's bounding radius
	double m_dBoundingRadius;

	// This entity collision box
	CollisionBox box;

	// State of collider (on or off)
	bool colliderState = true;

	void ColliderOn() { colliderState = true; }
	void ColliderOff() { colliderState = false; }

	BaseGameEntity() :m_ID(NextValidID()),
		m_dBoundingRadius(0.0),
		m_vPos(Vector2D()),
		m_vScale(Vector2D(1.0, 1.0)),
		m_EntityType(default_entity_type)
	{}

	BaseGameEntity(int entity_type) :m_ID(NextValidID()),
		m_dBoundingRadius(0.0),
		m_vPos(Vector2D()),
		m_vScale(Vector2D(1.0, 1.0)),
		m_EntityType(entity_type)
	{}

	BaseGameEntity(int entity_type, Vector2D pos, double r) :m_vPos(pos),
		m_dBoundingRadius(r),
		m_ID(NextValidID()),
		m_vScale(Vector2D(1.0, 1.0)),
		m_EntityType(entity_type)
	{}

	//this can be used to create an entity with a 'forced' ID. It can be used
	//when a previously created entity has been removed and deleted from the
	//game for some reason. For example, The Raven map editor uses this ctor 
	//in its undo/redo operations. 
	//USE WITH CAUTION!
	BaseGameEntity(int entity_type, int ForcedID) :m_ID(ForcedID),
		m_dBoundingRadius(0.0),
		m_vPos(Vector2D()),
		m_vScale(Vector2D(1.0, 1.0)),
		m_EntityType(entity_type)
	{}

public:

	virtual ~BaseGameEntity() {}

	virtual void Update() {};

	virtual void Render() {};

	// Entities should be able to read/write their data to a stream
	virtual void Write(std::ostream&  os)const {}
	virtual void Read(std::ifstream& is) {}

	Vector2D GetPos()const { return m_vPos; }
	void SetPos(Vector2D new_pos) { m_vPos = new_pos; }

	double GetBRadius()const { return m_dBoundingRadius; }
	void SetBRadius(double r) { m_dBoundingRadius = r; }

	int GetID()const { return m_ID; }

	Vector2D GetScale()const { return m_vScale; }
	void SetScale(Vector2D val) { m_dBoundingRadius *= MaxOf(val.x, val.y) / MaxOf(m_vScale.x, m_vScale.y); m_vScale = val; }
	void SetScale(double val) { m_dBoundingRadius *= (val / MaxOf(m_vScale.x, m_vScale.y)); m_vScale = Vector2D(val, val); }

	int GetEntityType()const { return m_EntityType; }
	void SetEntityType(int new_type) { m_EntityType = new_type; }

	CollisionBox GetCollisionBox()const { return box; }

	bool IsColliderActive()const { return colliderState; };
};

#endif // BASE_GAME_ENTITY_H
