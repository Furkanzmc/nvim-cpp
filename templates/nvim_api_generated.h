// Adapted from https://github.com/DaikiMaekawa/neovim.cpp/blob/master/templates/nvim.hpp
#ifndef NVIM_CPP_API_GENERATED_H
#define NVIM_CPP_API_GENERATED_H

#include "nvim/types.h"
#include "nvim/defs.h"

#include <variant>
#include <string_view>
#include <functional>

namespace nvim {

namespace types {
struct connection;

namespace detail {
    constexpr std::array<std::int8_t, {{ext_types|count}}> s_ext_types{ {% for type_name in ext_types %} {{ext_types[type_name]}}, {% endfor %}
    };

    nvim::types::uinteger_t unpack_ext(const nvim::types::object_t& var) NVIM_NOEXCEPT;
    }
}

namespace ui_events {
    {% for ui_event in ui_events %}
        struct {{ui_event.name}} {
            static constexpr std::string_view event_name{ "{{ui_event.name}}" };
            {% for param in ui_event.parameters %} const {{param.type}} {{param.name}}; {% endfor %}
        };
    {% endfor %}


    using generic_ui_event = std::variant<{% for ui_event in ui_events %} {{ui_event.name}} {% if not loop.last %}, {% endif %} {% endfor %}>;
}

namespace api {

namespace detail {
    std::vector<nvim::ui_events::generic_ui_event> parse_events(
    const nvim::types::response& res) NVIM_NOEXCEPT;
}

{% for func in functions%}
    void
    {{func.name}}(const types::connection& connection {% for arg in func.args %} ,{{arg.type}} {{arg.name}} {% if not loop.last %} {% endif %} {% endfor %}, std::function<void(const nvim::types::response&)> callback);
{% endfor %}

{% for func in functions%}
    {{func.return}}
    {{func.name}}(const types::connection& connection {% for arg in func.args %} ,{{arg.type}} {{arg.name}} {% if not loop.last %} {% endif %} {% endfor %});
{% endfor %}
}

} // namespace nvim

#endif
