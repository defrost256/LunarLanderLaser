#include "ContactListeners.h"

LunarLanderConatactManager* LunarLanderConatactManager::instance = nullptr;

LunarLanderConatactManager* LunarLanderConatactManager::Get()
{
	if (!instance)
	{
		instance = new LunarLanderConatactManager();
	}
	return instance;
}

void LunarLanderConatactManager::SetCallback(LunarLanderContactListener * listener, int callbackFlags)
{
	callbacks[listener] = callbackFlags;
}

void LunarLanderConatactManager::AddCallback(LunarLanderContactListener * listener, int callbackFlags)
{
	callbacks[listener] |= callbackFlags;
}

void LunarLanderConatactManager::RemoveCallback(LunarLanderContactListener * listener)
{
	callbacks[listener] = 0;
}

void LunarLanderConatactManager::ShrinkCallback(LunarLanderContactListener * listener, int callbackFlagsToRemove)
{
	callbacks[listener] &= ~callbackFlagsToRemove;
}

void LunarLanderConatactManager::BeginContact(b2Contact * contact)
{
	for (auto listener : callbacks)
	{
		if (listener.second & ContactCallbackFlag::BeginContact && isFiltered(listener.first, contact))
		{
			listener.first->BeginContact(contact);
		}
	}
}

void LunarLanderConatactManager::EndContact(b2Contact * contact)
{
	for (auto listener : callbacks)
	{
		if (listener.second & ContactCallbackFlag::EndContact && isFiltered(listener.first, contact))
		{
			listener.first->EndContact(contact);
		}
	}
}

void LunarLanderConatactManager::PreSolve(b2Contact * contact, const b2Manifold * oldManifold)
{
	for (auto listener : callbacks)
	{
		if (listener.second & ContactCallbackFlag::PreSolve && isFiltered(listener.first, contact))
		{
			listener.first->PreSolve(contact, oldManifold);
		}
	}
}

void LunarLanderConatactManager::PostSolve(b2Contact * contact, const b2ContactImpulse * impulse)
{
	for (auto listener : callbacks)
	{
		if (listener.second & ContactCallbackFlag::PostSolve && isFiltered(listener.first, contact))
		{
			listener.first->PostSolve(contact, impulse);
		}
	}
}

bool LunarLanderConatactManager::isFiltered(LunarLanderContactListener * listener, b2Contact * contact)
{
	uint32 filterFlag = listener->ContactFilterFlag;
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	if (filterFlag & ContactFilterFlags::FilterBody)
		return fixtureA->GetBody() == listener->filterBody || fixtureB->GetBody() == listener->filterBody;
	if (filterFlag & ContactFilterFlags::FilterFixture)
		return fixtureA == listener->filterFixture || fixtureB == listener->filterFixture;
	return true;
}

void DebugContactListener::BeginContact(b2Contact * contact)
{
	ofLogNotice("ContactListener") << "BeginContact";
}

void DebugContactListener::EndContact(b2Contact * contact)
{
	ofLogNotice("ContactListener") << "EndContact";
}

void DebugContactListener::PreSolve(b2Contact * contact, const b2Manifold * oldManifold)
{
	ofLogNotice("ContactListener") << "PreSolve";
}

void DebugContactListener::PostSolve(b2Contact * contact, const b2ContactImpulse * impulse)
{
	ofLogNotice("ContactListener") << "PostSolve";
}

LandingSpotContactListener::LandingSpotContactListener(ofApp* _app, b2Body* _lander)
{
	app = _app;
	lander = _lander;
}

void LandingSpotContactListener::BeginContact(b2Contact* contact)
{
	if(contact->GetFixtureA()->GetBody() == lander || contact->GetFixtureB()->GetBody() == lander)
		app->StartLanding();
}

void LandingSpotContactListener::EndContact(b2Contact* contact)
{
	if(contact->GetFixtureA()->GetBody() == lander || contact->GetFixtureB()->GetBody() == lander)
		app->EndLanding();
}

void LandingSpotContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold){}
void LandingSpotContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){}

LanderCrashContactListener::LanderCrashContactListener(Lander* lander)
{
	this->lander = lander;
}

void LanderCrashContactListener::BeginContact(b2Contact* contact){}
void LanderCrashContactListener::EndContact(b2Contact* contact){}
void LanderCrashContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold){}

void LanderCrashContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	ofLogNotice("Crash") << impulse->normalImpulses;
}

void LunarLanderContactListener::SetBodyFilter(b2Body * body)
{
	filterBody = body;
	ContactFilterFlag |= ContactFilterFlags::FilterBody;
}

void LunarLanderContactListener::ResetBodyFilter()
{
	ContactFilterFlag &= ~ContactFilterFlags::FilterBody;
}

void LunarLanderContactListener::SetFixtureFilter(b2Fixture* fixture)
{
	filterFixture = fixture;
	ContactFilterFlag |= ContactFilterFlags::FilterFixture;
}

void LunarLanderContactListener::ResetFixtureFilter()
{
	ContactFilterFlag &= ~ContactFilterFlags::FilterFixture;
}

void LunarLanderContactListener::ResetFilters()
{
	ContactFilterFlag = 0;
}
