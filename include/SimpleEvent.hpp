#pragma once

#ifndef SIMPLEEVENT_HPP
#define SIMPLEEVENT_HPP

#include <algorithm>
#include <functional>
#include <list>
#include <set>
#include <map>
#include <typeindex>

namespace groklab {
    // Based on https://github.com/KyrietS/tinyevents.git
    class SimpleEvent {
        using ListenerHandle = std::uint64_t;
        using Listeners = std::map<ListenerHandle, std::function<void(const void *)>>;

        [[nodiscard]] bool isScheduledForRemoval(const std::uint64_t handle) const {
            return listenersScheduledForRemoval.contains(handle);
        }

        std::map<std::type_index, Listeners> listenersByType;
        std::list<std::function<void(SimpleEvent&)>> queuedDispatches;
        std::set<ListenerHandle> listenersScheduledForRemoval;

        std::uint64_t nextListenerId = 0;
    public:
        SimpleEvent() = default;
        SimpleEvent(SimpleEvent &&) noexcept = default;
        SimpleEvent(const SimpleEvent &) = delete;
        SimpleEvent &operator=(SimpleEvent &&) noexcept = default;

        template<typename T>
        std::uint64_t listen(const std::function<void(const T &)> &listener) {
            auto& listeners = listenersByType[std::type_index(typeid(T))];
            const auto listenerHandle = ListenerHandle{nextListenerId++};

            listeners[listenerHandle] = [listener](const auto &msg) {
                const auto concreteMessage = static_cast<const T *>(msg);
                listener(*concreteMessage);
            };
            return listenerHandle;
        }

        template<typename T>
        std::uint64_t listenOnce(const std::function<void(const T &)> &listener) {
            const auto listenerId = nextListenerId;
            return listen<T>([this, listenerId, listener](const T &msg) {
                ListenerHandle handle{listenerId};
                listenersScheduledForRemoval.emplace(handle); // Nested listenOnce
                listener(msg);
                listenersScheduledForRemoval.erase(handle);
                this->remove(handle);
            });
        }

        template<typename T>
        void dispatch(const T &msg) {
            const auto &listenersIter = listenersByType.find(std::type_index(typeid(T)));
            if (listenersIter == listenersByType.end()) {
                return;
            }

            const auto& [msgType, listeners] = *listenersIter;

            // Cache handles to avoid iterator invalidation. This way listeners can safely remove themselves.
            std::vector<ListenerHandle> handles;
            handles.reserve(listeners.size());
            for (const auto &handle: listeners | std::views::keys) {
                handles.push_back(handle);
            }

            for(auto const& handle: handles) {
                const auto& handleAndListener = listeners.find(handle);
                if (const bool isListenerPresent = handleAndListener != listeners.end(); isListenerPresent && !isScheduledForRemoval(handle)) {
                    const auto& listener = handleAndListener->second;
                    listener(&msg);
                }
            }
        }

        template<typename T>
        void queue(const T &msg) {
            queuedDispatches.push_back([msg](SimpleEvent& dispatcher) {
                dispatcher.dispatch(msg);
            });
        }

        void process() {
            for (auto const &queuedDispatch: queuedDispatches) {
                queuedDispatch(*this);
            }
            queuedDispatches.clear();
        }

        void remove(const std::uint64_t handle) {
            if (isScheduledForRemoval(handle)) {
                return;
            }

            for (auto &val: listenersByType | std::views::values) {
                val.erase(handle);
            }
        }

        [[nodiscard]] bool hasListener(std::uint64_t handle) const {
            if (isScheduledForRemoval(handle)) {
                return false;
            }

            return std::ranges::any_of(listenersByType, [&handle](const auto& listeners) {
                return listeners.second.find(handle) != listeners.second.end();
            });
        }
    };
}

#endif //SIMPLEEVENT_HPP
