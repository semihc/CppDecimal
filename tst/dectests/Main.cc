#include <gtest/gtest.h>
#include "absl/base/log_severity.h"
#include "absl/flags/parse.h"
#include "absl/log/log.h"
#include "absl/log/initialize.h"
#include "absl/log/globals.h"
#include "absl/log/flags.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) 
{
  auto remvec = absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  LOG(INFO) << "Before InitGoogleTest()";
  spdlog::info("Before InitGoogleTest()");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
