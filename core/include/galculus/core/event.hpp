#pragma once

#include <string>
#include <utility>
#include <vector>
#include <cstdint>

namespace galculus::core {

struct Event {
    std::string name;
    std::vector<std::uint8_t> payload;

    static Event text(std::string name, std::string text) {
        Event event;
        event.name = std::move(name);
        event.payload.assign(text.begin(), text.end());
        return event;
    }

    std::string payload_as_string() const {
        return std::string(payload.begin(), payload.end());
    }
};

} // namespace galculus::core