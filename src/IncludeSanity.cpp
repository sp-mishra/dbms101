#include <iostream>

#include <nlohmann/json.hpp>
#include <mio.hpp>
#include <fast_io.h>
#include <hash.hpp>
#include <nanobench.h>

#include "Log.hpp"
#include "Math.hpp"
#include "SimpleEventBus.hpp"
#include <readerwriterqueue.h>

using namespace moodycamel;

using json = nlohmann::json;
namespace g = groklab;


void test_json() {
    // create object from string literal
    json j = R"({ "happy": true, "pi": 3.141 })";

    // or even nicer with a raw string literal
    auto j2 = R"(
  {
    "happy": true,
    "pi": 3.141
  }
)"_json;
    g::info(j.dump());
}

void test_hash() {
    g::info("hash: {}", g::hash("Hello, world!"));
}

void test_benchmark() {
    ankerl::nanobench::Bench().epochs(3).run("sleep 100ms", [] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
});
}

void test_event_bus() {
    struct TestEvent {
        std::string name = "TestEvent";
        long value = 0;
    };

    groklab::SimpleEventBus simple_event_bus;

    // Register a listener for MyEvent
    simple_event_bus.addListner<TestEvent>([](const auto& event) {
        g::info("Event Name: {}, Value: {}", event.name, event.value);
    });

    // // Dispatch an event
    simple_event_bus.send(TestEvent{"Sync", g::randomNumber()});
    for(int i = 0; i < 10; i++) {
        simple_event_bus.enqueue(TestEvent{std::format("ASync: {}", i), g::randomNumber()});
    }
}

void test_atomic_queue() {
    ReaderWriterQueue<int> q(100);       // Reserve space for at least 100 elements up front

    q.enqueue(17);                       // Will allocate memory if the queue is full
    bool succeeded = q.try_enqueue(18);  // Will only succeed if the queue has an empty slot (never allocates)
    assert(succeeded);

    int number;
    succeeded = q.try_dequeue(number);  // Returns false if the queue was empty

    assert(succeeded && number == 17);

    // You can also peek at the front item of the queue (consumer only)
    int const* front = q.peek();
    assert(*front == 18);
    succeeded = q.try_dequeue(number);
    assert(succeeded && number == 18);
    front = q.peek();
    assert(front == nullptr);           // Returns nullptr if the queue was empty
}

int check_sanity() {
    spdlog::info("Hello world");
    fast_io::io::print("Hello, fast_io world!\n");
    test_json();
    test_hash();
    test_event_bus();
    test_atomic_queue();
    test_benchmark();
    return 0;
}
