#include "hashmap.h"

using namespace std;
// add necessary includes here

void test();

int student_main() {
    cout << "This is student main. You can try using HashMap as a client" << endl;
    cout << "and call map.debug() to see the internal state of the hash table." << endl;
    cout << "This is a good way to debug if you like a good visual of the linked lists." << endl;
    cout << endl;
    cout << "To turn on the test harness, go to test_settings.cpp and change RUN_TEST_HARNESS to 1." << endl;
    cout << endl << endl;
    test();
    return 0;
}

void test() {
    HashMap<string, int> map(5);
    cout << "Hello from your past and current lecturers!" << endl;
    auto [anna_iter, anna_found] = map.insert({"Anna", 2});
    map.insert({"Avery", 3});
    map.insert({"Nikhil", 4});
    map.insert({"Ethan", 5});
    map.debug();
    map.rehash(2);
    map.debug();

    if (anna_found) map.erase(anna_iter);
    auto ethan_iter = map.find("Ethan");
    ethan_iter->second = 100;
    map.debug();

    auto iter = ethan_iter;
    ++iter;
    iter->second = 200;
    map.debug();
}
