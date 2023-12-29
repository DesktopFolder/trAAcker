#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

namespace aa::conf {
    using json = nlohmann::json;

    static json& get() {
        static json configuration;

        std::ifstream f("config.json");
        if (f.good()) {
            // this is SO not threadsafe lol :)
            configuration = json::parse(f);
        }

        return configuration;
    }

    inline json getNS(std::string ns) {
        auto& js = get();

        if (js.contains(ns)) {
            return js[ns];
        }

        return {};
    }

    inline json getNS(json& js, std::string ns) {
        if (js.contains(ns)) {
            return js[ns];
        }

        return {};
    }

    template<typename T>
    std::optional<T> get_if(const json& js, auto key) {
        if (js.contains(key))
        {
            return js[key].template get<T>();
        }
        return {};
    }
}
