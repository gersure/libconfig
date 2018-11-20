#include "hamtori/config.hh"
#include "hamtori/log.hh"
#include <iostream>



int main(int argc, const char * argv[]) {
    // insert code here...
    try{
    hamtori::appconfig::config conf;
    conf.read_from_file("test.yaml");
    hamtori::logging::logger log("test");

    hamtori::logging::logging_settings settings;
    settings.default_level=hamtori::logging::log_level::trace;
    settings.stdout_enabled=true;
    settings.syslog_enabled=true;
    hamtori::logging::apply_settings(settings);

    log.info("info log test:{}",2);
    log.debug("debug log test:{}",3);
    std::cout<<conf.get_options_description();
    std::cout<<conf.data_file_directories()<<std::endl;
    }catch(std::exception &e){
        std::cout<<"Execption: "<<e.what()<<std::endl;
    }
    return 0;
}
