#pragma once

#include <galculus/core/event.hpp>

#include <cstdint>
#include <string>
#include <utility>

namespace galculus::core {

using MessageId = std::uint64_t;

enum class MessageType : std::uint8_t {
    Event,
    Command,
    Error,
    Telemetry
};

struct Message {
    MessageId id{0};
    MessageType type{MessageType::Event};

    std::string source;
    std::string target;

    Event event;

    static Message event_message(
        MessageId id,
        std::string source,
        std::string target,
        Event event
    ) {
        Message message;
        message.id = id;
        message.type = MessageType::Event;
        message.source = std::move(source);
        message.target = std::move(target);
        message.event = std::move(event);
        return message;
    }

    static Message text_event(
        MessageId id,
        std::string source,
        std::string target,
        std::string name,
        std::string text
    ) {
        return event_message(
            id,
            std::move(source),
            std::move(target),
            Event::text(std::move(name), std::move(text))
        );
    }
};

} // namespace galculus::core