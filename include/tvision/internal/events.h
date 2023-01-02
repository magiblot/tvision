#ifndef TVISION_EVENTS_H
#define TVISION_EVENTS_H

#define Uses_TEvent
#include <tvision/tv.h>
#include <atomic>
#include <memory>
#include <vector>

#ifdef _TV_UNIX
#include <poll.h>
#else
#include <tvision/compat/windows/windows.h>
#endif

namespace tvision
{

#ifdef _TV_UNIX
using SysHandle = int;
#else
using SysHandle = HANDLE;
#endif

struct SysManualEvent
{
#ifdef _TV_UNIX
    using Handle = int[2];
    Handle fds;
#else
    using Handle = HANDLE;
    Handle hEvent;
#endif

    static bool createHandle(Handle &handle) noexcept;
    static SysHandle getWaitableHandle(Handle handle) noexcept;

    SysManualEvent(Handle aHandle) noexcept;
    ~SysManualEvent();
    void signal() noexcept;
    void clear() noexcept;
};

inline SysManualEvent::SysManualEvent(Handle aHandle) noexcept :
#ifdef _TV_UNIX
    fds {aHandle[0], aHandle[1]}
#else
    hEvent {aHandle}
#endif
{
}

inline SysHandle SysManualEvent::getWaitableHandle(Handle handle) noexcept
{
#ifdef _TV_UNIX
    return handle[0];
#else
    return handle;
#endif
}

class EventSource
{
public:

    const SysHandle handle;

    EventSource(SysHandle handle) noexcept :
        handle(handle)
    {
    }

    virtual ~EventSource() {}

    virtual bool hasPendingEvents() noexcept;
    virtual bool getEvent(TEvent &) noexcept;
};

class WakeUpEventSource : public EventSource
{
    SysManualEvent sys;
    bool (*callback) (void *, TEvent &) noexcept;
    void *callbackArgs;
    std::atomic<bool> signaled {false};

    bool clear() noexcept;

public:

    // Pre: if 'callback' or 'callbackArgs' are not null, their lifetime must
    // exceed that of 'this'. 'callback' must be noexcept.
    WakeUpEventSource( SysManualEvent::Handle aHandle,
                       bool (*aCallback) (void *, TEvent &),
                       void *aCallbackArgs ) noexcept;

    WakeUpEventSource &operator=(const WakeUpEventSource &) = delete;

    void signal() noexcept; // Multiple producers.
    bool getEvent(TEvent &event) noexcept override; // Single consumer.
};

inline WakeUpEventSource::WakeUpEventSource( SysManualEvent::Handle aHandle,
                                             bool (*aCallback) (void *, TEvent &),
                                             void *aCallbackArgs ) noexcept :
    EventSource(SysManualEvent::getWaitableHandle(aHandle)),
    sys(aHandle),
    callback((bool (*)(void *, TEvent &) noexcept) aCallback),
    callbackArgs(aCallbackArgs)
{
}

#ifdef _TV_UNIX
using PollItem = struct pollfd;
static inline PollItem pollItem(SysHandle fd) noexcept { return {fd, POLLIN}; }
#else
using PollItem = HANDLE;
static inline PollItem pollItem(SysHandle h) noexcept { return h; }
#endif

enum PollState : uint8_t
{
    psNothing,
    psReady,
    psDisconnect,
};

struct PollData
{
    std::vector<PollItem> items;
    std::vector<PollState> states;

    void push_back(SysHandle h)
    {
        items.push_back(pollItem(h));
        states.push_back(psNothing);
    }

    void erase(size_t i)
    {
        items.erase(items.begin() + i);
        states.erase(states.begin() + i);
    }

    size_t size()
    {
        return items.size();
    }
};

class EventWaiter
{
    std::vector<EventSource *> sources;
    PollData pd;
    std::unique_ptr<WakeUpEventSource> wakeUp {nullptr};
    TEvent readyEvent;
    bool readyEventPresent {false};

    void removeSource(size_t i) noexcept;
    void pollSources(int timeoutMs) noexcept;
    bool hasReadyEvent() noexcept;
    void getReadyEvent(TEvent &ev) noexcept;

public:

    EventWaiter() noexcept;

    void addSource(EventSource &) noexcept;
    void removeSource(EventSource &) noexcept;

    bool getEvent(TEvent &ev) noexcept;
    void waitForEvent(int ms) noexcept;
    void stopEventWait() noexcept;
};

} // namespace tvision

#endif // TVISION_EVENTS_H
