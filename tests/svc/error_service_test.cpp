#include "gtest/gtest.h"
#include "error_service.h"

class ErrorServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(ErrorServiceTest, RegisterWithValidBaseCode) {
    SX_REGISTER_ERROR_MODULE(test_module, 0x10000000);
    auto module = sx::ErrorRegistry::instance().getModule("test_module");
    ASSERT_NE(module, nullptr);
    EXPECT_EQ(module->baseCode(), 0x10000000);
}

TEST_F(ErrorServiceTest, RegisterWithInvalidBaseCode) {
    // 测试无效的基础码（不是4K对齐）
    // 注意：这里不会触发断言，因为基础码无效会直接返回nullptr
    auto module = sx::ErrorRegistry::instance().registerModule("test_module_invalid", 0x10000001);
    EXPECT_EQ(module, nullptr);
}

TEST_F(ErrorServiceTest, AddErrorWithValidCode) {
    SX_REGISTER_ERROR_MODULE(test_module_add, 0x20000000);
    EXPECT_TRUE(SX_ADD_MODULE_ERROR(test_module_add, 1, "test error 1"));
    EXPECT_TRUE(SX_ADD_MODULE_ERROR(test_module_add, 2, "test error 2"));
    
    EXPECT_EQ(SX_GET_FULL_ERROR_CODE(test_module_add, 1), 0x20000001);
    EXPECT_EQ(SX_GET_FULL_ERROR_CODE(test_module_add, 2), 0x20000002);
}

TEST_F(ErrorServiceTest, AddErrorWithInvalidLocalCode) {
    SX_REGISTER_ERROR_MODULE(test_module_invalid_code, 0x30000000);
    EXPECT_FALSE(SX_ADD_MODULE_ERROR(test_module_invalid_code, 0x1000, "invalid code")); // 超过 0xFFF
}

TEST_F(ErrorServiceTest, GetErrorDescriptionWithValidCode) {
    SX_REGISTER_ERROR_MODULE(test_module_desc, 0x40000000);
    SX_ADD_MODULE_ERROR(test_module_desc, 1, "connection failed");
    SX_ADD_MODULE_ERROR(test_module_desc, 2, "timeout occurred");
    
    EXPECT_EQ(SX_GET_ERROR_DESCRIPTION(0x40000001), "connection failed");
    EXPECT_EQ(SX_GET_ERROR_DESCRIPTION(0x40000002), "timeout occurred");
}

TEST_F(ErrorServiceTest, GetErrorDescriptionWithInvalidCode) {
    EXPECT_EQ(SX_GET_ERROR_DESCRIPTION(0x99999999), "Unknown error code");
}

TEST_F(ErrorServiceTest, MakeErrorCode) {
    SX_REGISTER_ERROR_MODULE(test_module_make, 0x50000000);
    SX_ADD_MODULE_ERROR(test_module_make, 1, "test error");
    
    EXPECT_EQ(SX_MAKE_ERROR_CODE(test_module_make, 1), 0x50000001);
    EXPECT_EQ(SX_MAKE_ERROR_CODE(test_module_make, 2), 0x50000002);
}

TEST_F(ErrorServiceTest, MakeError) {
    SX_REGISTER_ERROR_MODULE(test, 0x60000000);
    ASSERT_EQ(SX_MAKE_ERROR_CODE(test, 1), 0x60000001);
}
