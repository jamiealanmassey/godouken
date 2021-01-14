#include "godouken_test.h"
#include "gtest/gtest.h"

TEST(ExampleTests, ExampleTest1) {
	EXPECT_EQ(1000, 1000);
}

TEST(ExampleTests, ExampleTest2) {
	EXPECT_EQ(1050, 1050);
}

TEST(ExampleTests, ExampleTest3) {
	EXPECT_EQ(2000, 1000);
}

void GodoukenTest::_bind_methods() {

}

int32_t GodoukenTest::execute_tests() {
	::testing::InitGoogleTest();
	::testing::GTEST_FLAG(output) = "xml:/google_tests/result.xml";
	return RUN_ALL_TESTS();
}

GodoukenTest::GodoukenTest() {

}

GodoukenTest::~GodoukenTest() {
	
}
