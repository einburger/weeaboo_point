#pragma once
#include <list>
#include <memory> // unique_ptr

struct BaseEvent { 
    virtual bool on_going() = 0; 
    virtual ~BaseEvent() = default;
};

/*
template <class T>
struct Event : BaseEvent {
    T obj;

    Event() = default;
    Event(const T &obj) : obj(obj) {}

    // return false if event completed
    bool on_going() override {
        return obj.do_action();
    }
};
*/

struct EventHandler {
    EventHandler() = default;

    std::list<std::unique_ptr<BaseEvent>> events;

    template<class T>
    void push_back(T&& event) {
        events.emplace_back(event);
    }

	void process();
};

extern std::unique_ptr<EventHandler> event_handler;

