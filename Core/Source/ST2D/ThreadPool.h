#pragma once


#include "ST2D/Log.h"

#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <type_traits>

namespace ST
{

    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();


        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
            -> std::future<typename std::invoke_result<F, Args...>::type>;

        void stopAll();
        void waitToStopAll();
        void readyToWork();

    private:

        std::vector<std::thread> workers;

        std::queue<std::function<void()>> tasks;


        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;
    };


    inline ThreadPool::ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i)
            workers.emplace_back(
                [this]
                {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queueMutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }


                        task();
                    }
                }
            );
    }


    template<class F, class... Args>
    auto ThreadPool::enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            if (!stop)
            	tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    inline void ThreadPool::stopAll()
    {
	    {
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
        {
            std::unique_lock<std::mutex> lock(queueMutex);
			// clear all tasks
			tasks = std::queue<std::function<void()>>();
        }
    }

    inline void ThreadPool::waitToStopAll()
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
			worker.join();
    }

    inline void ThreadPool::readyToWork()
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = false;
		}
    }


    inline ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }
}