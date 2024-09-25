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
        explicit ThreadPool(size_t);
        template<class F, class... Args>
        decltype(auto) enqueue(F&& f, Args&&... args);
        ~ThreadPool();
    private:

        std::vector< std::thread > workers;

        std::queue< std::packaged_task<void()> > tasks;


        std::mutex queue_mutex;
        std::condition_variable condition;
        std::condition_variable condition_producers;
        bool stop;
    };


    inline ThreadPool::ThreadPool(size_t threads = std::thread::hardware_concurrency())
        : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back(
                [this]
                {
                    for (;;)
                    {
                        std::packaged_task<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                            if (tasks.empty()) {
                                condition_producers.notify_one(); // notify the destructor that the queue is empty
                            }
                        }

                        task();
                    }
                }
            );
    }


    template<class F, class... Args>
    decltype(auto) ThreadPool::enqueue(F&& f, Args&&... args)
    {
        using return_type = std::invoke_result_t<F, Args...>;

        std::packaged_task<return_type()> task(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task.get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);


            if (stop)
                CORE_ASSERT(false, "Enqueue on stopped ThreadPool");
            
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
        return res;
    }


    inline ThreadPool::~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition_producers.wait(lock, [this] { return tasks.empty(); });
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }
}