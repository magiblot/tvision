#define Uses_TEvent
#include <tvision/tv.h>

#ifdef _TV_UNIX

#include <internal/platform.h>
#include <cstddef>
#include <vector>
#include <queue>
#include <utility>
#include <chrono>
#include <poll.h>
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::vector;
using std::queue;

// These two always have the same length.
vector<FdInputStrategy*> FdInputStrategy::listeners;
vector<struct pollfd> FdInputStrategy::fds;

queue<size_t> FdInputStrategy::ready;

FdInputStrategy::FdInputStrategy() :
    // The default eventGetter is the virtual getEvent function.
    eventGetter([this] (TEvent &ev) { return getEvent(ev); })
{
}

FdInputStrategy::~FdInputStrategy()
{
    deleteListener(this);
}

bool FdInputStrategy::waitForEvent(int ms, TEvent &ev)
{
    bool result {false};
    auto &&end = steady_clock::now() + milliseconds(ms);
    do {
        if (ready.empty() && poll(fds.data(), fds.size(), ms) > 0)
            for (size_t i = 0; i < fds.size(); ++i)
            {
                if (fds[i].revents & POLLHUP)
                    // Broken pipe will cause poll to return immediately, so
                    // remove it from the list.
                    deleteListener(listeners[i]);
                else if (fds[i].revents & POLLIN)
                    ready.push(i);
            }
        if (!ready.empty())
        {
            ev = {};
            size_t i = ready.front(); ready.pop();
            result = listeners[i]->eventGetter(ev);
        }
    } while (!result && steady_clock::now() <= end);
    return result;
}

void FdInputStrategy::addListener(FdInputStrategy* a, int fd)
{
    listeners.push_back(a);
    fds.push_back({ .fd = fd, .events = POLLIN });
}

void FdInputStrategy::deleteListener(FdInputStrategy* listener)
{
    size_t shrink = 0;
    size_t i = 0;

    while (i < listeners.size() - shrink)
    {
        bool found = false;
        if (listeners[i] == listener)
        {
            found = true;
            ++shrink;
        }
        if (shrink && (i + shrink < listeners.size()))
        {
            listeners[i] = std::move(listeners[i + shrink]);
            fds[i] = std::move(fds[i + shrink]);
        }
        if (!found)
            ++i;
    }

    if (shrink)
    {
        ready = {};
        while (shrink--)
        {
            listeners.pop_back();
            fds.pop_back();
        }
    }
}

void FdInputStrategy::overrideEventGetter(std::function<bool (TEvent&)> &&func)
{
    eventGetter = func;
}

#endif // _TV_UNIX
