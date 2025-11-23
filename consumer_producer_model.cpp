/*

Achieve a consumer-producer model, use thread+mutex+condition_variables, 
to write a application that can safely shared queuesa,synchronizations

Keys:
Do you understand multi-thread
Do you understand mutex, condition_variable
Do you understand blocking queue
Can you avoid dead lock, busy waiting

What to write:
- Producer
    - generate data
    - add data into queue
    - if queue is full, waiting(block)

- consumer
    - get data from the queue
    - if queue is empty, waiting(block)
        - if not block, keep using cpu for nothing

- shared queue
    - multi thread safe
    - protect from mutex
    - use condition_variable to wait/awake
*/

#include<iostream>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<thread>
#include<chrono>

std::mutex cout_mtx;  // protect cout output

// typename means T is a type (like int, string, person etc)
template<typename T>  
class BlockingQueue{

private:
    std::queue<T> que;  
    std::mutex mtx;             // mutex: only one thread can get one lock one time
    std::condition_variable cv;

/* mutex ensuers only one thread can enter a critical session at a time 
so when a thread calls: mtx.lock(), when another thread calling mtx.lock(), it must wait until the first thread calls mtx.unlock()
！-- std::unique_lock is a RAII manager for a mutex --！
One mutex oer shared resource , here the resource is que*/

public:
    void push(const T& value){
        std::unique_lock<std::mutex> lock(mtx);     //unique_lock manage mutex
        que.push(value);
        cv.notify_one();   
    }

    T pop(){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{return !que.empty(); });   //wait there is something
        T value = que.front();
        que.pop();
        return value;
    }
};


BlockingQueue<int> bq;
// so this is initialize the members, so if we have one more std::vector<T> vec;
// then with the initialization, it will have both queue and vector

void producer(int id){
    for(int i=0 ; i<5; i++){
        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << "Producer " << id << " produced: " << i << std::endl;
        }
        bq.push(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(int id) {
    for(int i =0; i<5; i++){
        int value = bq.pop();
        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << " >>>Consumer " << id << " consumed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}
 
int main(){

/* default cpp is single thread, here we create multi-thread*/

    std::thread p1(producer, 1);   // the first one is function, second or more are input variables
    std::thread p2(producer, 2);
    std::thread c1(consumer, 1);
    std::thread c2(consumer, 2);

    /* join() means , make the main thread wait for this thread finish */

    p1.join();  //main thread will stop here and wait for p1 finish
    p2.join();   //main thread will stop here and wait for p2 finish
    c1.join();
    c2.join();

    return 0;
}