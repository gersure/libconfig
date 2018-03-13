/*
 * Copyright (C) 2018 zcf
 */
 
#pragma once

#include <unordered_map>
#include <iosfwd>
#include <experimental/string_view>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "sstring.hh"

#include "stdx.hh"

namespace core { class file; }
namespace YAML { class Node; }

namespace config {

namespace bpo = boost::program_options;

class config_file {
public:
    typedef std::unordered_map<core::sstring, core::sstring> string_map;
    typedef std::vector<core::sstring> string_list;

    enum class value_status {
        Used,
        Unused,
        Invalid,
    };

    enum class config_source : uint8_t {
        None,
        SettingsFile,
        CommandLine
    };

    struct config_src {
        stdx::string_view _name, _desc;
    public:
        config_src(stdx::string_view name, stdx::string_view desc)
            : _name(name)
            , _desc(desc)
        {}
        virtual ~config_src() {}

        const stdx::string_view & name() const {
            return _name;
        }
        const stdx::string_view & desc() const {
            return _desc;
        }

        virtual void add_command_line_option(
                        bpo::options_description_easy_init&, const stdx::string_view&,
                        const stdx::string_view&) = 0;
        virtual void set_value(const YAML::Node&) = 0;
        virtual value_status status() const = 0;
        virtual config_source source() const = 0;
    };

    template<typename T, value_status S = value_status::Used>
    struct named_value : public config_src {
    private:
        friend class config;
        stdx::string_view _name, _desc;
        T _value = T();
        config_source _source = config_source::None;
    public:
        typedef T type;
        typedef named_value<T, S> MyType;

        named_value(stdx::string_view name, const T& t = T(), stdx::string_view desc = {})
            : config_src(name, desc)
            , _value(t)
        {}
        value_status status() const override {
            return S;
        }
        config_source source() const override {
            return _source;
        }
        bool is_set() const {
            return _source > config_source::None;
        }
        MyType & operator()(const T& t) {
            _value = t;
            return *this;
        }
        MyType & operator()(T&& t, config_source src = config_source::None) {
            _value = std::move(t);
            if (src > config_source::None) {
                _source = src;
            }
            return *this;
        }
        const T& operator()() const {
            return _value;
        }
        T& operator()() {
            return _value;
        }

        void add_command_line_option(bpo::options_description_easy_init&,
                        const stdx::string_view&, const stdx::string_view&) override;
        void set_value(const YAML::Node&) override;
    };

    typedef std::reference_wrapper<config_src> cfg_ref;

    config_file(std::initializer_list<cfg_ref> = {});

    void add(cfg_ref);
    void add(std::initializer_list<cfg_ref>);

    boost::program_options::options_description get_options_description();
    boost::program_options::options_description get_options_description(boost::program_options::options_description);

    boost::program_options::options_description_easy_init&
    add_options(boost::program_options::options_description_easy_init&);

    /**
     * Default behaviour for yaml parser is to throw on
     * unknown stuff, invalid opts or conversion errors.
     *
     * Error handling function allows overriding this.
     *
     * error: <option name>, <message>, <optional value_status>
     *
     * The last arg, opt value_status will tell you the type of
     * error occurred. If not set, the option found does not exist.
     * If invalid, it is invalid. Otherwise, a parse error.
     *
     */
     using error_handler = std::function<void(const core::sstring&, const core::sstring&, boost::optional<value_status>)>;

    void read_from_yaml(const core::sstring&, error_handler = {});
    void read_from_yaml(const char *, error_handler = {});
    void read_from_file(const core::sstring&, error_handler = {});
    //void read_from_file(file, error_handler = {});

    using configs = std::vector<cfg_ref>;

    configs set_values() const;
    configs unset_values() const;
private:
    configs
        _cfgs;
};

}

