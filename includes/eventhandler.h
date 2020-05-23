#pragma once
#include <string>
#include <list>
#include <memory> // unique_ptr

#include "globals.h" 
#include "geometry.h" // character/box
#include "parser.h" // parser
#include "text.h" // text_write_animation

struct BaseEvent { 
    virtual bool on_going() = 0; 
    virtual ~BaseEvent() = default;
};

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

// Event<DialogEvent>(string);
struct DialogEvent {
    std::string dialog;
    DialogEvent(const std::string &dialog) : dialog(dialog) {}
    bool do_action() {
        return text_write_animation(dialog.c_str());
    }
};

// Event<CharacterMoveEvent>(ch);
struct CharacterMoveEvent {
    Character &ch;
    CharacterMoveEvent(Character &ch) : ch(ch) {}
    bool do_action() {
        return ch.move(ch.target_pos, ch.min_xy[1], ch.speed);
    }
};

struct WaitEvent {
    WaitEvent() = default;
    bool do_action() {
        return false;
    }
};

struct ParseEvent { 
    bool do_action() { return parse(); } 
};

struct FadeInEvent {
    Character &ch;
    FadeInEvent(Character &ch) : ch(ch) {}
    bool do_action() { 
        return ch.fade(ch.fade_speed);
    }
};

struct FadeOutEvent {
    Character &ch;
    FadeOutEvent(Character &ch) : ch(ch) {}
    bool do_action() { 
        return ch.fade(-ch.fade_speed);
    }
};

struct EventHandler {
    bool waiting_for_input = false;

    EventHandler() = default;

    std::list<std::unique_ptr<BaseEvent>> events;

    template<class T>
    void push_back(T&& event) {
        events.emplace_back(event);
    }

    void process() {
        events.remove_if([](const auto& event) {
                return not event->on_going();
                });

        if (events.empty()) {
            push_back(new Event<ParseEvent>);
        }
    }
};

