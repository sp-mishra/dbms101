#pragma once

#ifndef SIMPLEEVENT_HPP
#define SIMPLEEVENT_HPP

#include <algorithm>
#include <functional>
#include <list>
#include <set>
#include <map>
#include <typeindex>
#include <readerwriterqueue.h>

namespace groklab {
    // Inspired by https://github.com/KyrietS/tinyevents.git
    class SimpleEventBus {
        using ListenersType = std::vector<std::function<void(const void *)> >;
        std::map<std::type_index, ListenersType> listenersByType;
        std::map<std::type_index, void *> messageQueue;
        static constexpr size_t MIN_QUEUE_SIZE = 100;
        static constexpr size_t SLEET_TIME = 10;
        std::thread backgroundThread;
        std::atomic<bool> running{true};

    public:
        SimpleEventBus() {
            backgroundThread = std::thread(&SimpleEventBus::process, this);
        }

        ~SimpleEventBus() {
            running = false;
            if (backgroundThread.joinable()) {
                backgroundThread.join();
            }
        }

        SimpleEventBus(SimpleEventBus &&) noexcept = default;

        SimpleEventBus(const SimpleEventBus &) = delete;

        SimpleEventBus &operator=(SimpleEventBus &&) noexcept = delete;

        template<typename T>
        void addListner(const std::function<void(const T &)> &listener) {
            const auto typeIndex = std::type_index(typeid(T));
            auto &listeners = listenersByType[typeIndex];
            listeners.push_back([listener](const auto &msg) {
                const auto message = static_cast<const T *>(msg);
                listener(*message);
            });
            auto &queue = messageQueue[typeIndex];
            if (queue == nullptr) {
                queue = new moodycamel::ReaderWriterQueue<T>(MIN_QUEUE_SIZE);
            }
        }

        template<typename T>
        void send(const T &msg, bool sync = true) {
            const auto typeIndex = std::type_index(typeid(T));
            if (sync) {
                const auto &listenerWrappers = listenersByType[typeIndex];
                for (const auto &listenerWrapper: listenerWrappers) {
                    listenerWrapper(&msg);
                }
            } else {
                enqueue(&msg);
            }
        }

        template<typename T>
        void enqueue(const T &msg) {
            const auto typeIndex = std::type_index(typeid(T));
            auto &queue = messageQueue[typeIndex];
            auto *q = static_cast<moodycamel::ReaderWriterQueue<T> *>(queue);
            q->try_enqueue(msg);
        }

    private:
        void process() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(SLEET_TIME)); // Sleep to prevent busy-waiting
            }
        }
    };
}

#endif //SIMPLEEVENT_HPP
