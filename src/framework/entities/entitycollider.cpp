//#include "entitycollider.h"
//#include "framework/world.h"
//
//void EntityCollider::getCollisionsWithModel(const Matrix44& m, const Vector3 center)
//{
//	Vector3 collision_point;
//	Vector3 collision_normal;
//
//	float sphere_radius = World::get_instance()->sphere_radius;
//	float sphere_ground_radius = World::get_instance()->sphere_ground_radius;
//	float player_height = World::get_instance()->player_height;
//
//	Vector3 floor_sphere_center = center + Vector3(0.f, sphere_ground_radius, 0.0f);
//
//	if (mesh->testRayCollision(m, floor_sphere_center, sphere_radius, collision_point))
//	{
//		collisions.push_back({ collision_point, collision_normal.normalize(), floor_sphere_center });
//	}
//
//	Vector3 character_center = center + Vector3(0.f, player_height, 0.f);
//	if (mesh->testRayCollision(m, character_center, sphere_radius, collision_point))
//	{
//		collisions.push_back({ collision_point, collision_normal.normalize(), character_center });
//	}
//
//	if (mesh->testRayCollision(m, character_center, Vector3(0, -1, 0), collision_point))
//	{
//		ground_collisions.push_back({ collision_point, collision_normal.normalize());
//	}
//}
//
//void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions)
//{
//	if (!(layer & filter))
//		return;
//
//	if (!isInstanced)
//	{
//		getCollisionsWithModel(model, target_position, collisions, ground_collisions);
//	}
//	else
//	{
//		for (int i = 0; i < models.size(); ++i)
//		{
//			getCollisionsWithModel(models[i], target_position, collisions, ground_collisions);
//		}
//	}
//}
//
//bool EntityCollider::testRayCollision(Matrix44 model, Vector3 ray_origin, Vector3 ray_direction, Vector3& collision, Vector3& normal, float max_ray_dist, bool in_object_space)
//{
//	return false;
//}
