
#include <gtest/gtest.h>

#include "logger.h"

using namespace std;
using namespace testing;

class LoggerTest : public ::testing::Test {};

#ifdef C_LOGGER_SAFE_MODE
TEST_F(LoggerTest, InvalidArguments) {
}
#endif  // C_LOGGER_SAFE_MODE

TEST_F(LoggerTest, GetStr) {
    logger_log("testing str: %s", "my string");
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
