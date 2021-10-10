#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <stdint.h>
#include <queue>
#include <mutex>

#include "AutoResetEvent.hpp"


namespace ioi
{
namespace ai_center
{
namespace four_ch_dvr
{

enum class DequeueResult
{
    kInvalidForTimeout,
    kValidResult,
    kOnClearingState
};

template <class T>
class SyncQueue
{
public:
    ///
    /// \brief Constructor
    ///
    SyncQueue();
    ///
    /// \brief Destructor
    ///
    ~SyncQueue();
    ///
    /// \brief enQueue adds a new item to current queue.
    ///
    /// \details This function gets a new item and adds it to current queue.
    ///     This function is thread safe.
    ///
    void enQueue(T newItem);
    ///
    /// \brief This function returns the element in front of queue.
    ///
    /// \details This function returns the item that has been placed in
    ///     front of current queue and eliminates it from the queue.If there
    ///     is no item in current queue, this function blocks the
    ///     current thread Until a new item is queued.This function is
    ///     thread safe.
    ///
    /// \param[out] result is a output parameter that will used to return the
    ///     front item in the quqeue.
    /// \return Return value of this function specifies the validation of
    ///     result value.Return values of this function are as fallow:
    ///   - kValidResult: There was at least one item in the queue that was
    ///     successfully removed from the queue.
    ///   - kOnClearingState: Retuerned value is not valid, and caller thread
    ///     was unloked because of clearing operation.
    ///
    DequeueResult deQueue(T &result);
    ///
    /// \brief This function works like the deQueue function, except that it
    ///     has a specific time interval for blocking.
    ///
    /// \details This function returns the item that has been placed in
    ///     front of current queue and eliminates it from the queue.If there
    ///     is no item in current queue, this function locks the current
    ///     thread Until a new item  is queued or the specified timeout has
    ///     expires.This function is thread safe.
    ///
    /// \param[in] timeOut specifies maximum lock time in miliseconds when
    ///     there is no item in the queue.
    /// \param[out] result is a output parameter that will used to return the
    ///     front item in the quqeue.
    /// \return Return value of this function specifies the validation of
    ///     result value.Return values of this function are as fallow:
    ///   - kInvalidForTimeout: Retuerned value is not valid, because timeout
    ///     was occured.
    ///   - kValidResult: There was at least one item in the queue that was
    ///     successfully removed from the queue.
    ///   - kOnClearingState: Retuerned value is not valid, and caller thread
    ///     was unloked because of clearing operation.
    ///
    DequeueResult deQueueWithTimeOut(uint64_t timeOut, T &result);
    ///
    /// \brief This function eliminates all elements in the queue or
    ///      unlocks all waited threads. This function is thread safe.
    ///
    void clear();
    ///
    /// \brief This function returns the number of elelemnts in the
    ///     current queue. This function is thread safe.
    /// \return The number of elements in current queue.
    ///
    uint32_t count();
private:
    ///
    /// \brief mainQueu
    ///
    std::queue<T> mMainQueu;
    ///
    /// \brief This feild Will be used to block threads when there
    ///     is no element in the queue.
    ///
    AutoResetEvent mThreadsSyncHandler;
    ///
    /// \brief This feild will be used to protect critical sections.
    ///
    std::mutex mProtectionMutex;
};
#include "../../src/data_structures/SyncQueue.cpp"
}//end of namespace four_ch_dvr
}//end of namespace ai_center
}//end of namespace ioi
#endif // SYNCQUEUE_H
