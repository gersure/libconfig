#include <unordered_map>
#include <regex>

#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>

#include "hamtori/print.hh"

#include "hamtori/config.hh"
#include "hamtori/log.hh"
#include "config_file_impl.hh"

namespace YAML {

// yaml-cpp conversion would do well to have some enable_if-stuff to make it possible
// to do more broad spectrum converters.
template<>
struct convert<hamtori::logging::log_level> {
    static bool decode(const Node& node, hamtori::logging::log_level& rhs) {
        std::string tmp;
        if (!convert<std::string>::decode(node, tmp)) {
            return false;
        }
        rhs = boost::lexical_cast<hamtori::logging::log_level>(tmp);
        return true;
    }
};

template<>
struct convert<hamtori::appconfig::seed_provider_type> {
    static bool decode(const Node& node, hamtori::appconfig::seed_provider_type& rhs) {
        if (!node.IsSequence()) {
            return false;
        }
        rhs = hamtori::appconfig::seed_provider_type();
        for (auto& n : node) {
            if (!n.IsMap()) {
                continue;
            }
            for (auto& n2 : n) {
                if (n2.first.as<hamtori::sstring>() == "class_name") {
                    rhs.class_name = n2.second.as<hamtori::sstring>();
                }
                if (n2.first.as<hamtori::sstring>() == "parameters") {
                  auto v = n2.second.as<std::vector<hamtori::appconfig::config::string_map>>();
                    if (!v.empty()) {
                        rhs.parameters = v.front();
                    }
                }
            }
        }
        return true;
    }
};

} //end yaml

#define _mk_name(name, ...) name,
#define str(x)  #x
#define _mk_init(name, type, deflt, status, desc, ...)  , name(str(name), type(deflt), desc)

namespace hamtori {

namespace appconfig {

config::config()
    : config::config_file({ _make_config_values(_mk_name)
        default_log_level, logger_log_level, log_to_stdout, log_to_syslog })
    _make_config_values(_mk_init)
    , default_log_level("default_log_level")
    , logger_log_level("logger_log_level")
    , log_to_stdout("log_to_stdout")
    , log_to_syslog("log_to_syslog")
{}

config::~config()
{}


template<>
void config_file::named_value<seed_provider_type,
                config::value_status::Used>::add_command_line_option(
                boost::program_options::options_description_easy_init& init,
                const stdx::string_view& name, const stdx::string_view& desc) {
    init((hyphenate(name) + "-class-name").data(),
                    value_ex(&_value.class_name)->notifier(
                                    [this](auto&&) {_source = config_source::CommandLine;}),
                    desc.data());
    init((hyphenate(name) + "-parameters").data(),
                    value_ex(&_value.parameters)->notifier(
                                    [this](auto&&) {_source = config_source::CommandLine;}),
                    desc.data());
}

boost::program_options::options_description_easy_init&
config::add_options(boost::program_options::options_description_easy_init& init) {
    config_file::add_options(init);

    //data_file_directories.add_command_line_option(init, "datadir", "alias for 'data-file-directories'");
    //rpc_port.add_command_line_option(init, "thrift-port", "alias for 'rpc-port'");
    //native_transport_port.add_command_line_option(init, "cql-port", "alias for 'native-transport-port'");

    return init;
}

boost::filesystem::path config::get_conf_dir() {
    using namespace boost::filesystem;

    path confdir;
    auto* cd = std::getenv("CONFIG_PATH");
    if (cd != nullptr) {
        confdir = path(cd);
    } else {
        auto* p = std::getenv("CONFIG_PATH");
        if (p != nullptr) {
            confdir = path(p);
        }
        confdir /= "conf";
    }

    return confdir;
}


namespace bpo = boost::program_options;

  hamtori::logging::logging_settings config::logging_settings(const bpo::variables_map& map) const {
    struct convert {
        std::unordered_map<sstring, hamtori::logging::log_level> operator()(const hamtori::program_options::string_map& map) const {
            std::unordered_map<sstring, hamtori::logging::log_level> res;
            for (auto& p : map) {
                res.emplace(p.first, (*this)(p.second));
            };
            return res;
        }
        hamtori::logging::log_level operator()(const sstring& s) const {
            return boost::lexical_cast<hamtori::logging::log_level>(s);
        }
        bool operator()(bool b) const {
            return b;
        }
    };

    auto value = [&map](auto v, auto dummy) {
        auto name = hyphenate(v.name());
        const bpo::variable_value& opt = map[name];

        if (opt.defaulted() && v.is_set()) {
            return v();
        }
        using expected = std::decay_t<decltype(dummy)>;

        return convert()(opt.as<expected>());
    };

    return hamtori::logging::logging_settings{ value(logger_log_level, hamtori::program_options::string_map())
        , value(default_log_level, sstring())
        , value(log_to_stdout, bool())
        , value(log_to_syslog, bool())
    };
}
} //end appconfig
} //end hamtori
