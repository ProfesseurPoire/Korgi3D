#include "TransformSystem.h"

#include <corgi/ecs/Scene.h>

#include <corgi/components/Transform.h>
#include <corgi/ecs/Entity.h>



namespace corgi
{


	

static Matrix local_matrix(Transform& transform)
{
	return Matrix::translation
	(
		transform.position().x, transform.position().y, transform.position().z
	)
		* Matrix::rotation_x(transform.euler_angles().x)
		* Matrix::rotation_y(transform.euler_angles().y)
		* Matrix::rotation_z(transform.euler_angles().z)
		* Matrix::scale(transform.scale().x, transform.scale().y, transform.scale().z);
}

TransformSystem::TransformSystem(Scene& scene)
	: AbstractSystem(scene)
{
	
}

TransformSystem::~TransformSystem()
{

}

void TransformSystem::sort()
{
	auto& pool = *scene_.pools().get<Transform>();

	std::vector<int> ids;
	ids.reserve(pool.components().size());

	for(auto& pair : pool)
	{
		ids.push_back(pair.first);
	}

	std::sort(
		ids.begin(),
		ids.end(),
		[&](auto& a, auto& b)
		{
			auto& ea = *scene_.entities_[a];
			auto& eb = *scene_.entities_[b];

			return ea.depth()< eb.depth();
		}
	) ;

	// now It's supposed to be sorted
	auto& scene = scene_;
	for(size_t i=0; i< pool.components().size(); ++i)
	{
		auto first_id 	= pool.component_index_to_entity_id()[i];
		auto second_id 	=  ids[i];

		if(first_id!=second_id)
		{
			auto second_index  = pool.entity_id_to_components()[ids[i]];

			auto current 	= pool.components()[i]; 
			auto ne 		= pool.components()[second_index];

			pool.components()[i] = pool.components()[second_index];
			pool.components()[second_index]=current;

			pool.entity_id_to_components()[second_id]= i;
			pool.entity_id_to_components()[first_id]= second_index;

			pool.component_index_to_entity_id()[i]= second_id ;
			pool.component_index_to_entity_id()[second_index]= first_id;
		}
	}
}


void TransformSystem::update()
{
	sort();

	//  counting how many entities there is by depth order

	std::map<int, int> levels;


	for (auto& pool : *scene_.pools().get<Transform>())
	{
		auto& entity = *scene_.entities_[pool.first];

		if (!levels.contains(entity.depth()))
		{
			levels[entity.depth()] = 1;
		}
		else
		{
			levels[entity.depth()]++;
		}
	}

	auto& pool = *scene_.pools().get<Transform>();


	int size = 250;
	int  offset = 0;

	for (auto pair : levels)
	{
		int count = pair.second;
		int leftover = count % size;
		int repeat = count / size;

		for (int k = 0; k < repeat; k++)
		{
			int start = offset + k * size;
			int end = offset + k * size + size;

			tp.add_task
			(
				[&, st = start, en = end]()
				{
					for (int i = st; i < en; i++)
					{
						update_component(pool.components()[i]
							, *scene_.entities_[pool.component_index_to_entity_id()[i]]);
					}
				}
			);
		}

		if (leftover != 0)
		{
			int start = offset + repeat * size;
			int end = offset + repeat * size + leftover;

			tp.add_task
			(
				[&, st = start, en = end]()
				{
					for (int i = st; i < en; i++)
					{
						update_component(pool.components()[i]
							, *scene_.entities_[pool.component_index_to_entity_id()[i]]);
					}
				}
			);
		}

		tp.start();
		tp.wait_finished();

		offset += count;
	}
	
	/*for (auto& transform : *scene_.pools().get<Transform>())
	{
		auto& entity = *scene_.entities_[transform.first];
		update_component(transform.second,entity);
	}*/
}

void TransformSystem::update_component(Transform& transform, Entity& entity)
{
	if (transform._dirty)	// Means the transform needs to be updated
	{
		if (!transform.is_world())
		{
			if (entity.parent() && entity.parent()->has_component<Transform>())
			{
				transform.model_matrix_ = entity.parent()->get_component<Transform>().model_matrix_ * local_matrix(transform);
			}
			else
			{
				transform.model_matrix_ = local_matrix(transform);
			}
		}
		else
		{
			transform.model_matrix_ = local_matrix(transform);
		}

		transform._dirty = false;

		// Because the children transformations depends on their parent, we have to 
		// update them. Their dirty flag will set to false so we don't update them more than once
		for (auto& child : entity.children())
		{
			if (child->has_component<Transform>())
			{
				child->get_component<Transform>().set_dirty();
			}
		}
	}
}
}