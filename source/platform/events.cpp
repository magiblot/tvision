#include <internal/events.h>
#include <chrono>
using time_point = std::chrono::steady_clock::time_point;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

#ifdef _TV_UNIX
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#ifdef __APPLE__
#include <sys/select.h>
#endif
#endif

namespace tvision
{

/////////////////////////////////////////////////////////////////////////
// SysManualEvent

#ifdef _TV_UNIX

bool SysManualEvent::createHandle(int (&fds)[2]) noexcept
{
    if (pipe(fds) != -1)
    {
        for (int fd : fds)
            fcntl(fd, F_SETFD, FD_CLOEXEC);
        return true;
    }
    return false;
}

SysManualEvent::~SysManualEvent()
{
    close(fds[0]);
    close(fds[1]);
}

void SysManualEvent::signal() noexcept
{
    char c = 0;
    while (write(fds[1], &c, 1) == 0);
}

void SysManualEvent::clear() noexcept
{
    char c;
    while (read(fds[0], &c, sizeof(char)) == 0);
}

#else

bool SysManualEvent::createHandle(HANDLE &hEvent) noexcept
{
    return (hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr)) != NULL;
}

SysManualEvent::~SysManualEvent()
{
    CloseHandle(hEvent);
}

void SysManualEvent::signal() noexcept
{
    SetEvent(hEvent);
}

void SysManualEvent::clear() noexcept
{
    ResetEvent(hEvent);
}

#endif

/////////////////////////////////////////////////////////////////////////
// EventSource

bool EventSource::hasPendingEvents() noexcept
{
    return false;
}

bool EventSource::getEvent(TEvent &) noexcept
{
    return false;
}

/////////////////////////////////////////////////////////////////////////
// WakeUpEventSource

void WakeUpEventSource::signal() noexcept
{
    if (signaled.exchange(true) == false)
    {
        sys.signal();
    }
}

bool WakeUpEventSource::clear() noexcept
{
    if (signaled)
    {
        sys.clear();
        signaled = false;
        return true;
    }
    return false;
}

