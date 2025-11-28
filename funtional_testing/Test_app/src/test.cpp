#include <gtest/gtest.h>
#include "funtional_testing/Test_app/inc/app.h"

TEST(AppTest, GreetTest) {
    App app;
    EXPECT_EQ(app.greet("World"), "Hello, World!");
}