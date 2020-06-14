#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <mutex>
#include <thread>
#include <atomic>
#include <future>

namespace corgi
{
	class Transform;

	// TODO : Move this it it's own class something later on?
	class ThreadPool
	{
	public:

		// By default the thread pool will try to get the optimal number of threads
		// for the current computer, used the hardware_concurrency value
		ThreadPool()
		{
			for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
			{
				threads.emplace_back(&ThreadPool::run, this);
			}
		}

		ThreadPool(unsigned int thread_count)
		{
			for (unsigned int i = 0; i < thread_count; ++i)
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

			for (auto& t : threads)
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
				stuff_to_do_ += 1;
			}
		}

		void add_tasks(std::vector<std::function<void()>> tasks)
		{
			std::unique_lock<std::mutex> ll(tasks_mutex_);

			for (auto& task : tasks)
				tasks_.push(task);

			stuff_to_do_ = tasks_.size();
		}

		void run()
		{
			std::function<void()> task;
			while (true)
			{
				std::unique_lock<std::mutex> ll(tasks_mutex_);

				// So basically here, the thread will only escape the condition variable 
				// if there is at least 1 task or if the thread pool is deleted
				while (tasks_.empty() && (!end))
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
				ll.lock();
				--stuff_to_do_;
				if (stuff_to_do_ == 0)
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
		int thread_count = 0;
		bool ended = false;
	};

	
	class TransformSystem : public AbstractSystem
	{
	public:

		TransformSystem(Scene& scene);
		~TransformSystem();

		ThreadPool tp;
		void sort();

		static void update_component(Transform& transform, Entity& entity);

		void update() override;
	};
}