#include "config.hh"
#include "log.hh"
#include "config_log.hh"
#include <iostream>



int main(int argc, const char * argv[]) {
    // insert code here...
    config::config conf;
    conf.read_from_file("test.yaml");
    core::logger log("test");

    logging::settings settings;
    settings.default_level=log_level::trace;
    settings.stdout_enabled=true;
    settings.syslog_enabled=true;
    logging::apply_settings(settings);

    log.info("info log test:{}",2);
    log.debug("debug log test:{}",3);
    std::cout<<conf.get_options_description();
    std::cout<<conf.data_file_directories()<<std::endl;
    return 0;
}
