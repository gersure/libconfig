#include "program-options.hh"

#include <regex>

namespace bpo = boost::program_options;

namespace core {

namespace program_options {

core::sstring get_or_default(const string_map& ss, const core::sstring& key, const core::sstring& def) {
    const auto iter = ss.find(key);
    if (iter != ss.end()) {
        return iter->second;
    }

    return def;
}

static void parse_map_associations(const std::string& v, string_map& ss) {
    static const std::regex colon(":");

    std::sregex_token_iterator s(v.begin(), v.end(), colon, -1);
    const std::sregex_token_iterator e;
    while (s != e) {
        const core::sstring p = std::string(*s++);

        const auto i = p.find('=');
        if (i == core::sstring::npos) {
            throw bpo::invalid_option_value(p);
        }

        auto k = p.substr(0, i);
        auto v = p.substr(i + 1, p.size());
        ss[std::move(k)] = std::move(v);
    };
}

void validate(boost::any& out, const std::vector<std::string>& in, string_map*, int) {
    if (out.empty()) {
        out = boost::any(string_map());
    }

    auto* ss = boost::any_cast<string_map>(&out);

    for (const auto& s : in) {
        parse_map_associations(s, *ss);
    }
}

std::ostream& operator<<(std::ostream& os, const string_map& ss) {
    int n = 0;

    for (const auto& e : ss) {
        if (n > 0) {
            os << ":";
        }

        os << e.first << "=" << e.second;
        ++n;
    }

    return os;
}

std::istream& operator>>(std::istream& is, string_map& ss) {
    std::string str;
    is >> str;

    parse_map_associations(str, ss);
    return is;
}

}

}
