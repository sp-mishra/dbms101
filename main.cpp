#include <spdlog/spdlog.h>
void check_sanity();

int main() {
    spdlog::info("Hello world");
    check_sanity();
    return 0;
}
