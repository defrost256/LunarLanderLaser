#include "Lander.h"

Lander::Lander(ofxBox2d* world, LanderParams params, ofVec2f topBoxSize, ofVec2f bottomBoxSize, std::string svgFileName)
{
	this->params = params;
	this->topBoxSize = topBoxSize;
	this->bottomBoxSize = bottomBoxSize;
	this->landerSvg = svgFileName;


	//Create svg File
	ofxSVG svgHandler;
	std::string svgFilePath = ofFilePath::join(
		ofFilePath::join(ofFilePath::getCurrentExeDir(), svgSourceFolder),
		landerSvg + ".svg");
	ofFile svgFile = ofFile(svgFilePath, ofFile::ReadOnly, false);
	if (!svgFile.exists())
	{
		ofLogError("App") << "Couldn't fing svg file at " + svgFilePath;
		return;
	}
	//Read svg data
	svgHandler.load(svgFilePath);
	auto paths = svgHandler.getPaths();

	//Create polylines
	graphics.setMode(ofPath::POLYLINES);
	graphics.setFilled(false);
	graphics.setStrokeColor(ofColor::aliceBlue);
	graphics.setStrokeWidth(2);
	//Add points
	for (ofPath p : paths)
	{
		graphics.append(p);
	}
	
	float normalizeFactor = 1.f / std::max(svgHandler.getWidth(), svgHandler.getHeight());
	//Center on image center
	graphics.translate(ofVec2f(-svgHandler.getWidth() / 2.f, -svgHandler.getHeight() / 2.f));
	//Scale down to normalized space
	graphics.scale(normalizeFactor, normalizeFactor);

	//Create physics
	this->world = world;

	//Create physical body
	physicsBodyDef.type = b2BodyType::b2_dynamicBody;
	physicsBodyDef.allowSleep = false;
	physicsBodyDef.active = false;
	physicsBodyDef.angularDamping = params.angularDamping;
	physicsBodyDef.linearDamping = params.linearDamping;
	physicsBodyDef.position = b2Vec2(params.startingPos.x / OFX_BOX2D_SCALE, params.startingPos.y / OFX_BOX2D_SCALE);

	physicsBody = world->getWorld()->CreateBody(&physicsBodyDef);

	//Create collision shapes
	b2PolygonShape topShape;
	b2PolygonShape bottomShape;

	ofVec2f halfSize = topBoxSize / 2.f;
	topShape.SetAsBox(halfSize.x / OFX_BOX2D_SCALE, halfSize.y / OFX_BOX2D_SCALE, b2Vec2(0.f, halfSize.y / OFX_BOX2D_SCALE), 0.f);
	halfSize = bottomBoxSize / 2.f;
	bottomShape.SetAsBox(halfSize.x / OFX_BOX2D_SCALE, halfSize.y / OFX_BOX2D_SCALE, b2Vec2(0.f, halfSize.y / OFX_BOX2D_SCALE), 0.f);

	//Initialize fixtures
	topFixtureDef.shape = &topShape;
	topFixtureDef.density = params.density;
	topFixtureDef.friction = params.friction;
	topFixtureDef.restitution = params.bounce;
	bottomFixtureDef.shape = &bottomShape;
	bottomFixtureDef.density = params.density;
	bottomFixtureDef.friction = params.friction;
	bottomFixtureDef.restitution = params.bounce;

	//Create fixtures
	topFixture = physicsBody->CreateFixture(&topFixtureDef);
	bottomFixture = physicsBody->CreateFixture(&bottomFixtureDef);

	crashListener = new LanderCrashContactListener(this);
	crashListener->SetFixtureFilter(topFixture);
	LunarLanderConatactManager::Get()->AddCallback(crashListener, ContactCallbackFlag::PostSolve);
}

