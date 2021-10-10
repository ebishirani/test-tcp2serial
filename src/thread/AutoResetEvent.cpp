#include "AutoResetEvent.hpp"

using namespace ioi::ai_center::four_ch_dvr;

AutoResetEvent::AutoResetEvent()
{
    this->mNumOfPermissions = 0;
    this->mNumOfWaitedThreads = 0;
    this->mIsDestroing = false;
    this->mIsResetOccured = false;
}
//***************************************************************************
AutoResetEvent::~AutoResetEvent()
{
    //To change criticall area, it must be locked.Next line do this.
    std::lock_guard<std::mutex> locker(this->mProtectionMutex);
    this->mIsDestroing = true;
    this->mNumOfPermissions = this->mNumOfWaitedThreads;
    this->mConditionalBlock.notify_all();
}
//***************************************************************************
void AutoResetEvent::set()
{
    //To change criticall area, it must be locked.Next line do this.
    std::lock_guard<std::mutex> locker(this->mProtectionMutex);
    //Increment the number of permissions.
    this->mNumOfPermissions++;
    //Check if num of permissions, was zero, If yes, it is possible that another
    //Thread has been done waitOn operation and locked.So, it is necessory to unlock
    //it.In next if structure we do this.
    if (this->mNumOfPermissions == 1)
    {
        //Here we expected to call the notify_one function, but in the tests we
        //performed we found that for the case that there are several locked
        //threads, if the set function is called without interruption, this
        //function does not work properly, so we have to use notify_all
        //function.Although this will release all the locked threads, they
        //will only allow the number of licenses of the released threads to
        //continue, and the rest of them will be locked again in the loop in
        //function waitOnFor.
        this->mConditionalBlock.notify_all();
    }           
}
//***************************************************************************
UnlockStatus AutoResetEvent::waiteOn()
{
    return this->waiteOnFor(0);
}
//***************************************************************************
UnlockStatus AutoResetEvent::waiteOnFor(uint64_t milliSeconds)
{
    UnlockStatus result = UnlockStatus::kNoTimeout;

    std::unique_lock<std::mutex>
        locker(this->mProtectionMutex);
    //To prevent of blocking threads when current module is being distroid.
    if (true == this->mIsDestroing)
    {
        return UnlockStatus::kOnDestroingState;
    }
    else if (true == this->mIsResetOccured)
    {
        return UnlockStatus::kOnResettingState;
    }
    //har zaman ke thread ei vared in tabe mishavad farz bar an ast ke
    //motavaghef mishavad. benabar in numOfWaitedThreads yek vahed afzayesh
    //miyabad.
    this->mNumOfWaitedThreads++;

    if (this->mNumOfPermissions <= 0)
    {
        if (0 >= milliSeconds)
        {
            //While loop is used to avoid spurious wakeups. If spurious wake_
            //ups did not exist, while loop can be replaced with "if" block;
            while (this->mNumOfPermissions <= 0)
            {
                //If there isn't any permission, current thread must be bloc_
                //ked until another one provides a permission and signal this
                //one.
                this->mConditionalBlock.wait(locker);
            }
            result = UnlockStatus::kNoTimeout;
        }
        else
        {
            //While loop is used to avoid spurious wakeups. If spurious wake_
            //ups did not exist, while loop can be replaced with "if" block;
            //In this case current thread must be blocked for specified peri_
            //od of time.So in next line we estimate the end of specified pe_
            //riod from now to use with wait_until function of conditional
            //variable.
            auto endOfTimeInterval =
                std::chrono::system_clock::now() +
                std::chrono::milliseconds(milliSeconds);
            std::cv_status status = std::cv_status::no_timeout;
            while (this->mNumOfPermissions <= 0)
            {
                //If there isn't any permission, current thread must be blocked
                //until another one provides a permission and signal this one or
                //a certain period of time has elapsed.
                status = this->mConditionalBlock.wait_until(
                    locker,
                    endOfTimeInterval);
                //Check if timeout has occurred?
                if (std::cv_status::timeout == status)
                {
                    result = UnlockStatus::kTimeout;
                    break;
                }//end of "if (std::cv_status::timeout == status)"
            }//end of "while (this->mNumOfPermissions <= 0)"
        }//end of else of "if (0 == milliSeconds)"
    }//end of "if (this->mNumOfPermissions <= 0)"

    if (true == this->mIsDestroing)
    {
        result = UnlockStatus::kOnDestroingState;
    }
    else if (true == this->mIsResetOccured)
    {
        result = UnlockStatus::kOnResettingState;
        //After call reset function if all waited threads are freed it is
        //needed to end reset condition.In next if block we do this.
        if (1 == this->mNumOfWaitedThreads)//This thread is last locked one
        {
            this->mIsResetOccured = false;
        }
    }
    //dar soorati ke beliti vojood dashte bashad, thread ejra konandeye function
    //waitOn be in noghte az kod khahad resid.Dar inja bayd numOfPermissions ra
    //yek vahed kam konad(Albate dar soorati ke timeout etefagh nayoftade
    //bashad, dar soorat etefagh timeout ba kaheshe mNumOfPermissions,
    //meghdar in motaghayer manfi mishavad va mojavezhaye badi ra khonsa
    //mikonad), chon yek belit estefade karde az tarafi khodash ham az
    //gate kharej mishavad va bayad numOfWaitedThreads ra yeki kam konad.
    if (result != UnlockStatus::kTimeout)
    {
        this->mNumOfPermissions--;
    }
    this->mNumOfWaitedThreads--;    

    return result;
}
//***************************************************************************
uint32_t AutoResetEvent::getNumOfWaitedThreads()
{
    //To access criticall area, it must be locked.Next line do this.
    std::lock_guard<std::mutex> locker(this->mProtectionMutex);
    return this->mNumOfWaitedThreads;
}
//***************************************************************************
uint32_t AutoResetEvent::getNumOfPermissions()
{
    //To access criticall area, it must be locked.Next line do this.
    std::lock_guard<std::mutex> locker(this->mProtectionMutex);
    return this->mNumOfPermissions;
}
//***************************************************************************
void AutoResetEvent::reset()
{
    //To access criticall area, it must be locked.Next line do this.
    std::lock_guard<std::mutex> locker(this->mProtectionMutex);
    if (this->mNumOfWaitedThreads >= 1)
    {
        this->mIsResetOccured = true;
        this->mNumOfPermissions = this->mNumOfWaitedThreads;
        this->mConditionalBlock.notify_all();
    }
}
