//
// Created by Morten Nobel-Jørgensen on 19/10/2017.
//

#include <Box2D/Box2D.h>
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
//#include "BirdGame.hpp"

PhysicsComponent::PhysicsComponent(GameObject *gameObject)
        : Component(gameObject)
{
    world = RocketBall::gameInstance->world;
}

PhysicsComponent::~PhysicsComponent()
{
	//RocketBall::instance->deregisterPhysicsComponent(this);
	delete polygon;
	delete circle;
	if (body != nullptr && fixture != nullptr) {
		body->DestroyFixture(fixture);
		fixture = nullptr;
	}
	if (world != nullptr && body != nullptr) {
		world->DestroyBody(body);
		body = nullptr;
	}
}

void PhysicsComponent::addImpulse(glm::vec2 force) {
    b2Vec2 iForceV{force.x,force.y};
    body->ApplyLinearImpulse(iForceV, body->GetWorldCenter(), true);
}

void PhysicsComponent::addForce(glm::vec2 force) {
    b2Vec2 forceV{force.x,force.y};
    body->ApplyForce(forceV,body->GetWorldCenter(),true);
}

glm::vec2 PhysicsComponent::getLinearVelocity() {
    b2Vec2 v = body->GetLinearVelocity();
    return {v.x,v.y};
}

void PhysicsComponent::setLinearVelocity(glm::vec2 velocity) {
    b2Vec2 v{velocity.x, velocity.y};
    if (velocity != glm::vec2(0,0)){
        body->SetAwake(true);
    }
    body->SetLinearVelocity(v);
}

void PhysicsComponent::initCircle(b2BodyType type, float radius, glm::vec2 center, float density, float friction, float restitution, float linearDamping, float angularDamping, bool fixedRotation) {
    assert(body == nullptr);
    // do init
    shapeType = b2Shape::Type::e_circle;
    b2BodyDef bd;
    bd.type = type;
    rbType = type;
    bd.position = b2Vec2(center.x, center.y);
	bd.fixedRotation = fixedRotation;
    body = world->CreateBody(&bd);
	body->SetLinearDamping(linearDamping);
	body->SetAngularDamping(angularDamping);
    circle = new b2CircleShape();
    circle->m_radius = radius;
    b2FixtureDef fxD;
    fxD.shape = circle;
    fxD.density = density;
	fxD.friction = friction;
	fxD.restitution = restitution;
	//body->ResetMassData();
    fixture = body->CreateFixture(&fxD);

	RocketBall::gameInstance->registerPhysicsComponent(this);
}

void PhysicsComponent::initBox(b2BodyType type, glm::vec2 size, glm::vec2 center, float density) {
    assert(body == nullptr);
    // do init
    shapeType = b2Shape::Type::e_polygon;
    b2BodyDef bd;
    bd.type = type;
    rbType = type;
    bd.position = b2Vec2(center.x, center.y);
    body = world->CreateBody(&bd);
    polygon = new b2PolygonShape();
    polygon->SetAsBox(size.x, size.y, {0,0}, 0);
    b2FixtureDef fxD;
	fxD.friction = 1.0f;
    fxD.shape = polygon;
    fxD.density = density;

	fixture = body->CreateFixture(&fxD);

	RocketBall::gameInstance->registerPhysicsComponent(this);
}

void PhysicsComponent::initEdge(b2BodyType type, glm::vec2 point_1, glm::vec2 point_2, float density)
{
	assert(body == nullptr);
	// do init
	shapeType = b2Shape::Type::e_edge;
	b2BodyDef bd;
	bd.type = type;
	rbType = type;
	//bd.position = b2Vec2(center.x, center.y);
	body = world->CreateBody(&bd);
	edge = new b2EdgeShape();
	edge->Set(b2Vec2(point_1.x, point_1.y), b2Vec2(point_2.x, point_2.y));
	//polygon = new b2PolygonShape();
	//polygon->SetAsBox(size.x, size.y, { 0,0 }, 0);
	b2FixtureDef fxD;
	fxD.shape = edge;
	fxD.density = density;
	body->ResetMassData();
	fixture = body->CreateFixture(&fxD);

	RocketBall::gameInstance->registerPhysicsComponent(this);
}

bool PhysicsComponent::isSensor() {
    return fixture->IsSensor();
}

void PhysicsComponent::setSensor(bool enabled) {
    fixture->SetSensor(enabled);
}