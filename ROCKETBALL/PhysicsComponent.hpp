#pragma once

#include "Box2D/Common/b2Math.h"
#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include "Box2D/Collision/Shapes/b2CircleShape.h"
#include "Box2D/Dynamics/b2Body.h"
#include "Component.hpp"

class PhysicsComponent : public Component {
public:
    explicit PhysicsComponent(GameObject *gameObject);

	virtual ~PhysicsComponent();

    void initCircle(b2BodyType type, float radius,glm::vec2 center,float density, float friction, float restitution, float linearDamping, float angularDamping, bool fixedRotation);
    void initBox(b2BodyType type, glm::vec2 size,glm::vec2 center,float density);
	void initCarCollider(glm::vec2 size, glm::vec2 center, float _friction, float _density, float _linearDamping, float _angularDamping);
	void initCarSensorCollider(glm::vec2 size, glm::vec2 center);
	void initEdge(b2BodyType type, glm::vec2 point_1, glm::vec2 point_2, float density);

    void addForce(glm::vec2 force);     // Force gradually affects the velocity over time

    void addImpulse(glm::vec2 force);   // Instantly affects velocity

    void setLinearVelocity(glm::vec2 velocity);

    glm::vec2 getLinearVelocity();

	b2Fixture* fixture = nullptr;
	b2BodyType rbType;
	b2Body * body = nullptr;

    bool isSensor();

	float radius;

    void setSensor(bool enabled);
private:
    b2PolygonShape * polygon = nullptr;
    b2CircleShape * circle = nullptr;
	b2EdgeShape * edge = nullptr;
    
    b2Shape::Type shapeType;
    
    std::vector<PhysicsComponent *> collidingBodies;
    b2World * world = nullptr;
    friend class BirdGame;


};

