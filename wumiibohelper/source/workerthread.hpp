#pragma once
#include <functional>
#include <3ds.h>

template<typename ... Ts>
class WorkerThread{
    public:
        WorkerThread() {
            m_done = true;
            m_threadhandle = nullptr;
        }

        void CreateThread(std::function<void(Ts...)> cb, Ts& ... args, size_t stacksize = 64 * 1024){
            ThreadFuncParams *params = new ThreadFuncParams;
            params->func = [&]() -> void { cb(args...); };
            params->self = this;

            s32 prio;
            svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);

            m_done = false;
            m_threadhandle = threadCreate(&WorkerThread::_entrypoint, params, stacksize, prio - 1, -2, false);
        }

        ~WorkerThread(){
            Join();
            //threadFree(m_threadhandle);
        }

        void Join(){
            if(!IsDone())
                threadJoin(m_threadhandle, U64_MAX);
        }

        bool IsDone(){
            return m_done;
        }

    private:
        struct ThreadFuncParams{
            std::function<void()> func;
            WorkerThread *self;
        };

        static void _entrypoint(void *arg){
            ThreadFuncParams *params = (ThreadFuncParams*) arg;
            params->func();
            params->self->m_done = true;
            threadFree(params->self->m_threadhandle);
            delete params;
        }

        Thread m_threadhandle;
        bool m_done;
};