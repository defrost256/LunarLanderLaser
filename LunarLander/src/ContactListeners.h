#pragma once
#include "ofxBox2d.h"

enum ContactFilterFlags { FilterBody = 0x01 };
enum ContactCallbackFlag { BeginContact = 0x01, EndContact = 0x02, PreSolve = 0x04, PostSolve = 0x08 };

class LunarLanderContactListener {

public:

	uint32 ContactFilterFlag;

	b2Body* filterBody;

	void SetBodyFilter(b2Body* body);
	void ResetBodyFilter();

	void ResetFilters();

	virtual void BeginContact(b2Contact* contact) = 0;

	virtual void EndContact(b2Contact* contact) = 0;

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) = 0;

	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) = 0;
};

class LunarLanderConatactManager : public b2ContactListener
{

	static LunarLanderConatactManager* instance;

	std::map<LunarLanderContactListener*,int> callbacks;

public:

	static LunarLanderConatactManager* Get();

	void SetCallback(LunarLanderContactListener* listener, int callbackFlags);

	void AddCallback(LunarLanderContactListener* listener, int callbackFlags);

	void RemoveCallback(LunarLanderContactListener* listener);

	void ShrinkCallback(LunarLanderContactListener* listener, int callbackFlagsToRemove);

	virtual void BeginContact(b2Contact* contact) override;

	virtual void EndContact(b2Contact* contact) override;

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:

	bool isFiltered(LunarLanderContactListener* listener, b2Contact* contact);
};

class DebugContactListener : public LunarLanderContactListener
{
	// Inherited via LunarLanderContactListener
	virtual void BeginContact(b2Contact * contact) override;
	virtual void EndContact(b2Contact * contact) override;
	virtual void PreSolve(b2Contact * contact, const b2Manifold * oldManifold) override;
	virtual void PostSolve(b2Contact * contact, const b2ContactImpulse * impulse) override;
};