
// NO_LINT_BEGIN
#include "logger.h"  // Has to be included before "mock_wrapper.h"
// NO_LINT_END

#include <gmock/gmock.h>

#include "mock_wrapper.hpp"

using namespace std;
using namespace testing;

class LoggerTest : public ::testing::Test {
    void SetUp() {
        mWMock.reset(new MWMock);
        mWMock->SetUp();
    }

    void TearDown() {
        mWMock.reset(nullptr);
    }
};

TEST_F(LoggerTest, InitAndDeinit) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

TEST_F(LoggerTest, AddStdout) {
    const Logger_StreamConfig_t streamConfig{
        .stream            = stdout,
        .loggingLevel      = LOGGING_LEVEL_DEBUG,
        .isSupportingColor = true};

    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_addOutputStream(streamConfig), SUCCESS);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

#ifdef C_LOGGER_SAFE_MODE

class LoggerTestMalloc : public LoggerTest {};

TEST_F(LoggerTest, InvalidArguments) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_init(), ERR_LIBRARY_ALREADY_INITIALIZED);

    ASSERT_EQ(Logger_deinit(), SUCCESS);
    ASSERT_EQ(Logger_deinit(), ERR_LIBRARY_NOT_INITIALIZED);
}

TEST_F(LoggerTestMalloc, InitFail) {
    EXPECT_CALL(*mWMock, malloc(StrEq("Logger_init"), _))
        .WillOnce(Return(nullptr));

    ASSERT_EQ(Logger_init(), ERR_MEM_ALLOC);
}

#endif  // C_LOGGER_SAFE_MODE

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