bool WakeUpEventSource::getEvent(TEvent &ev) noexcept
{
    if (clear())
    {
        if (callback)
            return callback(callbackArgs, ev);
        ev.what = evNothing;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////
// EventWaiter

#ifdef _TV_UNIX

static bool fdEmpty(int fd) noexcept
{
    int nbytes;
    return ioctl(fd, FIONREAD, &nbytes) == -1 || !nbytes;
}

#ifdef __APPLE__

// With '_DARWIN_UNLIMITED_SELECT', which the build defines on macOS, 'select'
// accepts file descriptors from 'FD_SETSIZE' on in a set made of several
// 'fd_set'. Without it, they are not waited for. Negative ones never are.
static bool fdSelectable(int fd) noexcept
{
#ifdef _DARWIN_UNLIMITED_SELECT
    return 0 <= fd;
#else
    return 0 <= fd && fd < FD_SETSIZE;
#endif
}

static void pollHandles(PollData &pd, int ms) noexcept
{
    auto &fds = pd.handles;
    auto &states = pd.states;
    // We use 'select' instead of 'poll' on macOS because 'poll' does not work
    // with terminal devices in some of its versions (see magiblot/turbo#65).
    int maxFd = -1;
    for (size_t i = 0; i < fds.size(); ++i)
        if (fdSelectable(fds[i]) && fds[i] > maxFd)
            maxFd = fds[i];

    if (maxFd >= 0)
    {
        // One 'fd_set' is enough unless a descriptor is past it.
        fd_set single;
        std::vector<fd_set> several;
        fd_set *readFds = &single;
        FD_ZERO(&single);
        if (maxFd >= FD_SETSIZE)
        {
            several.resize(maxFd/FD_SETSIZE + 1);
            readFds = several.data();
        }
        for (size_t i = 0; i < fds.size(); ++i)
            if (fdSelectable(fds[i]))
                FD_SET(fds[i], readFds);

        struct timeval timeout;
        timeout.tv_sec = ms/1000;
        timeout.tv_usec = ms%1000*1000;
        if ( select( maxFd + 1, readFds, nullptr, nullptr,
                     (ms < 0 ? nullptr : &timeout) ) >= 0 )
            for (size_t i = 0; i < fds.size(); ++i)
                if (fdSelectable(fds[i]) && FD_ISSET(fds[i], readFds))
                {
                    if (fdEmpty(fds[i]))
                        states[i] = psDisconnect;
                    else
                        states[i] = psReady;
                }
    }
}

#else

static void pollHandles(PollData &pd, int ms) noexcept
{
    auto &fds = pd.handles;
    auto &states = pd.states;
    // Unlike 'select', 'poll' supports file descriptors of any number, which
    // matters when the application has opened many files before the terminal.
    // Negative ones are not waited for.
    auto &items = pd.items;
    items.resize(fds.size());
    for (size_t i = 0; i < fds.size(); ++i)
    {
        items[i].fd = fds[i];
        items[i].events = POLLIN;
    }

    if (poll(items.data(), items.size(), ms) > 0)
        for (size_t i = 0; i < items.size(); ++i)
        {
            if ( (items[i].revents & POLLHUP) ||
                 ((items[i].revents & POLLIN) && fdEmpty(fds[i])) )
                // Broken pipe or EOF will cause poll to return immediately,
                // so remove it from the list.
                states[i] = psDisconnect;
            else if (items[i].revents & POLLIN)
                states[i] = psReady;
        }
}

#endif // __APPLE__

#else

static void pollHandles(PollData &pd, int ms) noexcept
{
    auto &handles = pd.handles;
    auto &states = pd.states;
    DWORD timeout = ms < 0 ? INFINITE : ms;
    if (handles.size() == 0)
        Sleep(timeout);
    else
    {
        DWORD res = WaitForMultipleObjects(handles.size(), &handles[0], false, timeout);
        size_t i = 0;
        while (WAIT_OBJECT_0 <= res && res <= WAIT_OBJECT_0 + handles.size() - i - 1)
        {
            i += res - WAIT_OBJECT_0;
            states[i] = psReady;
            if (++i < handles.size())
                res = WaitForMultipleObjects(handles.size() - i, &handles[i], false, 0);
            else
                break;
        }
    }
}

#endif // _TV_UNIX

EventWaiter::EventWaiter() noexcept
{
    SysManualEvent::Handle handle;
    if (SysManualEvent::createHandle(handle))
    {
        wakeUp.reset(new WakeUpEventSource(handle, nullptr, nullptr));
        addSource(*wakeUp);
    }
}

void EventWaiter::addSource(EventSource &src) noexcept
{
    sources.push_back(&src);
    pd.push_back(src.handle);
}

void EventWaiter::removeSource(EventSource &src) noexcept
{
    for (size_t i = 0; i < sources.size(); ++i)
        if (sources[i] == &src)
        {
            removeSource(i);
            break;
        }
}

inline void EventWaiter::removeSource(size_t i) noexcept
{
    sources.erase(sources.begin() + i);
    pd.erase(i);
}

void EventWaiter::pollSources(int timeoutMs) noexcept
{
    bool hasEvents = false;
    for (size_t i = 0; i < pd.size(); ++i)
        if ((pd.states[i] = sources[i]->hasPendingEvents() ? psReady : psNothing))
            hasEvents = true;
    if (!hasEvents)
    {
        pollHandles(pd, timeoutMs);
        for (size_t i = 0; i < pd.size(); ++i)
            if (pd.states[i] == psDisconnect)
                removeSource(i--);
    }
}

bool EventWaiter::hasReadyEvent() noexcept
{
    if (!readyEventPresent)
        for (size_t i = 0; i < pd.size(); ++i)
            if (pd.states[i] == psReady)
            {
                pd.states[i] = psNothing;
                readyEvent = {};
                if (sources[i]->getEvent(readyEvent))
                {
                    readyEventPresent = true;
                    break;
                }
            }
    return readyEventPresent;
}

inline void EventWaiter::getReadyEvent(TEvent &ev) noexcept
{
    ev = readyEvent;
    readyEventPresent = false;
}

static int pollDelayMs(time_point now, time_point end) noexcept
{
    return max(duration_cast<milliseconds>(nanoseconds(999999) + end - now).count(), 0);
}

bool EventWaiter::getEvent(TEvent &ev) noexcept
{
    if (hasReadyEvent() || (pollSources(0), hasReadyEvent()))
    {
        getReadyEvent(ev);
        return true;
    }
    return false;
}

void EventWaiter::waitForEvents(int ms) noexcept
{
    auto now = steady_clock::now();
    const auto end = ms < 0 ? time_point::max() : now + milliseconds(ms);
    while (!hasReadyEvent() && now <= end)
    {
        pollSources(ms < 0 ? -1 : pollDelayMs(now, end));
        now = steady_clock::now();
    }
}

void EventWaiter::interruptEventWait() noexcept
{
    if (wakeUp)
        wakeUp->signal();
}

} // namespace tvision
