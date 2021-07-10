#include "list.h"
#include <sstream>
#include <iomanip>
#include <set>
#include "tests.cpp"

int run_public_tests() {
    int pass = 0;
    pass += run_test(erase_simple, "A_erase_simple");
    pass += run_test(remove_if_simple, "B_remove_if_simple");
    pass += run_test(equality_simple, "C_equality_simple");
    pass += run_test(comparison_simple, "D_comparison_simple");
    pass += run_test(stream_insert_simple, "E_stream_insert_simple");
    pass += run_test(stream_insert_const_correct, "F_stream_insert_const_correct");
    pass += run_test(copy_constructor_simple, "G_copy_ctor_simple");
    pass += run_test(move_constructor_simple, "H_move_ctor_simple");
    pass += run_test(copy_assignment_simple, "I_copy_assign_simple");
    pass += run_test(move_assignment_simple, "J_move_assign_simple");
    return pass;
}

int run_hidden_tests() {
    std::cout << "Tests hidden until grading" << std::endl;
    return 0;
}

void run_test_harness() {
    std::cout << std::endl;
    std::cout << "----- CS 106L SIMPLE TEST HARNESS -----" << std::endl;
    std::cout << "Written by Avery Wang (2019-2020 lecturer)" << std::endl << std::endl;
    int required_pass = 0;
    int student_pass = 0;
    std::cout << "----- Provided Tests -----" << std::endl;
    required_pass += run_public_tests();
    std::cout << std::endl;
    std::cout << "----- Student Tests -----" << std::endl;
    student_pass += run_student_tests();
    std::cout << std::endl << "----- Test Harness Summary -----" << std::endl;
    std::cout << "Provided Tests passed: " << required_pass << "/10" << std::endl;
    std::cout << "Student Tests passed: " << student_pass << std::endl;

    if (required_pass < 9) {
        std::cout << "Some provided tests were failed or skipped. " << std::endl;
    } else {
        std::cout << "You passed all provided tests! Great job!" << std::endl;
    }

    std::cout << std::endl << "----- End of Test Harness -----" << std::endl;
}

int main() {
    run_test_harness();
}