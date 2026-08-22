#ifndef TVISION_TEST_GROUP_H
#define TVISION_TEST_GROUP_H

#define Uses_TEvent
#define Uses_TGroup
#include <tvision/tv.h>

#include <forward_list>
#include <test_operators.h>

// A TGroup whose getEvent()/putEvent() are backed by a queue, so that code
// relying on event loops can be driven deterministically in tests. Once the
// queue is exhausted, 'fallbackEvent' is returned on every subsequent getEvent().
class TestGroup : public TGroup
{
public:

    TestGroup( std::forward_list<TEvent> aEventQueue = {},
               TEvent aFallbackEvent = {} ) :
        TGroup(TRect()),
        eventQueue(std::move(aEventQueue)),
        fallbackEvent(aFallbackEvent)
    {
    }

    void getEvent(TEvent &event) override
    {
        if (!eventQueue.empty())
        {
            event = eventQueue.front();
            eventQueue.pop_front();
        }
        else
            event = fallbackEvent;
    }

    void putEvent(TEvent &event) override
    {
        eventQueue.push_front(event);
    }

private:

    std::forward_list<TEvent> eventQueue;
    TEvent fallbackEvent;
};

#endif // TVISION_TEST_GROUP_H
