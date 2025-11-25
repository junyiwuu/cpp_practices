#include<condition_variable>
#include<functional>
#include<iostream>
#include<mutex>
#include<queue>
#include<thread>


class threadPool{

public:
    threadPool(size_t num_threads = std::thread::hardware_concurrency()){
        //creating worker threads
        for(size_t i=0 ; i<num_threads; ++i){
            threads_.emplace_back( [this]{                           // this allow the lambda access the class's member variables
                while(true){
                    std::function<void()>task;                       // store any callable object
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);

                        //waiting until there is a task to execute or the pool is stopped
                        cv_.wait(lock, [this]{
                            return !tasks_.empty() || stop_ ;
                        });

                        // exit the thread in case the pool is stopped and there are no tasks
                        if(stop_ && tasks_.empty()){ return; }

                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~threadPool(){
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }

        //notify all threads
        cv_.notify_all();

        // joining all worker threads to ensure they have completed their tasks
        for(auto& thread: threads_){
            thread.join();
        }
    }

    // enqueue task for execution by the thread pool
    void enqueue(std::function<void()> task){
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(std::move(task));
        }
        cv_.notify_all();
    }



private:
    // vector to store worker threads
    std::vector<std::thread> threads_;

    //queue of tasks
    std::queue<std::function<void()>> tasks_;

    // mutex to synchronize access to shared data
    std::mutex queue_mutex_;

    // condition variable to signal changes in the state of the tasks queue
    std::condition_variable cv_;

    //flag to indicate whether the thread pool should stop or not
    bool stop_ = false;
};








int main(){
    //creaye a thread pool with 4 threads
    threadPool pool(4);
    std::mutex cout_guard;

    // enqueue tasks for execution
    for(int i = 0; i<5; ++i){
        pool.enqueue([i, &cout_guard]{                              // copy i value inside
            {
                std::lock_guard<std::mutex> lock(cout_guard);
                std::cout<< "Task " << i << " is running on thread"
                << std::this_thread::get_id() << std::endl;

            }

                // simulate some work
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }

    return 0;

}




/*

In the above code, we have used the following C++ features for the implementation of the thread pool:

    1. A vector of worker threads, a task queue, a mutex for synchronization, a condition variable 
       for signaling, and a boolean flag to indicate whether the pool should stop are all managed
       by the ThreadPool class.

    2. The worker threads are initialized by the constructor, who then puts them in an endless loop
        while they wait for jobs to be enqueued. We use a wrapper class std::function over the given
        tasks.

    3. A job is added to the queue and one of the worker threads is notified to begin executing it 
        using the enqueue method.

    4. To guarantee a clean shutdown, the destructor joins the worker threads, sets the stop flag, 
        and informs all threads.

    5. A ThreadPool with four threads is formed in the main function. Ten jobs are queued up, each 
        of which prints a message including the task number and the thread ID that is currently 
        carrying it out.

    6. It should be noted that in a real-world situation, you would usually connect the threads or
        use some other kind of synchronization to make sure that all jobs are finished before the
        program ends.


    Advantages of Thread Pooling in C++

        1. Resource Management: Thread pools effectively manage resources by preventing resource
          depletion by restricting the number of threads that are executing concurrently.

        2. Enhanced Performance: By lowering the cost involved in establishing and terminating 
           threads, reusing threads enhances performance.
           
        3. Scalability: Thread Pools are scalable in a variety of settings because they may 
           dynamically modify the number of worker threads according to the capabilities of the
           system.

           
    Disadvantages of Thread Pooling in C++
    
        1. Thread pool adds complexity to the code hence managing threads and task queues might 
           impact the performance of other lightweight tasks.

        2. The thread pool works on the assumption that each task is independent. So, Handling the
           dependencies between tasks is challenging when one task depends on the result of another 
           task.

        3. The behavior of thread pools is platform-dependent. Hence, behavior may vary in different
           operating systems and C++ compilers. 
    
    Conclusion
    C++ thread pools offer an effective method of managing several tasks at once, with advantages in 
    resource management, performance, and scalability. Through the use of threads, developers may 
    build high-performing, responsive apps in a methodical and controlled manner and thread pooling 
    optimizes the use of resources and minimizes the thread creation and deletion overhead


*/

