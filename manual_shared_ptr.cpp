/*


*/

#include <iostream>

template<typename T>
class sharedPtr{
public:
    explicit sharedPtr(T* p = nullptr): ptr(p), count(new size_t(p? 1: 0)){}
    // if p is not null,count 1, otherwise 0

    // copy constructor
    sharedPtr(const sharedPtr& other){
        ptr = other.ptr;
        count = other.count;
        if(ptr){(*count)++;}
    }

    // copy assignment
    sharedPtr& operator=(const sharedPtr& other){
        if( this != &other){
            release();
            ptr = other.ptr;
            count = other.count;
            if(ptr){
                (*count)++;  }
        }
        return *this;
    }

    ~sharedPtr(){
        release();
    }


    void release(){
        if(ptr && --(*count)==0){
            delete ptr;
            delete count;
        }
    }

    T& operator*() const {return *ptr;}
    T* operator->() const {return ptr;}   
    
private:
    T* ptr;
    size_t* count;  // because all sharedPtr will share the same counter



};



/*
USAGE:
sharedPtr<int> p1(new int(10));     // count=1
sharedPtr<int> p2(p1);              // count=2
or
sharedPtr<int> p4(new int(42));
p4 = p1;


*/