void Lander::Draw()
{
	if(!isActive)
		return;
	ofPushStyle();
	ofPushMatrix();
	//Apply physics transform to graphics

	ofTranslate(currentPosition);
	ofRotateRad(currentRotationRad);
	ofScale(currentScale);

	/*ofSetColor(ofColor::lightPink);
	ofDrawRectangle(-params.topBoxSize / 2.f - ofVec2f(0.f, params.topBoxSize.y / 2.f), params.topBoxSize.x, params.topBoxSize.y);
	ofSetColor(ofColor::azure);
	ofDrawRectangle(-params.bottomBoxSize / 2.f + ofVec2f(0.f, params.bottomBoxSize.y / 2.f), params.bottomBoxSize.x, params.bottomBoxSize.y);*/
	ofSetColor(180);
	graphics.draw();


	ofPopMatrix();

	ofSetColor(ofColor::white);
	std::string dbgString = "LANDER INFO\n";
	dbgString += "Pos: " + ofToString(currentPosition) + "\n";
	dbgString += "Rot: " + ofToString(GetRotationDeg()) + "\n";
	dbgString += "deltaRot: " + ofToString(currentRotationRate * RAD_TO_DEG) + "\n";
	dbgString += "thrust: " + ofToString(currentThrusterStrength) + "\n";
	ofDrawBitmapString(dbgString, ofVec2f(30, 30));

	ofPopStyle();
}

void Lander::Update()
{
	currentPosition = worldPtToscreenPt(physicsBody->GetPosition());
	currentRotationRad = physicsBody->GetAngle();
	physicsBody->ApplyForceToCenter(b2Vec2(sin(currentRotationRad) * currentThrusterStrength, -cos(currentRotationRad) * currentThrusterStrength), true);
	physicsBody->ApplyTorque(currentRotationRate, true);
}

void Lander::SetScale(float scale)
{
	currentScale = scale;

	//Clean up fixtures
	physicsBody->DestroyFixture(bottomFixture);
	physicsBody->DestroyFixture(topFixture);

	//Create new collision shapes 
	b2PolygonShape topShape;
	b2PolygonShape bottomShape;

	ofVec2f halfSize = topBoxSize * scale / OFX_BOX2D_SCALE / 2.f;
	topShape.SetAsBox(halfSize.x, halfSize.y, b2Vec2(0, halfSize.y), 0.f);
	halfSize = bottomBoxSize * scale / OFX_BOX2D_SCALE / 2.f;
	bottomShape.SetAsBox(halfSize.x, halfSize.y, b2Vec2(0, -halfSize.y), 0.f);

	//Create new fixtures
	topFixtureDef.shape = &topShape;
	bottomFixtureDef.shape = &bottomShape;

	topFixture = physicsBody->CreateFixture(&topFixtureDef);
	bottomFixture = physicsBody->CreateFixture(&bottomFixtureDef);
}

void Lander::Start(LanderParams params)
{
	ofLogNotice("Lander") << "Start";
	this->params = params;
	topFixture->SetDensity(params.density);
	topFixture->SetFriction(params.friction);
	topFixture->SetRestitution(params.bounce);
	bottomFixture->SetDensity(params.density);
	bottomFixture->SetFriction(params.friction);
	bottomFixture->SetRestitution(params.bounce);
	physicsBody->SetLinearDamping(params.linearDamping);
	physicsBody->SetAngularDamping(params.angularDamping);
	physicsBody->SetTransform(screenPtToWorldPt(params.startingPos), 0.f);
	physicsBody->SetLinearVelocity(b2Vec2(params.startVelocity, 0.f));
	physicsBody->SetActive(true);
	currentThrusterStrength = 0.f;
	isActive = true;
}

void Lander::Sleep()
{
	physicsBody->SetActive(false);
	isActive = false;
}

void Lander::SetThrusterStrength(float strength)
{
	currentThrusterStrength = max(strength, 0.f);
}

void Lander::AddThrusterStrength(float strength)
{
	currentThrusterStrength = max(strength + currentThrusterStrength, 0.f);
}

void Lander::SetRotationRate(float rotation)
{
	currentRotationRate = rotation;
}

void Lander::Reset()
{
	physicsBody->SetActive(false);
	physicsBody->SetTransform(
		b2Vec2(params.startingPos.x / OFX_BOX2D_SCALE, params.startingPos.y / OFX_BOX2D_SCALE),
		0.f
	);
	physicsBody->SetLinearVelocity(b2Vec2(params.startVelocity, 0.f));
	currentThrusterStrength = 0.f;
	physicsBody->SetActive(true);
}

ofVec2f Lander::GetPosition()
{
	return currentPosition;
}

float Lander::GetRotationRad()
{
	return currentRotationRad;
}

float Lander::GetRotationDeg()
{
	return RAD_TO_DEG * currentRotationRad;
}

bool Lander::IsStationary(float tolerance)
{
	float vel = physicsBody->GetLinearVelocity().Length();
	return vel < tolerance && vel > -tolerance;
}

b2Body* Lander::GetBody()
{
	return physicsBody;
}