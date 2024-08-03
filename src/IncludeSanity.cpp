#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <mio.hpp>
#include <fast_io.h>
#include <hash.hpp>

#include "Math.hpp"
#include "SimpleEvent.hpp"

using json = nlohmann::json;


void test_json() {
    // create object from string literal
    json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;

    // or even nicer with a raw string literal
    auto j2 = R"(
  {
    "happy": true,
    "pi": 3.141
  }
)"_json;
    spdlog::info(j.dump());
}

void test_hash() {
    spdlog::info("hash: {}", groklab::hash("Hello, world!"));
}

void test_event_bus() {
    struct TestEvent {
        long value = 0;
    };

    groklab::SimpleEvent simple_event;

    // Register a listener for MyEvent
    auto handle = simple_event.listen<TestEvent>([](const auto& event) {
        spdlog::info("Received Event: {}", event.value);
    });

    // Dispatch an event
    simple_event.dispatch(TestEvent{groklab::randomNumber()});  // Prints "Received MyEvent: 11"

    // Queue events
    simple_event.queue(TestEvent{groklab::randomNumber()});
    simple_event.queue(TestEvent{333});
    simple_event.queue(TestEvent{});
    simple_event.process();

    simple_event.remove(handle);         // Remove the listener
    simple_event.dispatch(TestEvent{groklab::randomNumber()});  // No listener, so nothing happens
}

int check_sanity() {
    spdlog::info("Hello world");
    fast_io::io::print("Hello, fast_io world!\n");
    test_json();
    test_hash();
    test_event_bus();
    return 0;
}
