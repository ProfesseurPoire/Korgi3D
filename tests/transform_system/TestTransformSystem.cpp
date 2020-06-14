#include <corgi/test/test.h>

#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>

#include <corgi/components/Transform.h>
#include <corgi/systems/TransformSystem.h>

#include <mutex>
#include <thread>
#include <atomic>
#include <future>

using namespace corgi;
using namespace corgi::test;

int main()
{
	return test::run_all();
}

class TestTransformSystem :  public Test
{
public:

	Scene scene;


	void set_up() override
	{
		scene.pools().add<Transform>();
		scene.systems().add<TransformSystem>();

		auto& e1 = scene.new_entity();
		e1.add_component<Transform>(5.0f,0.0f);

		auto& e11 = e1.add_child();
		e11.add_component<Transform>(1.0f,0.0f);

		auto& e12 = e1.add_child();
		e12.add_component<Transform>(2.0f,0.0f);

		auto& e2 = scene.new_entity();
		e2.add_component<Transform>(10.0f,0.0f);

		auto& e21 = e2.add_child();
		e21.add_component<Transform>(3.0f, 0.0f);
	}

	void tear_down()override
	{

	}

	TransformSystem& transform_system()
	{
		return scene.systems().get<TransformSystem>();
	}

	ComponentPool<Transform>& transform_pool()
	{
		return *scene.pools().get<Transform>();
	}
};

TEST_F(TestTransformSystem, SingleThread)
{
	assert_that(scene.pools().get<Transform>()->size(),equals(5));

	int i=0;

	for(auto& pair : transform_pool())
	{
		switch (i)
		{
		case 0:
			assert_that(pair.second.position().x, equals(5.0f))
		break;

		case 1:
			assert_that(pair.second.position().x, equals(1.0f))
		break;

		case 2:
			assert_that(pair.second.position().x, equals(2.0f))
		break;

		case 3:
			assert_that(pair.second.position().x, equals(10.0f))
		break;

		case 4:
			assert_that(pair.second.position().x, equals(3.0f))
		break;
		}
		i++;
	}
}

TEST_F(TestTransformSystem, Sorting)
{
	 transform_system().sort();
	 transform_system().update();

	// I feel this test might not work on every compiler depending on how the
	// sort works?
	int i=0;
	for(auto& pair : transform_pool())
	{
		switch (i)
		{
		case 0:
			assert_that(pair.second.position().x, equals(5.0f))
			assert_that((pair.second.world_matrix()*Vec3(0.0f,0.0f,0.0f)).x, equals(5.0f));
		break;

		case 1:
			assert_that(pair.second.position().x, equals(10.0f))
			assert_that((pair.second.world_matrix()*Vec3(0.0f,0.0f,0.0f)).x, equals(10.0f));
		break;

		case 2:
			assert_that(pair.second.position().x, equals(1.0f))
			assert_that((pair.second.world_matrix()*Vec3(0.0f,0.0f,0.0f)).x, equals(6.0f));
		break;

		case 3:
			assert_that(pair.second.position().x, equals(2.0f))
			assert_that((pair.second.world_matrix()*Vec3(0.0f,0.0f,0.0f)).x, equals(7.0f));
		break;

		case 4:
			assert_that(pair.second.position().x, equals(3.0f));
			assert_that((pair.second.world_matrix()*Vec3(0.0f,0.0f,0.0f)).x, equals(13.0f));
		break;
		}
		i++;
	}
}


class ThreadPool
{
public:

	// By default the thread pool will try to get the optimal number of threads
	// for the current computer, used the hardware_concurrency value
	ThreadPool()
	{
		for(int i=0; i< std::thread::hardware_concurrency(); ++i)
		{
			threads.emplace_back(&ThreadPool::run, this);
		}
		Sleep(1000);
	}

