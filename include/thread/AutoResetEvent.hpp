#ifndef AUTORESETEVENT_H
#define AUTORESETEVENT_H

#include <stdint.h>
#include <mutex>
#include <condition_variable>

namespace ioi
{
namespace ai_center
{
namespace four_ch_dvr
{

enum class UnlockStatus
{
    kTimeout,
    kNoTimeout,
    kOnResettingState,
    kOnDestroingState
};

class AutoResetEvent
{
public:
    AutoResetEvent();

    ~AutoResetEvent();
    ///
    /// \brief set: This function registers a permission to allow a thread
    /// pass lock mechanism.
    ///
    void set();
    ///
    /// \brief waiteOn locks current thread until a permission is provided.
    ///
    /// \details This function checks registered permisions and if there
    ///      isn't any permision locks curent thread until a permission is
    ///      provided.
    /// \return This function returns kNoTimeout if the caller thread gets a
    ///     permission and passes the thread lock.In destroing or reset modes
    ///     proper values will be returnd.
    ///
    UnlockStatus waiteOn();
    ///
    /// \brief waiteOnFor locks current thread until a permission is provided
    ///     or a timeout is reached.
    ///
    /// \details This function operates like waiteOn function but for a
    ///     specified period of time.After the specified time has elapsed, if
    ///     any permission has not been registered, the locked thread will be
    ///     unlocked and this function returns the ETIMEDOUT value.
    ///
    /// \param miliSeconds is a time period in miliseconds that specifies
    ///     max duration of time that a thread will be locked to get a
    ///     permission If the value of this parameter equals 0, this function
    ///     operate exactlly like waitOn function.
    /// \return This function returns kNoTimeout if the caller thread gets a
    ///     permission and passes the thread lock or kTimeout if the caller
    ///     thread does not get a permission and reaches the end of specified
    ///     time.In destroing or reset modes proper values will be returnd.
    ///
    UnlockStatus waiteOnFor(uint64_t milliSeconds);
    ///
    /// \brief getNumOfWaitedThreads returns the number of waited threads
    /// that wait to get permission.
    /// \return This function return a positive value that indecates the
    /// number of locked threads. This function is thread safe
    ///
    uint32_t getNumOfWaitedThreads();
    ///
    /// \brief getNumOfPermissions returns the number of registered permiss_
    /// ios.This function is thread safe
    /// \return This function returns the number of registered permissions.
    ///
    uint32_t getNumOfPermissions();
    ///
    /// \brief reset wakes up all waited threads, and reset all parameters of
    /// AutoResetEvent module.
    ///
    void reset();

private:
    ///
    /// \brief protectionMutex is used to provide thread safety.
    ///
    /// \details Some fields of AutoResetEvent module will be modified
    /// by multiple threads.To provide thred safty next mutex module will be
    /// used in related functions.
    ///
    ///
    std::mutex mProtectionMutex;
    ///
    /// \brief conditionalBlock: This module will be used to block threads
    /// for a specified time or until another thread emit a signal.
    ///
    std::condition_variable mConditionalBlock;

    int mNumOfPermissions;
    int mNumOfWaitedThreads;
    ///
    /// \brief This field is used to identify if current object is being
    /// distroed.In distructor this variable will be set to true.
    ///
    bool mIsDestroing;
    ///
    /// \brief This field is used to identify if user call reset function?
    ///     In this situation waitON function must returns kResetOccured
    ///     to notify user.
    ///
    bool mIsResetOccured;
};
}//end of namespace four_ch_dvr
}//end of namespace ai_center
}//End of namespace ioi
#endif // AUTORESETEVENT_H
