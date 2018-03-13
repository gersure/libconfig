#pragma once

#include "sstring.hh"

#include <boost/any.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace core {

namespace program_options {
///
/// \brief Wrapper for command-line options with arbitrary string associations.
///
/// This type, to be used with Boost.Program_options, will result in an option that stores an arbitrary number of
/// string associations.
///
/// Values are specified in the form "key0=value0:[key1=value1:...]". Options of this type can be specified multiple
/// times, and the values will be merged (with the last-provided value for a key taking precedence).
///
/// \note We need a distinct type (rather than a simple type alias) for overload resolution in the implementation, but
/// advertizing our inheritance of \c std::unordered_map would introduce the possibility of memory leaks since STL
/// containers do not declare virtual destructors.
///

class string_map final : private std::unordered_map<core::sstring, core::sstring> {
private:
    using base = std::unordered_map<core::sstring, core::sstring>;
public:
    using base::value_type;
    using base::key_type;
    using base::mapped_type;

    using base::base;
    using base::at;
    using base::find;
    using base::count;
    using base::emplace;
    using base::clear;
    using base::operator[];
    using base::begin;
    using base::end;

    friend bool operator==(const string_map&, const string_map&);
    friend bool operator!=(const string_map&, const string_map&);
};

inline bool operator==(const string_map& lhs, const string_map& rhs) {
    return static_cast<const string_map::base&>(lhs) == static_cast<const string_map::base&>(rhs);
}

inline bool operator!=(const string_map& lhs, const string_map& rhs) {
    return !(lhs == rhs);
}

///
/// \brief Query the value of a key in a \c string_map, or a default value if the key doesn't exist.
///
core::sstring get_or_default(const string_map&, const core::sstring& key, const core::sstring& def = core::sstring());

std::istream& operator>>(std::istream& is, string_map&);
std::ostream& operator<<(std::ostream& os, const string_map&);

/// \cond internal

//
// Required implementation hook for Boost.Program_options.
//
void validate(boost::any& out, const std::vector<std::string>& in, string_map*, int);

/// \endcond

}

}