	ThreadPool(unsigned int thread_count)
	{
		for (auto i = 0; i < thread_count; ++i)
		{
			threads.emplace_back(&ThreadPool::run, this);
		}
	}

	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> l(tasks_mutex_);
			end = true;
		}
		cv.notify_all();

		for(auto& t : threads)
			t.join();
	}

	void start()
	{
		{
			std::unique_lock<std::mutex> ll(tasks_mutex_);

			if (tasks_.size() == 1)
			{
				cv.notify_one();
			}
			else
			{
				cv.notify_all();
			}
		}
		
	}

	// Lock the thread calling this function until the thread pool is empty 
	void wait_finished() 
	{
		std::unique_lock<std::mutex> ll(tasks_mutex_);
		
		while (stuff_to_do_ != 0)
		{
			finished_.wait(ll);
		}
	}

	void add_task(std::function<void()> task)
	{
		{
			std::unique_lock<std::mutex> ll(tasks_mutex_);
			tasks_.push(task);
			stuff_to_do_ +=1;
		}
	}

	void add_tasks(std::vector<std::function<void()>> tasks)
	{
		std::unique_lock<std::mutex> ll(tasks_mutex_);

		for(auto& task : tasks)
			tasks_.push(task);

		stuff_to_do_ = tasks_.size();
	}

	void run()
	{
		std::function<void()> task;
		while(true)
		{
			std::unique_lock<std::mutex> ll (tasks_mutex_);

			// So basically here, the thread will only escape the condition variable 
			// if there is at least 1 task or if the thread pool is deleted
			while(tasks_.empty() && (!end))
			{
				cv.wait(ll);
			}
			
			// Directly kills a thread once it wakes up if the ThreadPool should be deleted and there's no more tasks
			if (end && tasks_.empty())	
			{
				return;
			}
			
			task = std::move(tasks_.top());
			tasks_.pop();
			ll.unlock();
			task();
			--stuff_to_do_;
			if(stuff_to_do_==0)
			{
				finished_.notify_all();
			}
		}
	}

	std::vector<std::thread> threads;
	std::mutex work_left_mutex;
	std::mutex tasks_mutex_;

	std::atomic<int> stuff_to_do_ = 0;
	std::atomic<int> sleeping_threads = 0;

	std::condition_variable cv;
	std::condition_variable finished_;
	std::stack<std::function<void()>> tasks_;
	std::atomic<bool> end = false;
	int thread_count=0;
	bool ended = false;
};

int first_depth_size	= 200;
int second_depth_size	= 0;
int third_depth_size	= 0;


class ThreadPoolT : public Test
{
public:

	ThreadPool threadpool;
	Scene scene;


	void set_up() override
	{
		scene.pools().add<Transform>();
		scene.systems().add<TransformSystem>();

		for (int i = 0; i < first_depth_size; i++)
		{
			auto& e = scene.new_entity();
			e.add_component<Transform>(1.0f, 2.0f);
			for (int j = 0; j < second_depth_size; j++)
			{
				auto& e1 = e.add_child();
				e1.add_component<Transform>(1.0f, 2.0f);
				for (int k = 0; k < third_depth_size; k++)
				{
					auto& e2 = e1.add_child();
					e2.add_component<Transform>(1.0f, 2.0f);
				}
			}
		}
		transform_system().sort();
	}

	void tear_down()override
	{
	}

	TransformSystem& transform_system()
	{
		return scene.systems().get<TransformSystem>();
	}
};

TEST_F(ThreadPoolT, MultiThreaded)
{
	//  counting how many entities there is by depth order

	std::map<int, int> levels;


	for (auto& pool : *scene.pools().get<Transform>())
	{
		auto& entity = *scene.entities_[pool.first];

		if (!levels.contains(entity.depth()))
		{
			levels[entity.depth()] = 1;
		}
		else
		{
			levels[entity.depth()]++;
		}
	}

	auto& pool = *scene.pools().get<Transform>();


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

			threadpool.add_task
			(
				[&, st = start, en = end]()
				{
					for (int i = st; i < en; i++)
					{
						TransformSystem::update_component(pool.components()[i]
							, *scene.entities_[pool.component_index_to_entity_id()[i]]);
					}
				}
			);
		}

		if (leftover != 0)
		{
			int start = offset + repeat * size;
			int end = offset + repeat * size + leftover;

			threadpool.add_task
			(
				[&, st = start, en = end]()
				{
					for (int i = st; i < en; i++)
					{
						TransformSystem::update_component(pool.components()[i]
							, *scene.entities_[pool.component_index_to_entity_id()[i]]);
					}
				}
			);
		}

		threadpool.start();
		threadpool.wait_finished();

		offset += count;
	}
}


