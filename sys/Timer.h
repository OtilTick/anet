
#ifndef SYS_TIMER_H
#define SYS_TIMER_H
#include <set>
#include <map>
#include "Shared.h"
#include "Thread.h"
#include "Monitor.h"
#include "Time.h"
namespace util
{
class Timer;
typedef Handle<Timer> TimerPtr;

class TimerTask : virtual public Shared
{
public:

virtual ~TimerTask() { }

virtual void runTimerTask() = 0;
};
typedef Handle<TimerTask> TimerTaskPtr;

class Timer :public virtual Shared ,private virtual util::Thread
{
public:

Timer();

void destroy();

int schedule(const TimerTaskPtr& task, const Time& delay);

int scheduleRepeated(const TimerTaskPtr& task, const Time& delay);

bool cancel(const TimerTaskPtr&);

private:

struct Token
{
Time scheduledTime;
Time delay;
TimerTaskPtr task;

inline Token(const Time&, const Time&, const TimerTaskPtr&);
inline bool operator<(const Token& r) const;
};

virtual void run();

Monitor<Mutex> _monitor;
bool _destroyed;
std::set<Token> _tokens;

class TimerTaskCompare : public std::binary_function<TimerTaskPtr, TimerTaskPtr, bool>
{
public:

bool operator()(const TimerTaskPtr& lhs, const TimerTaskPtr& rhs) const
{
return lhs.get() < rhs.get();
}
};
std::map<TimerTaskPtr, Time, TimerTaskCompare> _tasks;
Time _wakeUpTime;
};
typedef Handle<Timer> TimerPtr;

inline
Timer::Token::Token(const Time& st, const Time& d, const TimerTaskPtr& t) :
scheduledTime(st), delay(d), task(t)
{
}

inline bool
Timer::Token::operator<(const Timer::Token& r) const
{
if(scheduledTime < r.scheduledTime)
{
return true;
}
else if(scheduledTime > r.scheduledTime)
{
return false;
}

return task.get() < r.task.get();
}

}

#endif
