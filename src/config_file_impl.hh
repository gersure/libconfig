#pragma once

#include <iterator>
#include <regex>

#include <yaml-cpp/yaml.h>
#include <boost/any.hpp>

#include "config_file.hh"

namespace YAML {

/*
 * Add converters as needed here...
 *
 * TODO: Maybe we should just define all node conversionas as "lexical_cast".
 * However, vanilla yamp-cpp does some special treatment of scalar types,
 * mainly inf handling etc. Hm.
 */
template<>
struct convert<hamtori::sstring> {
    static bool decode(const Node& node, hamtori::sstring& rhs) {
        std::string tmp;
        if (!convert<std::string>::decode(node, tmp)) {
            return false;
        }
        rhs = tmp;
        return true;
    }
};

template<typename K, typename V, typename... Rest>
struct convert<std::unordered_map<K, V, Rest...>> {
    using map_type = std::unordered_map<K, V, Rest...>;

    static bool decode(const Node& node, map_type& rhs) {
        if (!node.IsMap()) {
            return false;
        }
        rhs.clear();
        for (auto& n : node) {
            rhs[n.first.as<K>()] = n.second.as<V>();
        }
        return true;
    }
};

} //end yaml

namespace std {

template<typename K, typename V, typename... Args>
std::istream& operator>>(std::istream&, std::unordered_map<K, V, Args...>&);

template<>
std::istream& operator>>(std::istream&, std::unordered_map<hamtori::sstring, hamtori::sstring>&);

extern template
std::istream& operator>>(std::istream&, std::unordered_map<hamtori::sstring, hamtori::sstring>&);

template<typename V, typename... Args>
std::istream& operator>>(std::istream&, std::vector<V, Args...>&);

template<>
std::istream& operator>>(std::istream&, std::vector<hamtori::sstring>&);

extern template
std::istream& operator>>(std::istream&, std::vector<hamtori::sstring>&);

template<typename K, typename V, typename... Args>
std::istream& operator>>(std::istream& is, std::unordered_map<K, V, Args...>& map) {
    std::unordered_map<hamtori::sstring, hamtori::sstring> tmp;
    is >> tmp;

    for (auto& p : tmp) {
        map[boost::lexical_cast<K>(p.first)] = boost::lexical_cast<V>(p.second);
    }
    return is;
}

template<typename V, typename... Args>
std::istream& operator>>(std::istream& is, std::vector<V, Args...>& dst) {
    std::vector<hamtori::sstring> tmp;
    is >> tmp;
    for (auto& v : tmp) {
        dst.emplace_back(boost::lexical_cast<V>(v));
    }
    return is;
}

template<typename K, typename V, typename... Args>
void validate(boost::any& out, const std::vector<std::string>& in, std::unordered_map<K, V, Args...>*, int utf8) {
    using map_type = std::unordered_map<K, V, Args...>;

    if (out.empty()) {
        out = boost::any(map_type());
    }

    static const std::regex key(R"foo((?:^|\:)([^=:]+)=)foo");

    auto* p = boost::any_cast<map_type>(&out);
    for (const auto& s : in) {
        std::sregex_iterator i(s.begin(), s.end(), key), e;

        if (i == e) {
            throw boost::program_options::invalid_option_value(s);
        }

        while (i != e) {
            auto k = (*i)[1].str();
            auto vs = s.begin() + i->position() + i->length();
            auto ve = s.end();

            if (++i != e) {
                ve = s.begin() + i->position();
            }

            (*p)[boost::lexical_cast<K>(k)] = boost::lexical_cast<V>(hamtori::sstring(vs, ve));
        }
    }
}

} //end std

namespace hamtori {

namespace appconfig {
namespace {

/*
 * Our own bpo::typed_valye.
 * Only difference is that we _don't_ apply defaults (they are already applied)
 * Needed to make aliases work properly.
 */
template<class T, class charT = char>
class typed_value_ex : public bpo::typed_value<T, charT> {
public:
    typedef bpo::typed_value<T, charT> _Super;

    typed_value_ex(T* store_to)
        : _Super(store_to)
    {}
    bool apply_default(boost::any& value_store) const override {
        return false;
    }
};

template<class T>
inline typed_value_ex<T>* value_ex(T* v) {
    typed_value_ex<T>* r = new typed_value_ex<T>(v);
    return r;
}

template<class T>
inline typed_value_ex<std::vector<T>>* value_ex(std::vector<T>* v) {
    auto r = new typed_value_ex<std::vector<T>>(v);
    r->multitoken();
    return r;
}
} //end null namespace

hamtori::sstring hyphenate(const stdx::string_view&);

} //end appconfig
} //end hamtori

template<typename T, hamtori::appconfig::config_file::value_status S>
void hamtori::appconfig::config_file::named_value<T, S>::add_command_line_option(
                boost::program_options::options_description_easy_init& init,
                const stdx::string_view& name, const stdx::string_view& desc) {
    // NOTE. We are not adding default values. We could, but must in that case manually (in some way) geenrate the textual
    // version, since the available ostream operators for things like pairs and collections don't match what we can deal with parser-wise.
    // See removed ostream operators above.
    init(hyphenate(name).data(), value_ex(&_value)->notifier([this](auto&&) { _source = config_source::CommandLine; }), desc.data());
}

template<typename T, hamtori::appconfig::config_file::value_status S>
void hamtori::appconfig::config_file::named_value<T, S>::set_value(const YAML::Node& node) {
    (*this)(node.as<T>());
    _source = config_source::SettingsFile;
}
