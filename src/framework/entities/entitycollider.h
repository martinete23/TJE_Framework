//#pragma once
//
//#include "entitymesh.h"
//
//enum {
//	NONE = 0,
//	FLOOR = 1 << 0,
//	WALL = 1 << 1,
//	PLAYER = 1 << 2,
//	ENEMY = 1 << 3,
//	// Both WALL and FLOOR are considered SCENARIO
//	// using OR operator
//	SCENARIO = WALL | FLOOR,
//	ALL = 0xFF
//};
//
//
//class EntityCollider : public EntityMesh
//{
//public:
//	
//	bool is_dynamic = false;
//	
//	int layer = NONE;
//
//	void getCollisionsWithModel(const Matrix44 &m, const Vector3);
//	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions);
//	bool testRayCollision(Matrix44 model, Vector3 ray_origin, Vector3 ray_direction, Vector3& collision, Vector3& normal, float max_ray_dist, bool in_object_space);
//	bool testSphereCollision(Matrix44 model, Vector3 center, float radius, Vector3& collision, Vector3& normal);
//};
//
