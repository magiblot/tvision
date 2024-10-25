#define Uses_TTimerQueue
#define Uses_THardwareInfo
#include <tvision/tv.h>

static TTimePoint systemTimeMs()
{
#if !defined( __FLAT__ )
    return THardwareInfo::getTickCount()*55;
#elif defined( __BORLANDC__ )
    return GetTickCount();
#else
    return GetTickCount64();
#endif
}

struct TTimer
{
    TTimePoint expiresAt;
    int32_t period;
    TTimer *next;
    void *collectId;
};

TTimerQueue::TTimerQueue() noexcept :
    getTimeMs(systemTimeMs),
    first(0)
{
}

TTimerQueue::TTimerQueue(TTimePoint (&aGetTimeMs)()) noexcept :
    getTimeMs(aGetTimeMs),
    first(0)
{
}

TTimerQueue::~TTimerQueue()
{
    TTimer *timer = first;
    while (timer != 0)
    {
        TTimer *next = timer->next;
        delete timer;
        timer = next;
    }
    first = 0;
}

TTimerId TTimerQueue::setTimer(uint32_t timeoutMs, int32_t periodMs)
{
    TTimer *timer = new TTimer;
    memset(timer, 0, sizeof(TTimer));
    timer->expiresAt = getTimeMs() + timeoutMs;
    timer->period = periodMs;

    TTimer **p = &first;
    while (*p != 0)
        p = &(*p)->next;
    *p = timer;

    return timer;
}

void TTimerQueue::killTimer(TTimerId id)
{
    TTimer **p = &first;
    TTimer *timer = (TTimer *) id;
    while (*p != 0)
    {
        if (*p == timer)
        {
            *p = timer->next;
            delete timer;
            break;
        }
        p = &(*p)->next;
    }
}

static TTimePoint calcNextExpiresAt(TTimePoint expiresAt, TTimePoint now, int32_t period)
// Pre: expiresAt <= now && period > 0.
{
    return (1 + (now - expiresAt + period)/period)*period + expiresAt - period;
}

void TTimerQueue::collectExpiredTimers(void (&func)(TTimerId, void *), void *args)
{
    if (first == 0)
        return;

    // Given that the timer list may be mutated while we process expired timers,
    // we iterate it from the beginning every time. In order to know which
    // timers have already been processed, we mark them with a 'collectId'
    // which identifies the current invocation of 'collectExpiredTimers'.
    void *collectId = &collectId;
    TTimePoint now = getTimeMs();
    while (True)
    {
        TTimer **p = &first;
        while (*p != 0 && ((*p)->collectId != 0 || now < (*p)->expiresAt))
            p = &(*p)->next;
        if (*p == 0)
            break;

        TTimerId id = *p;
        if ((*p)->period >= 0)
        {
            (*p)->collectId = collectId;
            if ((*p)->period > 0)
                (*p)->expiresAt = calcNextExpiresAt((*p)->expiresAt, now, (*p)->period);
        }
        else // One-shot timer
        {
            TTimer *next = (*p)->next;
            delete *p;
            *p = next;
        }

        func(id, args); // May mutate the timer list.
    }

    TTimer *timer = first;
    while (timer != 0)
    {
        if (timer->collectId == collectId)
            timer->collectId = 0;
        timer = timer->next;
    }
}

int32_t TTimerQueue::timeUntilNextTimeout()
{
    if (first == 0)
        return -1;
    TTimePoint now = getTimeMs();
    uint32_t maxValue = uint32_t(-1) >> 1;
    int32_t timeout = maxValue;
    TTimer *timer = first;
    do
    {
        if (timer->expiresAt <= now)
        {
            timeout = 0;
            break;
        }
        uint32_t aTimeout = min(uint32_t(timer->expiresAt - now), maxValue);
        timeout = min(timeout, (int32_t) aTimeout);
        timer = timer->next;
    }
    while (timer != 0);
    return timeout;
}
