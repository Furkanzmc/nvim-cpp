#!/usr/bin/python3

import re
from jinja2 import Environment, FileSystemLoader
import msgpack, subprocess
import logging as log
from subprocess import run


class invalid_type(Exception):
    pass


class native_type:
    def __init__(self, name: str, expect_ref=False):
        self.name = name
        self.expect_ref = expect_ref


TYPE_MAP = {
    "ArrayOf(Integer, 2)": native_type("std::vector<nvim::types::integer_t>"),
    "Boolean": native_type("nvim::types::bool_t"),
    "String": native_type("nvim::types::string_t"),
    "void": native_type("void"),
    "Window": native_type("nvim::types::window_t"),
    "Buffer": native_type("nvim::types::buffer_t"),
    "Tabpage": native_type("nvim::types::tab_page_t"),
    "Integer": native_type("nvim::types::integer_t"),
    "Float": native_type("nvim::types::float_t"),
    "Dictionary": native_type("nvim::types::multimap_t"),
    "Array": native_type("nvim::types::array_t"),
    "Object": native_type("nvim::types::object_t"),
}

CONVERSION_METHOD_MAP = {
    "std::vector<nvim::types::integer_t>": "as_vector()",
    "nvim::types::bool_t": "as_bool()",
    "nvim::types::string_t": "as_string()",
    "nvim::types::window_t": "as_uint64_t()",
    "nvim::types::buffer_t": "as_uint64_t()",
    "nvim::types::tab_page_t": "as_uint64_t()",
    "nvim::types::integer_t": "as_int64_t()",
    "nvim::types::float_t": "as_double()",
    "nvim::types::multimap_t": "as_multimap()",
    "nvim::types::array_t": "as_vector()",
    "nvim::types::object_t": "",
}


def convert_type_to_native(nvim_t: str, enable_ref_op: bool) -> str:
    array_of = r"ArrayOf\(\s*(\w+)\s*\)"

    obj = re.match(array_of, nvim_t)
    if obj:
        ret = "std::vector<{}>".format(convert_type_to_native(obj.groups()[0], False))
        return "const " + ret + "&" if enable_ref_op else ret

    if nvim_t in TYPE_MAP:
        native_t = TYPE_MAP[nvim_t]
        return (
            "const " + native_t.name + "&"
            if enable_ref_op and native_t.expect_ref
            else native_t.name
        )
    else:
        log.error("Unknown nvim type name: {}".format(nvim_t))
        raise invalid_type()


def generate_api():
    env = Environment(loader=FileSystemLoader("templates", encoding="utf8"))
    header_template = env.get_template("nvim_api_generated.h")
    imp_template = env.get_template("nvim_api_generated.cpp")

    api_info = subprocess.check_output(["nvim", "--api-info"])
    unpacked_api = msgpack.unpackb(api_info)

    functions = []
    ext_types = {
        typename: info["id"] for typename, info in unpacked_api["types"].items()
    }
    ui_events = []
    for event in unpacked_api["ui_events"]:
        event_data = {"name": event.get("name")}
        event_data["parameters"] = [
            {
                "type": convert_type_to_native(arg[0], True),
                "name": arg[1],
                "conversion_method": CONVERSION_METHOD_MAP[
                    convert_type_to_native(arg[0], True)
                ],
            }
            for arg in event["parameters"]
        ]
        ui_events.append(event_data)

    for f in unpacked_api["functions"]:
        d = {}
        d["name"] = f["name"]

        try:
            d["return"] = convert_type_to_native(f["return_type"], False)
            d["args"] = [
                {"type": convert_type_to_native(arg[0], True), "name": arg[1]}
                for arg in f["parameters"]
            ]
            functions.append(d)
        except invalid_type:
            log.error("Invalid function: {}".format(f))

    header_file: str = "./include/nvim/generated/nvim_api_generated.h"
    imp_file: str = "./src/generated/nvim_api_generated.cpp"
    with open(header_file, "w") as f:
        f.write(
            header_template.render(
                {"functions": functions, "ext_types": ext_types, "ui_events": ui_events}
            )
        )

    with open(imp_file, "w") as f:
        f.write(
            imp_template.render(
                {"functions": functions, "ext_types": ext_types, "ui_events": ui_events}
            )
        )

    run(["clang-format", "-i", header_file], capture_output=True)
    run(["clang-format", "-i", imp_file], capture_output=True)


def main():
    generate_api()


if __name__ == "__main__":
    main()
