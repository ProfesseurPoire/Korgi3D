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
	auto opt_pool = scene_.pools().get<Transform>();

	if(!opt_pool)	
	{
		log_error("No Transform component pool exist for Transform System");
		return;
	}

	auto& pool = opt_pool->get();

	std::vector<int> ids;
	ids.reserve(pool.components().size());

	for(auto& pair : pool)
	{
		ids.push_back(pair.first);
	}

	std::sort(
		ids.begin(), ids.end(),
		[&](auto& a, auto& b)
		{
			auto ea = scene_.entities()[a];
			auto eb = scene_.entities()[b];

			if(!( ea || eb))
			{
				log_error("One entity doesn't have a transform component attached");
			}

			
			return ea->get().depth()< eb->get().depth();
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

	auto opt_pool = scene_.pools().get<Transform>();

	if (!opt_pool)
	{
		log_error("Scene doesn't own a Transform component pool");
		return;
	}

	auto& pool = opt_pool->get();
	
	for (const auto transform_pair: pool)
	{
		auto opt_entity = scene_.entities()[transform_pair.first];

		if(opt_entity)
		{
			auto& entity = opt_entity->get();

			if (!levels.contains(entity.depth()))
			{
				levels[entity.depth()] = 1;
			}
			else
			{
				levels[entity.depth()]++;
			}
		}
	}

	/*int size = 250;
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
							, *scene_.entities()[pool.component_index_to_entity_id()[i]]);
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
							, *scene_.entities()[pool.component_index_to_entity_id()[i]]);
					}
				}
			);
		}

		tp.start();
		tp.wait_finished();

		offset += count;
	}*/

	//int start = 0;
	
	
	for (auto& transform : scene_.pools().get<Transform>()->get())
	{
		auto& entity = *scene_.entities()[transform.first];
		update_component(transform.second,entity);
	}
}

void TransformSystem::update_component(Transform& transform, Entity& entity)
{
	if (transform._dirty)	// Means the transform needs to be updated
	{
		if (!transform.is_world())
		{
			if (entity.parent() && entity.parent()->has_component<Transform>())
			{
				auto m = entity.parent()->get_component<Transform>()->get().model_matrix_;
				transform.model_matrix_ = m * local_matrix(transform);
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
			auto opt_transform = child->get_component<Transform>();

			if(opt_transform)
			{
				opt_transform->get().set_dirty();
			}
		}
	}
}
}