/*
Achieve manual unique pointer
unique pointer is: there is only one pointer, point to a memory
NOT allow to have another pointer point to the same memory
*/


/*
copy constructor: (const T&)
copy assignment operator: operator=(const T&)
not allow : =delete

noexcept means this function will not throw. if there is error, program will just std::terminate()

*/
#include<iostream>

template<typename T>
class uniquePtr{
public:
    explicit uniquePtr(T* p = nullptr): ptr(p) {}
    ~uniquePtr(){
        delete ptr;
    }

    // copy not allowed
    uniquePtr(const uniquePtr&) = delete;               // delete copy constructor
    uniquePtr& operator=(const uniquePtr&) = delete;    // delete copy assignment operator.. (at front) uniquePtr& means return reference (return *this) . if it is just uniquePtr, means return the copy

    // move constructor --------------------------------------------
    uniquePtr(uniquePtr&& other)noexcept: ptr(other.ptr){
        other.ptr = nullptr;
    }
    /* ptr(other.ptr) means copy other.ptr to my ptr here
    other.ptr = nullptr; means we remove resource that in other, because we are unique pointer

    usage: uniquePtr a(new T);  uniquePtr b(std::move(a));
    or: uniquePtr p = uniquePtr(new T);

    noexcept: semantically, move constructor shouldn't throw error (throw std::runtime_error("error"))
    */

    // --------------------------------------------------------------

    // move and assignment
    uniquePtr& operator=(uniquePtr&& aa )noexcept{
        if(this != &other){
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const {return *ptr;}   // be called when writing *obj   (if put * at back, it it will be "multiple")
    // return T& , const means will not change the current obj

    T* operator->() const {return ptr;}   // be called when writing obj->xxx()
    // when you write obj->xxx, cpp will translate it into (obj.operator->())->xxx )

    T* get() const {return ptr;}
    

private:
    T* ptr;
    // if i just print ptr, will see the memory address



};





