#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

// SpdLog includes
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/cfg/argv.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace std;


ABSL_FLAG(int, debug, 0, "Set debug output");


int main(int argc, char **argv) 
{
  int rv = 1; // Failure by default
  auto exe_name = argv[0];

  try {
    auto remvec = absl::ParseCommandLine(argc, argv);

    // Prepare the logger
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("%C%m%d%H%M%S [%^%l%$] %v");
    
    // Create a file rotating logger with 5mb size max and 3 rotated files
    auto max_size = 1048576 * 5;
    auto max_files = 3;
    auto rfile_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("dectests.log", max_size, max_files, false);
    rfile_sink->set_level(spdlog::level::trace);
    rfile_sink->set_pattern("%C%m%d%H%M%S.%e[%n-%P-%t-%l] %v");

    // Create main/root logger (make_shared did not work)
    std::shared_ptr<spdlog::logger> logger(new spdlog::logger("main", {console_sink, rfile_sink}) );
    // Globally register the loggers so the can be accessed using spdlog::get(logger_name)
    spdlog::register_logger(logger);
    logger->set_level(spdlog::level::debug);
    // Overwrite current logging levels if env vars specified for it
    spdlog::cfg::load_env_levels();
    spdlog::cfg::load_argv_levels(argc, argv);

    // Set the logger as the default logger
    spdlog::set_default_logger(logger);
  
    logger->info("{} starting...", exe_name);

    ::testing::InitGoogleTest(&argc, argv);
    rv = RUN_ALL_TESTS();
    if(rv)
      logger->info("{} ended with rv={}", exe_name, rv);
    else
      logger->info("{} ended successfully", exe_name);
  }
  catch (const spdlog::spdlog_ex& e) {
    cerr << "Log initialization failed: " << e.what() << endl;
  }
  catch(const exception& e) {
    cerr << e.what() << endl;
  }
  catch(...) {
    cerr << "Unknown exception, exiting..." << endl;
  }
  
  return rv;
}
