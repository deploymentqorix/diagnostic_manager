#include "funtional_testing/Test_app/inc/app.h"

App::App() {}

std::string App::greet(const std::string& name) {
    return "Hello, " + name + "!";
}