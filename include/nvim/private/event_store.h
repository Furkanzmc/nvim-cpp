#ifndef NVIM_CPP_EVENT_STORE_H
#define NVIM_CPP_EVENT_STORE_H

#include "nvim/defs.h"

#include "nvim/generated/nvim_api_generated.h"

namespace nvim {
namespace api {

namespace detail {
void push_response_callback(nvim::types::uinteger_t msg_id,
                            nvim::types::response_callback callback)
  NVIM_NOEXCEPT;

nvim::types::response_callback get_response_callback(
  nvim::types::uinteger_t msg_id) NVIM_NOEXCEPT;

bool has_response_callback(nvim::types::uinteger_t msg_id) NVIM_NOEXCEPT;

void push_events(std::vector<nvim::ui_events::generic_ui_event> events)
  NVIM_NOEXCEPT;
nvim::ui_events::generic_ui_event consume_event() NVIM_NOEXCEPT;
std::vector<nvim::ui_events::generic_ui_event> get_all_events() NVIM_NOEXCEPT;
bool has_event() NVIM_NOEXCEPT;
}
}
}

#endif
