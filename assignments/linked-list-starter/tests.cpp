#include "test_utility.cpp"
#include "list.h"
#include <exception>
#include <sstream>
#include <iomanip>
#include <set>

void erase_simple() {
    List<int> list;
    for (int i : {1, 2, 3, 4, 5}) {
        list.push_back(i);
    }
    CHECK_TRUE(list.is_same({1, 2, 3, 4, 5}), __LINE__);
    auto iter1 = list.begin(); // points to 1
    auto iter3 = ++++list.begin(); // points to 3
    auto iter5 = --list.end(); // points to 5
    auto iter2 = list.erase(iter1); // list = {2, 3, 4, 5}
    CHECK_TRUE(list.is_same({2, 3, 4, 5}), __LINE__);
    auto iter4 = list.erase(iter3); // list = {2, 4, 5}
    CHECK_TRUE(list.is_same({2, 4, 5}), __LINE__);
    auto iter6 = list.erase(iter5); // list = {2, 4}
    CHECK_TRUE(list.is_same({2, 4}), __LINE__);
    CHECK_TRUE(*iter2 == 2, __LINE__);
    CHECK_TRUE(*iter4 == 4, __LINE__);
    CHECK_TRUE(iter6 == list.end(), __LINE__);
}

void remove_if_simple() {
    // remove this line once you have fixed the bug in remove_if.
    NOT_YET_FIXED("remove_if", __LINE__);

    List<int> list;
    for (int i : {2, 3, 5, 1, 8, 9, -3, 0, 4}) {
        list.push_back(i);
    }
    
    auto less_than_5 = [](const auto& elem) {
        return elem < 5;
    };
    list.remove_if(less_than_5); 
    CHECK_TRUE(list.is_same({5, 8, 9}), __LINE__);
}

void equality_simple() {
    List<int> list1, list2;
    for (int i : {1, 2}) {
        list1.push_back(i);
        list2.push_back(i);
    }
    CHECK_TRUE(list1 == list2, __LINE__); // list1 == list2
    list1.pop_front(); // list1 = {1}
    CHECK_TRUE(!(list1 == list2), __LINE__); // list1 != list2
}

void comparison_simple() {
    List<int> list1;
    list1.push_back(1); // list1 = {1}
    List<int> list2;
    list2.push_back(2); // list2 = {2}
    CHECK_TRUE(list1 < list2, __LINE__); // {1} < {2}
    CHECK_TRUE(!(list1 > list2), __LINE__); 
    CHECK_TRUE(list1 != list2, __LINE__);

    list1.push_front(2); // list1 = {2, 1}
    CHECK_TRUE(!(list1 < list2), __LINE__);
    CHECK_TRUE(list1 > list2, __LINE__); // list2 is prefix of list1
    CHECK_TRUE(list1 != list2, __LINE__);
}

 void stream_insert_simple() {
    List<int> list;
    for (int i : {1, 2, 3}) {
        list.push_back(i);
    }
    std::ostringstream oss;
    oss << list;
    CHECK_TRUE(oss.str() == "{1, 2, 3}", __LINE__);
}

void stream_insert_const_correct() {
    // Nugget #6

    // The starter code's header for operator<< is:
    //      std::ostream& operator<<(std::ostream&, List<T>&)
    // The correct header should be:
    //      std::ostream& operator<<(std::ostream&, const List<T>&)

    // TODO: write a test case using List<T> such that
    //      - it compiles correctly when the const is added
    //      - it does not compile when the const is missing
    

    // remove this line once you have implemented a test case for the stream insertion operator.
    NOT_YET_FIXED("operator<< test case", __LINE__);
}

void copy_constructor_simple() {
    // remove this line once you have fixed the copy constructor.
    NOT_YET_FIXED("copy constructor", __LINE__);

    List<int> list;
    list.push_back(1); // list = {1}
    list.push_back(2); // list = {1, 2}
    List<int> copy = list; // copy = list = {1, 2}
    CHECK_TRUE(list.is_same({1, 2}), __LINE__);
    CHECK_TRUE(copy.is_same({1, 2}), __LINE__);
    list.push_back(4); // list = {1, 2, 4}, copy = {1, 2}
    CHECK_TRUE(list.is_same({1, 2, 4}), __LINE__);
    CHECK_TRUE(copy.is_same({1, 2}), __LINE__);
}

void move_constructor_simple() {
    // remove this line once you have fixed the move constructor.
    NOT_YET_FIXED("move constructor", __LINE__);

    List<int> list;
    list.push_back(1); // list = {1}
    list.push_back(2); // list = {1, 2}
    List<int> moved = std::move(list); // moved = {1, 2}, list is undefined
    CHECK_TRUE(moved.is_same({1, 2}), __LINE__);
    moved.push_back(4); // moved = {1, 2, 4}
    List<int> new_list = std::move(moved); // new_list = {1, 2, 4}, moved is undefined
    CHECK_TRUE(new_list.is_same({1, 2, 4}), __LINE__);
}

void copy_assignment_simple() {
    List<int> list;
    list.push_back(1); // list = {1}
    list.push_back(2); // list = {1, 2}
    List<int> copy;
    copy = list; // copy = list = {1, 2}
    CHECK_TRUE(list.is_same({1, 2}), __LINE__); 
    CHECK_TRUE(copy.is_same({1, 2}), __LINE__);
    list.push_back(4);
    CHECK_TRUE(list.is_same({1, 2, 4}), __LINE__);
    CHECK_TRUE(copy.is_same({1, 2}), __LINE__);
}

void move_assignment_simple() {
    List<int> list;
    list.push_back(1); // list = {1}
    list.push_back(2); // list = {1, 2}
    List<int> moved;
    moved = std::move(list); // moved = {1, 2}, list is undefined
    CHECK_TRUE(moved.is_same({1, 2}), __LINE__);
    moved.push_back(4); // moved = {1, 2, 4}
    list = std::move(moved); // list = {1, 2, 4}, moved is undefined - check that you can still reassign after moved
    CHECK_TRUE(list.is_same({1, 2, 4}), __LINE__);
}

void student_test_demo() {
    // Nugget 10
    // TODO: add your own test case
    // Feel free to add your own using the format shown here.

    // remove this line once you have implemented a test case.
    NOT_YET_FIXED("operator<< test case", __LINE__);
}

// Add more tests here as functions

int run_student_tests() {
    int pass = 0;
    pass += run_test(student_test_demo, "A_student_test_demo");
    // TODO: call your test case using the following format
    // pass += run_test(name of the function that runs the test, string identifier for that test)
    return pass;
}