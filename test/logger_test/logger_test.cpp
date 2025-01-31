
// NO_LINT_BEGIN
#include "logger.h"  // Has to be included before "mock_wrapper.h"
// NO_LINT_END

#include <gmock/gmock.h>

#include "mock_wrapper.hpp"

using namespace std;
using namespace testing;

class LoggerTest : public ::testing::Test {
protected:
    static inline Logger_StreamConfig_t streamConfigDefault;
    static inline Logger_StreamConfig_t streamConfigNullStream;
    static inline Logger_StreamConfig_t streamConfigLoggingLevelTooBig;
    static inline Logger_StreamConfig_t streamConfigLoggingLevelTooSmall;
    static inline Logger_StreamConfig_t streamConfigLoggingFormatTooBig;
    static inline Logger_StreamConfig_t streamConfigLoggingFormatTooSmall;

public:
    void SetUp() override {
        mWMock = make_unique<MWMock>();
        mWMock->SetUp();

        streamConfigDefault = {
            .stream        = stdout,
            .loggingLevel  = LOGGING_LEVEL_DEBUG,
            .loggingFormat = LOGGING_FORMAT_SCREEN};

        streamConfigNullStream        = streamConfigDefault;
        streamConfigNullStream.stream = nullptr;

        streamConfigLoggingLevelTooBig              = streamConfigDefault;
        streamConfigLoggingLevelTooBig.loggingLevel = LOGGING_LEVEL_COUNT;

        streamConfigLoggingLevelTooSmall              = streamConfigDefault;
        streamConfigLoggingLevelTooSmall.loggingLevel = static_cast<Logger_LoggingLevel_t>(-1);

        streamConfigLoggingFormatTooBig               = streamConfigDefault;
        streamConfigLoggingFormatTooBig.loggingFormat = LOGGING_FORMAT_COUNT;

        streamConfigLoggingFormatTooSmall               = streamConfigDefault;
        streamConfigLoggingFormatTooSmall.loggingFormat = static_cast<Logger_LoggingFormat_t>(-1);
    }

    void TearDown() override {
        mWMock.reset(nullptr);
    }
};

TEST_F(LoggerTest, InitAndDeinit) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

TEST_F(LoggerTest, AddStdout) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_addOutputStream(streamConfigDefault), SUCCESS);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

TEST_F(LoggerTest, PrintToStdout) {
    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_addOutputStream(streamConfigDefault), SUCCESS);
    LOG_DEBUG("hello");
    FAIL();
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

#ifdef C_LOGGER_SAFE_MODE

class LoggerTestMemoryAllocationFailure : public LoggerTest {};

TEST_F(LoggerTest, InvalidArguments) {
    ASSERT_EQ(Logger_addOutputStream(streamConfigDefault), ERR_LIBRARY_NOT_INITIALIZED);

    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_init(), ERR_LIBRARY_ALREADY_INITIALIZED);

    ASSERT_EQ(Logger_addOutputStream(streamConfigNullStream), ERR_BAD_ARGUMENT);
    ASSERT_EQ(Logger_addOutputStream(streamConfigLoggingLevelTooBig), ERR_BAD_ARGUMENT);
    ASSERT_EQ(Logger_addOutputStream(streamConfigLoggingLevelTooSmall), ERR_BAD_ARGUMENT);
    ASSERT_EQ(Logger_addOutputStream(streamConfigLoggingFormatTooBig), ERR_BAD_ARGUMENT);
    ASSERT_EQ(Logger_addOutputStream(streamConfigLoggingFormatTooSmall), ERR_BAD_ARGUMENT);

    ASSERT_EQ(Logger_deinit(), SUCCESS);
    ASSERT_EQ(Logger_deinit(), ERR_LIBRARY_NOT_INITIALIZED);
}

TEST_F(LoggerTestMemoryAllocationFailure, InitFail) {
    EXPECT_CALL(*mWMock, malloc(StrEq("Logger_init"), _))
        .WillOnce(Return(nullptr));

    ASSERT_EQ(Logger_init(), ERR_MEM_ALLOC);
}

TEST_F(LoggerTestMemoryAllocationFailure, AddOutputStreamFail) {
    EXPECT_CALL(*mWMock, realloc(StrEq("Logger_addOutputStream"), _, _))
        .WillOnce(Return(nullptr));

    ASSERT_EQ(Logger_init(), SUCCESS);
    ASSERT_EQ(Logger_addOutputStream(streamConfigDefault), ERR_MEM_ALLOC);
    ASSERT_EQ(Logger_deinit(), SUCCESS);
}

#endif  // C_LOGGER_SAFE_MODE

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
