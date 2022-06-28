#pragma once
#include <queue>
#include <3ds.h>

template<typename T>
class ThreadSafeQueue{
    public:
        ThreadSafeQueue(){
            m_queue = {};
            LightLock_Init(&m_lock);
        }
        
        ~ThreadSafeQueue(){
        }
        
        void Enqueue(T t){
            LightLock_Lock(&m_lock);
            m_queue.push(t);
            LightLock_Unlock(&m_lock);
        }

        T Dequeue(){
            LightLock_Lock(&m_lock);
            T val = m_queue.front();
            m_queue.pop();
            LightLock_Unlock(&m_lock);
            return val;
        }

        bool Empty(){
            LightLock_Lock(&m_lock);
            bool v = m_queue.empty();
            LightLock_Unlock(&m_lock);
            return v;
        }

        size_t Size(){
            LightLock_Lock(&m_lock);
            auto s = m_queue.size();
            LightLock_Unlock(&m_lock);
            return s;
        }

    private:
        LightLock m_lock;
        std::queue<T> m_queue;
};