class SingleThreadedTransformPerformance : public Test
{
	public:

	Scene scene;

	void set_up() override
	{
		scene.pools().add<Transform>();
		scene.systems().add<TransformSystem>();

		for(int i=0; i< first_depth_size; i++)
		{
			auto& e = scene.new_entity();
			e.add_component<Transform>(1.0f, 2.0f);
			for(int j=0; j< second_depth_size; j++)
			{
				auto& e1 = e.add_child();
				e1.add_component<Transform>(1.0f, 2.0f);
				for(int k = 0; k< third_depth_size; k++)
				{
					auto& e2 = e1.add_child();
					e2.add_component<Transform>(1.0f, 2.0f);
				}
			}
		}

	transform_system().sort();

	}

	void tear_down()override
	{

	}

	TransformSystem& transform_system()
	{
		return scene.systems().get<TransformSystem>();
	}
};

TEST_F(SingleThreadedTransformPerformance, SingleThreaded)
{
	// I'm kinda assuming it just works  TM
	transform_system().update();
}




//
//
//
//class AsyncPerformanceTransformSystem : public Test
//{
//public:
//
//	Scene scene;
//
//
//	void set_up() override
//	{
//		scene.pools().add<Transform>();
//		scene.systems().add<TransformSystem>();
//
//		for (int i = 0; i < first_depth_size; i++)
//		{
//			auto& e = scene.new_entity();
//			e.add_component<Transform>(1.0f, 2.0f);
//			for (int j = 0; j < second_depth_size; j++)
//			{
//				auto& e1 = e.add_child();
//				e1.add_component<Transform>(1.0f, 2.0f);
//				for (int k = 0; k < third_depth_size; k++)
//				{
//					auto& e2 = e1.add_child();
//					e2.add_component<Transform>(1.0f, 2.0f);
//				}
//			}
//		}
//		transform_system().sort();
//	}
//
//	void tear_down()override
//	{
//		//threadpool.reset();
//	}
//
//	TransformSystem& transform_system()
//	{
//		return scene.systems().get<TransformSystem>();
//	}
//};
//
//TEST_F(AsyncPerformanceTransformSystem, MultiThreaded)
//{
//	//  counting how many entities there is by depth order
//
//	std::map<int, int> levels;
//
//
//	for (auto& pool : *scene.pools().get<Transform>())
//	{
//		auto& entity = *scene.entities_[pool.first];
//
//		if (!levels.contains(entity.depth()))
//		{
//			levels[entity.depth()] = 1;
//		}
//		else
//		{
//			levels[entity.depth()]++;
//		}
//	}
//
//	auto& pool = *scene.pools().get<Transform>();
//
//
//	int size = 200;
//	int  offset = 0;
//
//	std::vector<std::future<void>> returns;
//
//	for (auto pair : levels)
//	{
//		int count = pair.second;
//		int leftover = count % size;
//		int repeat = count / size;
//
//		for (int k = 0; k < repeat; k++)
//		{
//			int start = offset + k * size;
//			int end = offset + k * size + size;
//
//			returns.push_back(std::async(
//			
//				[&, st = start, en = end]()
//				{
//					for (int i = st; i < en; i++)
//					{
//						TransformSystem::update_component(pool.components()[i]
//							, *scene.entities_[pool.component_index_to_entity_id()[i]]);
//					}
//				}
//			));
//		}
//
//		if (leftover != 0)
//		{
//			int start = offset + repeat * size;
//			int end = offset + repeat * size + leftover;
//
//			returns.push_back(std::async(
//				[&, st = start, en = end]()
//				{
//					for (int i = st; i < en; i++)
//					{
//						TransformSystem::update_component(pool.components()[i]
//							, *scene.entities_[pool.component_index_to_entity_id()[i]]);
//					}
//				}
//			));
//		}
//
//		for(auto& r : returns)
//		{
//			r.get();
//		}
//		
//		
//		offset += count;
//	}
//}