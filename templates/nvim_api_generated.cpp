#include "nvim/generated/nvim_api_generated.h"

#include "nvim/api.h"
#include "nvim/exceptions.h"
#include "nvim/request.h"
#include "nvim/response.h"
#include "nvim/private/event_store.h"

#include <fmt/core.h>

namespace {

nvim::types::integer_t get_int(const nvim::types::object_t& var) NVIM_NOEXCEPT
{
    if (var.is_uint64_t()) {
        return var.as_uint64_t();
    }

    assert(var.is_int64_t());
    return var.as_int64_t();
}

{% for ui_event in ui_events %}

[[nodiscard]] std::vector<nvim::ui_events::{{ui_event.name}}> parse_{{ui_event.name}}(
  const nvim::types::array_t& array) NVIM_NOEXCEPT
{
    assert(array.at(0).is_string());
    assert(array.at(0).as_string() == "{{ui_event.name}}");
    if (array.size() == 2) {
        assert(array.at(1).is_vector());

        {% if ui_event.parameters|count > 0 %}
        const auto& values = array.at(1).as_vector();
        assert(values.size() == {{ui_event.parameters|count}});

        return {nvim::ui_events::{{ui_event.name}} {
            {% for param in ui_event.parameters %}
                {% if param.type == "nvim::types::integer_t" %}
                    get_int(values.at({{loop.index - 1}})),
                {% elif param.type == "nvim::types::window_t" %}
                    nvim::types::detail::unpack_ext(values.at({{loop.index - 1}})),
                {% elif param.type == "nvim::types::buffer_t" %}
                    nvim::types::detail::unpack_ext(values.at({{loop.index - 1}})),
                {% elif param.type == "nvim::types::tab_page_t" %}
                    nvim::types::detail::unpack_ext(values.at({{loop.index - 1}})),
                {% elif param.conversion_method != "" %}
                    static_cast<{{param.type}}>(values.at({{loop.index - 1}}).{{param.conversion_method}}),
                {% else %}
                    static_cast<{{param.type}}>(values.at({{loop.index - 1}})),
                {% endif %}
            {% endfor %}
        }};
        {% else %}
            return {nvim::ui_events::{{ui_event.name}}{}};
        {% endif %}
    }

    std::vector<nvim::ui_events::{{ui_event.name}}> options;

    auto it = std::next(array.cbegin());
    auto end = array.cend();
    for (; it != end; it++) {
        assert(it->is_vector());

        {% if ui_event.parameters|count > 0 %}
        const auto& values = it->as_vector();
        assert(values.size() == {{ui_event.parameters|count}});

        options.push_back(nvim::ui_events::{{ui_event.name}} {
            {% for param in ui_event.parameters %}
                {% if param.type == "nvim::types::integer_t" %}
                    get_int(values.at({{loop.index - 1}})),
                {% elif param.type == "nvim::types::window_t" %}
                    nvim::types::detail::unpack_ext(values.at({{loop.index - 1}})),
                {% elif param.type == "nvim::types::buffer_t" %}
                    nvim::types::detail::unpack_ext(values.at({{loop.index - 1}})),
                {% elif param.type == "nvim::types::tab_page_t" %}
                    nvim::types::detail::unpack_ext(values.at({{loop.index - 1}})),
                {% elif param.conversion_method != "" %}
                    static_cast<{{param.type}}>(values.at({{loop.index - 1}}).{{param.conversion_method}}),
                {% else %}
                    static_cast<{{param.type}}>(values.at({{loop.index - 1}})),
                {% endif %}
            {% endfor %}
        });
        {% else %}
        options.push_back(nvim::ui_events::{{ui_event.name}}{});
        {% endif %}
    }

    return options;
}

{% endfor %}

}

namespace nvim {

namespace types {
    namespace detail {

        nvim::types::uinteger_t unpack_ext(const nvim::types::object_t& var) NVIM_NOEXCEPT
        {
            const auto& ext = var.as_ext();
            const std::int8_t type = ext.type();

            const auto& ext_types = nvim::types::detail::s_ext_types;
            assert (std::find(ext_types.cbegin(), ext_types.cend(), type) != ext_types.cend());

            return static_cast<nvim::types::uinteger_t>(ext.data()[0]);
        }
    }
}

namespace api {

namespace detail {

std::vector<nvim::ui_events::generic_ui_event> parse_events(
  const nvim::types::response& res) NVIM_NOEXCEPT
{
    const nvim::types::array_t& array = res.value;
    std::vector<nvim::ui_events::generic_ui_event> events;

    for (const auto& item : array) {
        assert(item.is_vector());

        const nvim::types::array_t& item_array = item.as_vector();
        assert(item_array.at(0).is_string());

        const auto& event_name = item_array.at(0).as_string();
        {% for ui_event in ui_events %}
        {% if loop.index != 1 %} else {% endif %} if (event_name == "{{ui_event.name}}") {
            std::vector<nvim::ui_events::{{ui_event.name}}> option_events{
                parse_{{ui_event.name}}(item_array)
            };

            std::move(option_events.begin(),
                      option_events.end(),
                      std::back_inserter(events));
        }
        {% endfor %}
        else {
            fmt::print(stderr, "Unsupported event: {}\n", event_name);
        }
    }

    return events;
}

}

{% for func in functions%}
    void {{func.name}}(const types::connection& connection {% for arg in func.args %} ,{{arg.type}} {{arg.name}} {% if not loop.last %} {% endif %} {% endfor %}, std::function<void(const nvim::types::response&)> callback) {
        nvim::types::request request{ nvim::api::detail::setup_request(
            "{{func.name}}", {{func.args|count}}) };

        {% for arg in func.args %} request.args << {{arg.name}}; {% endfor %}

        nvim::api::detail::push_response_callback(request.msg_id, std::move(callback));
        nvim::api::detail::write(connection, request, 500);
    }
{% endfor %}

{% for func in functions%}
    {{func.return}} {{func.name}}(const nvim::types::connection& connection {% for arg in func.args %} ,{{arg.type}} {{arg.name}} {% if not loop.last %} {% endif %} {% endfor %}) {
        nvim::types::request request{ nvim::api::detail::setup_request(
            "{{func.name}}", {{func.args|count}}) };

        {% for arg in func.args %} request.args << {{arg.name}}; {% endfor %}

        nvim::api::detail::write(connection, request, 500);

        const nvim::types::response response = nvim::api::detail::get_response(connection);
        if (response.error.code != -1) {
            throw nvim::exceptions::bad_response{response.error.message, response.error.code};
        }

        {% if func.return != "void" %}
            return std::move(response.value);
        {% else %}
            (void)response;
        {% endif %}
    }
{% endfor %}

}

} // namespace nvim
