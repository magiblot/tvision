#define Uses_TEvent
#define Uses_TGroup
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TMenuView
#define Uses_TSubMenu
#include <tvision/tv.h>

#include <forward_list>
#include <test.h>
#include <test_operators.h>
#include <test_charops.h>

enum : ushort
{
    cmNoItemChosen = 0,
    cmMenuItemChosen = 1000,
    cmWrongItemChosen = 2000,
};

struct MenuItemsAndEventQueue
{
    TMenuItem &menuItems;
    std::forward_list<TEvent> eventQueue;
};

static std::ostream &operator<<(std::ostream &os, const TMenuItem &menuItem);

static std::ostream &operator<<(std::ostream &os, const TMenu &menu)
{
    os << '{';
    if (menu.items)
        os << *menu.items;
    else
        os << 0;
    os << ", default: ";
    if (menu.deflt)
        os << '\'' << menu.deflt->name << '\'';
    else
        os << 0;
    os << '}';

    return os;
}

static std::ostream &operator<<(std::ostream &os, const TMenuItem &menuItem)
{
    os << '{';
    if (menuItem.command != 0)
        os << '\'' << menuItem.name << "', " << menuItem.command << ", " << menuItem.keyCode << ", " << menuItem.helpCtx << ", '" << TStringView(menuItem.param) << "'";
    else
        os << '\'' << menuItem.name << "', " << menuItem.keyCode << ", " << menuItem.helpCtx << ", " << *menuItem.subMenu;
    os << '}';

    if (menuItem.next)
        os << " + " << *menuItem.next;

    return os;
}

static std::ostream &operator<<(std::ostream &os, const MenuItemsAndEventQueue &menuItemsAndEventQueue)
{
    os << "Menu items: " << menuItemsAndEventQueue.menuItems << std::endl;
    os << "Event queue: " << testing::PrintToString(menuItemsAndEventQueue.eventQueue);
    return os;
}

class TestGroup : public TGroup
{
public:

    TestGroup(std::forward_list<TEvent> aEventQueue) :
        TGroup(TRect()),
        eventQueue(std::move(aEventQueue))
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
            // Force the menu to exit.
            event = messageEv(evCommand, cmNoItemChosen);
    }

    void putEvent(TEvent &event) override
    {
        eventQueue.push_front(event);
    }

private:

    std::forward_list<TEvent> eventQueue;
};

TEST(TMenuView, ShouldSelectMenuItemOnKeyPress)
{
    TestCharOps::init();
    const MenuItemsAndEventQueue inputs[] =
    {
        {   *new TMenuItem("~F~ile", cmMenuItemChosen, kbNoKey),
            {   keyDownEv(kbAltF, kbLeftAlt),
            },
        },
        {   *new TMenuItem("File", cmMenuItemChosen, kbAltF),
            {   keyDownEv(kbAltF, kbLeftAlt),
            },
        },
        {   *new TMenuItem("~Ф~айл", cmMenuItemChosen, kbNoKey),
            {   keyDownEv(kbNoKey, kbLeftAlt, "ф"), // Lowercase.
            },
        },
        {   *new TMenuItem("~Ф~айл", cmMenuItemChosen, kbNoKey),
            {   keyDownEv(kbNoKey, kbLeftAlt, "Ф"), // Uppercase.
            },
        },
        {   *new TMenuItem("~F~ile", cmWrongItemChosen, kbNoKey) +
            *new TMenuItem("~Ф~айл", cmMenuItemChosen, kbNoKey),
            {   keyDownEv(kbAltF, kbLeftAlt, "Ф"),
            },
        },
        {   *new TMenuItem("~Ф~айл", cmMenuItemChosen, kbNoKey) +
            *new TMenuItem("~F~ile", cmWrongItemChosen, kbNoKey),
            {   keyDownEv(kbAltF, kbLeftAlt, "Ф"),
            },
        },
        {   *new TSubMenu("~Ф~айл", 0) +
              *new TMenuItem("В~ы~ход", cmMenuItemChosen, kbNoKey), // Lowercase.
            {   keyDownEv(kbAltF, kbLeftAlt, "Ф"),
                keyDownEv(kbNoKey, 0x0000, "Ы"), // Uppercase.
            },
        },
        {   *new TMenuItem("~\xF0~", cmMenuItemChosen, kbNoKey),
            {   keyDownEv(kbAltSpace, kbLeftAlt),
            },
        },
        {   *new TMenuItem("~≡~", cmMenuItemChosen, kbNoKey),
            {   keyDownEv(kbAltSpace, kbLeftAlt),
            },
        },
    };

    for (const auto &input : inputs)
    {
        TestCase<MenuItemsAndEventQueue, TEvent> testCase {
            input,
            messageEv(evCommand, cmMenuItemChosen),
        };

        TMenu menu(input.menuItems);
        auto *menuView = new TMenuView(TRect(), &menu, nullptr);
        menuView->options |= ofPreProcess;

        auto *group = new TestGroup(input.eventQueue);
        group->insert(menuView);
        menuView->select();

        TEvent inputEvent {};
        group->getEvent(inputEvent);
        group->handleEvent(inputEvent);

        TEvent resultEvent {};
        group->getEvent(resultEvent);

        expectResultMatches(resultEvent, testCase);

        TObject::destroy(group);
    }
}

TEST(TMenuView, ShouldNotSelectMenuItemOnKeyPress)
{
    TestCharOps::init();
    const MenuItemsAndEventQueue inputs[] =
    {
        {   *new TSubMenu("~F~ile", 0) +
              *new TMenuItem("Exit", cmMenuItemChosen, kbNoKey), // No shortcut.
            {   keyDownEv(kbAltF, kbLeftAlt),
                keyDownEv(kbNoKey, 0x0000), // No text.
            },
        },
    };

    for (const auto &input : inputs)
    {
        TestCase<MenuItemsAndEventQueue, TEvent> testCase {
            input,
            messageEv(evCommand, cmNoItemChosen),
        };

        TMenu menu(input.menuItems);
        auto *menuView = new TMenuView(TRect(), &menu, nullptr);
        menuView->options |= ofPreProcess;

        auto *group = new TestGroup(input.eventQueue);
        group->insert(menuView);
        menuView->select();

        TEvent inputEvent {};
        group->getEvent(inputEvent);
        group->handleEvent(inputEvent);

        TEvent resultEvent {};
        group->getEvent(resultEvent);

        expectResultMatches(resultEvent, testCase);

        TObject::destroy(group);
    }
}
