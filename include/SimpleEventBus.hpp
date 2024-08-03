#pragma once

#ifndef SIMPLEEVENT_HPP
#define SIMPLEEVENT_HPP

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <typeindex>
#include <readerwriterqueue.h>
#include <thread>
#include <vector>

namespace groklab {
    // Inspired by https://github.com/KyrietS/tinyevents.git
    class SimpleEventBus {
        using ListenersType = std::vector<std::function<void(const void *)> >;
        using QueueFunction = std::function<void()>;
        static constexpr size_t MIN_QUEUE_SIZE = 100;
        static constexpr size_t SLEET_TIME = 10;
        std::map<std::type_index, ListenersType> listenersByType;
        std::unique_ptr<moodycamel::ReaderWriterQueue<QueueFunction>> messageQueue;
        std::thread backgroundThread;
        std::atomic<bool> running{true};

    public:
        SimpleEventBus() {
            backgroundThread = std::thread(&SimpleEventBus::process, this);
            messageQueue = std::make_unique<moodycamel::ReaderWriterQueue<QueueFunction>>(MIN_QUEUE_SIZE);
        }

        ~SimpleEventBus() {
            running = false;
            if (backgroundThread.joinable()) {
                backgroundThread.join();
            }
        }

        SimpleEventBus(SimpleEventBus &&) noexcept {
        }

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
        }

        template<typename T>
        void sendSync(const T &msg) {
            const auto typeIndex = std::type_index(typeid(T));
            const auto &listenerWrappers = listenersByType[typeIndex];
            for (const auto &listenerWrapper: listenerWrappers) {
                listenerWrapper(&msg);
            }
        }

        template<typename T>
        void send(const T &msg, bool sync = true) {
            if (sync) {
                sendSync(msg);
            } else {
                enqueue(&msg);
            }
        }

        template<typename T>
        void enqueue(const T &msg) {
            messageQueue->enqueue([this, msg]() {
                sendSync(msg);
            });
        }

    private:
        void process() const {
            while (running) {
                // Iterate over the message queue and process each message
                QueueFunction message;
                while (messageQueue->try_dequeue(message)) {
                    message();
                    std::this_thread::sleep_for(std::chrono::milliseconds(SLEET_TIME)); // Sleep to prevent busy-waiting
                }
            }
        }
    };
}

#endif //SIMPLEEVENT_HPP
