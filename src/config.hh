#pragma once

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <unordered_map>

#include "hamtori/sstring.hh"
#include "log.hh"
#include "config_file.hh"
#include "program-options.hh"

namespace hamtori {

//namespace config {
/*
 * This type is not use, and probably never will be.
 * So it makes sense to jump through hoops just to ensure
 * it is in fact handled properly...
 */
namespace stdx = std::experimental;

struct seed_provider_type {
    seed_provider_type() = default;
    seed_provider_type(hamtori::sstring n,
            std::initializer_list<program_options::string_map::value_type> opts =
                    { })
            : class_name(std::move(n)), parameters(std::move(opts)) {
    }
    hamtori::sstring class_name;
    std::unordered_map<hamtori::sstring, hamtori::sstring> parameters;
};

class config : public config_file {
public:
    config();
    ~config();

    // Throws exception if experimental feature is disabled.
    void check_experimental(const hamtori::sstring& what) const;

    /**
     * Scans the environment variables for configuration files directory
     * definition. It's either $SCYLLA_CONF, $SCYLLA_HOME/conf or "conf" if none
     * of SCYLLA_CONF and SCYLLA_HOME is defined.
     *
     * @return path of the directory where configuration files are located
     *         according the environment variables definitions.
     */
    static boost::filesystem::path get_conf_dir();

    using string_map = std::unordered_map<hamtori::sstring, hamtori::sstring>;
                    //program_options::string_map;
    using string_list = std::vector<hamtori::sstring>;
    //using seed_provider_type = seed_provider_type;

    /*
     * All values and documentation taken from
     * http://docs.datastax.com/en/cassandra/2.1/cassandra/configuration/configCassandra_yaml_r.html
     *
     * Big fat x-macro expansion of _all_ (at this writing) cassandra opts, with the "documentation"
     * included.
     *
     * X-macro syntax:
     *
     * X(member, type, default, status, desc [, value, value...])
     *
     * Where
     *  member: is the property name -> config member name
     *  type:   is the value type (bool, uint32_t etc)
     *  status: is the current _usage_ of the opt. I.e. if you actually use the value, set it to "Used".
     *          A value marked "UsedFromSeastar" is a configuration value that is assigned based on a Seastar-defined
     *          command-line interface option.
     *          Most values are set to "Unused", as in "will probably have an effect eventually".
     *          Values set to "Invalid" have no meaning/usage in scylla, and should (and will currently)
     *          be signaled to a user providing a config with them, that these settings are pointless.
     *  desc:   documentation.
     *  value...: enumerated valid values if any. Not currently used, but why not...
     *
     *
     * Note:
     * Only values marked as "Used" will be added as command line options.
     * Options marked as "Invalid" will be warned about if read from config
     * Options marked as "Unused" will also warned about.
     *
     */

#define _make_config_values(val)                \
    val(background_writer_scheduling_quota, double, 1.0, Unused, \
            "max cpu usage ratio (between 0 and 1) for compaction process. Not intended for setting in normal operations. Setting it to 1 or higher will disable it, recommended operational setting is 0.5." \
    )   \
    val(auto_adjust_flush_quota, bool, false, Unused, \
            "true: auto-adjust memtable shares for flush processes" \
    )   \
    val(memtable_flush_static_shares, float, 0, Used, \
            "If set to higher than 0, ignore the controller's output and set the memtable shares statically. Do not set this unless you know what you are doing and suspect a problem in the controller. This option will be retired when the controller reaches more maturity" \
    )   \
    /* Initialization properties */             \
    /* The minimal properties needed for configuring a cluster. */  \
    val(cluster_name, hamtori::sstring, "", Used,   \
            "The name of the cluster; used to prevent machines in one logical cluster from joining another. All nodes participating in a cluster must have the same value."   \
    )                                           \
    val(data_file_directories, string_list, { "/var/lib/scylla/data" }, Used,   \
            "The directory location where table data (SSTables) is stored"   \
    )                                           \
    /* done! */

#define _make_value_member(name, type, deflt, status, desc, ...)    \
    named_value<type, value_status::status> name;

    _make_config_values(_make_value_member)

    hamtori::logging::logging_settings logging_settings(const boost::program_options::variables_map&) const;

    boost::program_options::options_description_easy_init&
    add_options(boost::program_options::options_description_easy_init&);

private:
    template<typename T>
    struct log_legacy_value : public named_value<T, value_status::Used> {
        using MyBase = named_value<T, value_status::Used>;

        using MyBase::MyBase;

        T value_or(T&& t) const {
            return this->is_set() ? (*this)() : t;
        }
        // do not add to boost::options. We only care about yaml config
        void add_command_line_option(boost::program_options::options_description_easy_init&,
                        const stdx::string_view&, const stdx::string_view&) override {}
    };

    log_legacy_value<hamtori::logging::log_level> default_log_level;
    log_legacy_value<std::unordered_map<hamtori::sstring, hamtori::logging::log_level>> logger_log_level;
    log_legacy_value<bool> log_to_stdout, log_to_syslog;

};

}
