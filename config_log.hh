#pragma once

#include "log.hh"

namespace logging {

//
// Seastar changed the names of some of these types. Maintain the old names here to avoid too much churn.
//

using log_level = core::log_level;
using logger = core::logger;
using registry = core::logger_registry;

inline registry& logger_registry() noexcept {
    return core::global_logger_registry();
}

using settings = core::logging_settings;

inline void apply_settings(const settings& s) {
    core::apply_logging_settings(s);
}

using core::pretty_type_name;
using core::level_name;

}
