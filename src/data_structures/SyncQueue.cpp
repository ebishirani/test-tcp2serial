#include "SyncQueue.hpp"

using namespace ioi::ai_center::four_ch_dvr;
//Next function is used to extract Underlying container of modules such as
//queue.
template<class ADAPTER>
typename ADAPTER::container_type& get_container(ADAPTER& a)
{
    struct hack : ADAPTER
    {
        static typename ADAPTER::container_type& get(ADAPTER& a)
        {
            return a .* &hack::c;
        }
    };
    return hack::get(a);
}
//***************************************************************************
template <class T>
SyncQueue<T>::SyncQueue()
{    
}
//***************************************************************************
template <class T>
SyncQueue<T>::~SyncQueue()
{
    this->clear();
}
//***************************************************************************
template <class T>
void SyncQueue<T>::enQueue(T newItem)
{
    {
        //To provide thread safety next lock is used.
        std::lock_guard<std::mutex> lock(this->mProtectionMutex);
        this->mMainQueu.push(newItem);
    }
    this->mThreadsSyncHandler.set();
}
//***************************************************************************
template <class T>
DequeueResult SyncQueue<T>::deQueue(T &result)
{
    return this->deQueueWithTimeOut(0, result);
}
//***************************************************************************
template <class T>
DequeueResult SyncQueue<T>::deQueueWithTimeOut(uint64_t timeOut, T &result)
{
    UnlockStatus status = UnlockStatus::kNoTimeout;

    DequeueResult dequeueResult = DequeueResult::kOnClearingState;
    if (0 >= timeOut)
    {
        //If there is no item in the queue, current tread must be locked.Next
        //command do this for us.
        status = this->mThreadsSyncHandler.waiteOn();
    }
    else
    {
        //If there is no item in the queue, current tread must be locked.Next
        //command do this for us.
        status = this->mThreadsSyncHandler.waiteOnFor(timeOut);
    }
    result = T();
    if (UnlockStatus::kNoTimeout == status)
    {
        //To provide thread safety next lock is used.
        std::lock_guard<std::mutex> lock(this->mProtectionMutex);
        result = this->mMainQueu.front();
        this->mMainQueu.pop();
        dequeueResult = DequeueResult::kValidResult;
    }
    else if(UnlockStatus::kTimeout == status)
    {
        dequeueResult = DequeueResult::kInvalidForTimeout;
    }
    return dequeueResult;
}
//***************************************************************************
template <class T>
uint32_t SyncQueue<T>::count()
{
    //To provide thread safety next lock is used.
    std::lock_guard<std::mutex> lock(this->mProtectionMutex);
    return this->mMainQueu.size();
}
//***************************************************************************
template <class T>
void SyncQueue<T>::clear()
{
    //To provide thread safety next lock is used.
    std::lock_guard<std::mutex> lock(this->mProtectionMutex);
    this->mThreadsSyncHandler.reset();
    get_container(this->mMainQueu).clear();
}
//***************************************************************************
