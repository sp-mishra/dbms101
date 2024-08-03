#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <mio.hpp>
#include <fast_io.h>
#include <hash.hpp>
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
    spdlog::info("hash: {}", grklab::hash("Hello, world!"));
}

int check_sanity() {
    spdlog::info("Hello world");
    fast_io::io::print("Hello, fast_io world!\n");
    test_json();
    test_hash();
    return 0;
}
