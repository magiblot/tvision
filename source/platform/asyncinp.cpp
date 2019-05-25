#define Uses_TEvent
#include <tvision/tv.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <internal/platform.h>
using std::thread;
using std::mutex;
using std::unique_lock;
using std::lock_guard;
using std::condition_variable;
using std::list;
using std::chrono::milliseconds;
using waiter = AsyncInputStrategy::waiter;

/* AsyncInputStrategy allows for capturing input events in separate threads,
 * and forwarding them asynchronously to the main thread. It serves a
 * 'several producers, one consumer' model.
 *
 * The idea is that each AsyncInputStrategy instance registers itself in a list
 * of input listeners upon construction. Later, when the main thread calls
 * 'resumeListening' for the first time, a thread for each listener will be
 * started. When it is ready to receive events, the main thread must
 * call 'waitForEvent', which times out after 'ms' milliseconds.
 *
 * On the other side, listener threads must call 'notifyEvent' when ready, which
 * will keep them blocked until 'resumeListening' is called from the main thread.
 * This is to prevent data races between the main thread and the listeners,
 * since the libraries used for input and display may not be thread-safe
 * (as is the case of ncurses).
 *
 * The code below prevents any events from being lost between the main thread
 * and the listeners, as well as 'resume' signals from the main thread. It will
 * behave in a deadlock-proof manner if used properly.
 *
 * The advantages of this design are that each listener has the same priority
 * and that all input events may be treated as soon as the main thread can read
 * them.
 *
 * The downside is that there might not be a simple way to terminate the
 * listener threads properly. In addition, listener threads might not receive
 * signals such as SIGWINCH. */

/* List of listeners whose thread will be started the first time 'resumeListening'
 * is executed. */
list<AsyncInputStrategy*> AsyncInputStrategy::listeners;
/* Pointer to the mutex+condition_variable of the listener thread waiting
 * for the main thread to call 'resumeListening'. */
waiter *AsyncInputStrategy::inputListener;
/* mutex+condition_variable representing the main thread, which waits for
 * an event to be received. */
waiter AsyncInputStrategy::eventRequester;
// Mutex to prevent several listeners from notifying an event at the same time.
std::mutex AsyncInputStrategy::notifying;

// Did any listener receive an event?
bool AsyncInputStrategy::evReceived = false;
// Did the main thread finish processing the last event?
bool AsyncInputStrategy::evProcessed = true;
// The last received event.
TEvent AsyncInputStrategy::received;

AsyncInputStrategy::AsyncInputStrategy() :
    // The default eventGetter is the virtual getEvent function.
    eventGetter([this] (TEvent &ev) { return getEvent(ev); })
{
    // Register itself in the list of listeners.
    listeners.push_back(this);
}

AsyncInputStrategy::~AsyncInputStrategy()
{
    /* If there's no way to terminate the input thread, it must be
     * detached before becoming out of scope. */
    inputThread.detach();
}

bool AsyncInputStrategy::waitForEvent(long ms, TEvent &ev)
{
    unique_lock<mutex> rlk(eventRequester.m);
    return eventRequester.cv.wait_for(rlk, milliseconds(ms), [&] {
        bool b = evReceived;
        if (b)
        {
            evReceived = false;
            ev = received;
        }
        return b;
    });
}

void AsyncInputStrategy::resumeListening()
{
    static bool firstTime = true;
    if (!evProcessed)
    {
        {
            lock_guard<mutex> ilk(inputListener->m);
            evProcessed = true;
        }
        inputListener->cv.notify_one();
    }
    else if (firstTime)
    {
        firstTime = false;
        for (auto listener : listeners)
            listener->startInputThread();
    }
}

void AsyncInputStrategy::notifyEvent(TEvent &ev, waiter &inp)
{
    lock_guard<mutex> nlk(notifying);
    unique_lock<mutex> ilk(inp.m);
    {
        lock_guard<mutex> rlk(eventRequester.m);
        evReceived = true;
        received = ev;
    }
    inputListener = &inp;
    evProcessed = false;
    eventRequester.cv.notify_one();
    inp.cv.wait(ilk, [&] {
        bool b = evProcessed;
        if (b)
            evProcessed = false;
        return b;
    });
}

void AsyncInputStrategy::startInputThread()
{
    inputThread = thread([this] {
        waiter w;
        while (true)
        {
            TEvent ev = {};
            if (eventGetter(ev))
                notifyEvent(ev, w);
        }
    });
}

void AsyncInputStrategy::overrideEventGetter(std::function<bool (TEvent&)> &&func)
{
    eventGetter = func;
}
