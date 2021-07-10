#include "hashmap.h"

using namespace std;
#include "tests.cpp"
#include "student_main.cpp"
#include "test_settings.cpp"
// add necessary includes here

int main() {
#if RUN_TEST_HARNESS
    run_test_harness();
#else
    std::cout << "RUNNING STUDENT MAIN" << std::endl;
    student_main();
    std::cout << "SUCCESSFULLY COMPLETED STUDENT MAIN" << std::endl;
#endif
    return 0;
}
