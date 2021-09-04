#include "nvim/private/event_store.h"

#include <queue>
#include <mutex>

namespace {
std::queue<nvim::ui_events::generic_ui_event> s_event_queue;
std::unordered_map<nvim::types::uinteger_t, nvim::types::response_callback>
  s_response_callbacks;
std::mutex s_eq_mutex;
std::mutex s_rc_mutex;
}

namespace nvim {
namespace api {
namespace detail {
void push_response_callback(nvim::types::uinteger_t msg_id,
                            nvim::types::response_callback callback)
  NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_rc_mutex };
    assert(s_response_callbacks.find(msg_id) == s_response_callbacks.end());

    s_response_callbacks[msg_id] = std::move(callback);
}

nvim::types::response_callback get_response_callback(
  nvim::types::uinteger_t msg_id) NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_rc_mutex };
    assert(s_response_callbacks.find(msg_id) != s_response_callbacks.end());

    auto callback = s_response_callbacks[msg_id];
    s_response_callbacks.erase(msg_id);
    return callback;
}

bool has_response_callback(nvim::types::uinteger_t msg_id) NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_rc_mutex };
    return s_response_callbacks.find(msg_id) != s_response_callbacks.end();
}

void push_events(std::vector<nvim::ui_events::generic_ui_event> events)
  NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_eq_mutex };

    std::for_each(events.begin(), events.end(), [](auto& ev) {
        s_event_queue.push(std::move(ev));
    });
}

nvim::ui_events::generic_ui_event consume_event() NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_eq_mutex };

    assert(s_event_queue.size() > 0);
    auto event = s_event_queue.front();
    s_event_queue.pop();

    return event;
}

std::vector<nvim::ui_events::generic_ui_event> get_all_events() NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_eq_mutex };

    std::vector<nvim::ui_events::generic_ui_event> events;
    while (!s_event_queue.empty()) {
        events.push_back(s_event_queue.front());
        s_event_queue.pop();
    }

    return events;
}

bool has_event() NVIM_NOEXCEPT
{
    std::lock_guard<std::mutex> lg{ s_eq_mutex };
    return s_event_queue.size() > 0;
}
}
}
}
