
#include <gtest/gtest.h>

#include "logger.h"

using namespace std;
using namespace testing;

class LoggerTest : public ::testing::Test {};

#ifdef C_LOGGER_SAFE_MODE
TEST_F(LoggerTest, InvalidArguments) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_init(), ERR_LIBRARY_ALREADY_INITIALIZED);

    ASSERT_EQ(Logger_deinit(), SUCCESS);
    ASSERT_EQ(Logger_deinit(), ERR_LIBRARY_NOT_INITIALIZED);
}
#endif  // C_LOGGER_SAFE_MODE

TEST_F(LoggerTest, InitAndDeinit) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

TEST_F(LoggerTest, AddStdout) {
    Logger_StreamConfig_t streamConfig = {.stream = stdout, .loggingLevel = LOGGING_LEVEL_DEBUG, .isSupportingColor = true};
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_addOutputStream(streamConfig), SUCCESS);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// test Github Actions
