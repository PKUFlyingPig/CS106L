/*
 * STL HashMap Test Harness
 * Created by Avery Wang (lecturer for 2019-2020 - awvry952@stanford.edu)
 *
 * V1 (spring 2020) - basic, rehash, operators, special members tests
 * V2 (fall 2020) - improve special members test
 * V3 (winter 2021) - added iterator tests, factored out edge case and const-correctness tests
 *                  - added benchmarking tests
 *
 * DO NOT SUBMIT THIS FILE (unless you added extra tests for us)
 */
#include "hashmap.h"
#include "test_settings.cpp"

using namespace std;

#include <vector>
#include <unordered_map>
#include <random>
#include <utility>
#include <algorithm>
#include <sstream>
#include <set>
#include <iomanip>
#include <chrono>       // for chrono timers

// ----------------------------------------------------------------------------------------------
// Global Constants and Type Alises (DO NOT EDIT)
using clock_type = std::chrono::high_resolution_clock;
using ns = std::chrono::nanoseconds;
const std::vector<std::pair<std::string, int> > vec {
    {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1},
    {"D", 10}, {"K", 30}, {"Avery", 2020}, {"Anna", 2020}, {"Ali", 2019}
};

const std::vector<std::string> keys {"A", "B", "C", "Not found"};
template <typename Map1, typename Map2> bool check_map_equal(Map1& map, Map2& answer) {
    if (answer.empty() != map.empty() || answer.size() != map.size()) return false;

    for (const auto& [key, mapped] : answer) {
        if (map.contains(key) == false || map.at(key) != mapped) return false;
    }
    return true;
}

// Runtime assertion check
// Equivalent to CHECK_TRUE

struct VerifyTrueAssertionFailure {
    int line;
};

void VERIFY_TRUE(bool condition, int line) {
    if (!condition) {
        throw VerifyTrueAssertionFailure{line};
    }
}


// ----------------------------------------------------------------------------------------------
/* Milestone 1 Test Cases (DO NOT EDIT) */

void A_basic() {
    /*
    * Verifies basic operations by comparing behavior with std::unordered_map
    *      - default ctor
    *      - size, empty, bucket_count
    *      - contains, at (used as an r-value)
    *
    * Mainly checking that check_map_equal compiles correctly.
    */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    VERIFY_TRUE(map.bucket_count() == 10, __LINE__);
}
void B_insert() {
    /*
    * Verifies functionality of insert.
    */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    VERIFY_TRUE(map.bucket_count() == 10, __LINE__);

    for (const auto& kv_pair : vec) {
        answer.insert(kv_pair);
        map.insert(kv_pair);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    VERIFY_TRUE(map.bucket_count() == 10, __LINE__);
}
void C_clear() {
    /*
    * Tests clear operations, and ensure map is in valid
    * state after a call to clear.
    */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;

    for (size_t j = 0; j < 3; ++j) {
       for (const auto& kv_pair : vec) {
           answer.insert(kv_pair);
           map.insert(kv_pair);
       }

       answer.clear();
       map.clear();

       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}
void D_at() {
    /*
    * Tests whether at correctly returns a reference.
    */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    answer.insert({"A", 42});
    answer.insert({"B", 137});
    answer.insert({"C", -1});

    // at returns a reference, so assigning their return value on the left-hand value should
    // change the contents of the map.
    answer.at("A") = 42;
    answer.at("B") = 137;
    answer.at("A") = -42;
    answer.at("C") = answer.at("A");
    VERIFY_TRUE(answer.at("A") == -42, __LINE__);
    VERIFY_TRUE(answer.at("B") == 137, __LINE__);
    VERIFY_TRUE(answer.at("C") == -42, __LINE__);

    // verify that we can save the reference returned by at.
    auto copy = answer.at("A");
    auto& ref = answer.at("B");
    copy = 0;
    ref = 0;
    VERIFY_TRUE(answer.at("A") == -42, __LINE__);
    VERIFY_TRUE(answer.at("B") == 0, __LINE__);

    // verify that correct exceptions are thrown
    HashMap<std::string, int> map2;
    bool correct_exception = false;
    try {
        map2.at("Exists") = 5;
    } catch (const std::out_of_range& e) {
        correct_exception = true;
    }
    VERIFY_TRUE(correct_exception, __LINE__);
    map2.insert({"Exists", 4});
    VERIFY_TRUE(map2.contains("Exists"), __LINE__);
    VERIFY_TRUE(!map2.contains("Does not exist"), __LINE__);
    correct_exception = true;
    try {
        map2.at("Does not exist") = 5;
    } catch (const std::out_of_range& e) {
        correct_exception = true;
    }
    VERIFY_TRUE(correct_exception, __LINE__);
}


template <typename... Ts>
void ensure_at_return_is_const(Ts&...) {}

template <typename T>
auto ensure_at_return_is_const(const T& map) -> decltype((void) (map.at(0) = 3), void()) {
    VERIFY_TRUE(false, __LINE__);
    // If you run into an error here, this means your return value of const
    // at is a non-const reference. Why is that wrong?

    // Details about this error (it's a hacky sfinae technique):
    // we tried running the following code segment (in the decltype line)
    //      const T& map;
    //      map.at(0) = 3;
    // if the segment compiles, then this function is run (which fails)
    // if the segment does not compile, the variadic template before runs (which passes)
}

void E_at_const_correct() {
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map1;
    VERIFY_TRUE(check_map_equal(map1, answer), __LINE__); // both maps should be empty

    for (const auto& kv_pair : vec) {
       answer.insert(kv_pair);
       map1.insert(kv_pair);
       VERIFY_TRUE(check_map_equal(map1, answer), __LINE__); // both maps have same elements
    }

    // create const references to the maps
    // to see if these const references work correctly
    const auto& c_ref_answer = answer;
    const auto& c_ref_map1 = map1;

    // check if we can still check contents of maps as equal
    // even when dealing with const references
    VERIFY_TRUE(check_map_equal(c_ref_map1, c_ref_answer), __LINE__);

    for (const auto& [key, mapped] : answer) {
        VERIFY_TRUE(c_ref_map1.contains(key), __LINE__);
        VERIFY_TRUE(map1.contains(key), __LINE__);
        VERIFY_TRUE(c_ref_map1.at(key) == mapped, __LINE__); // confirm parameter is a const reference
        VERIFY_TRUE(map1.at(key) == mapped, __LINE__);
        VERIFY_TRUE(check_map_equal(map1, answer), __LINE__);
    }

    map1.erase("A");
    answer.erase("A");

    for (const auto& [key, mapped] : answer) {
        VERIFY_TRUE(c_ref_map1.contains(key), __LINE__);
        VERIFY_TRUE(c_ref_map1.at(key) == mapped, __LINE__); // confirm parameter is a const reference
        VERIFY_TRUE(check_map_equal(map1, answer), __LINE__);
    }

    HashMap<int, int> map3;
    map3.insert({0, 0});
    ensure_at_return_is_const(map3); // confirm return value is a const reference
}

void F_custom_bucket_count() {
    /*
    * Tests ctor taking in num_buckets, while hash function
    * still uses the default. Also tests correct function of load_factor.
    */
    HashMap<int, int> many_buckets(10000);
    HashMap<int, int> one_bucket(1);
    std::unordered_map<int, int> answer;

    for (int i = 0; i < 100; ++i) {
       many_buckets.insert({i, -i});
       one_bucket.insert({i, -i});
       answer.insert({i, -i});
    }

    VERIFY_TRUE(check_map_equal(many_buckets, answer), __LINE__);
    VERIFY_TRUE(check_map_equal(one_bucket, answer), __LINE__);

    VERIFY_TRUE(many_buckets.bucket_count() == 10000, __LINE__);
    VERIFY_TRUE(one_bucket.bucket_count() == 1, __LINE__);

    float epsilon = 0.001;
    VERIFY_TRUE(many_buckets.load_factor() - 0.01 < epsilon, __LINE__);
    VERIFY_TRUE(one_bucket.load_factor() - 100 < epsilon, __LINE__);
}
void G_custom_hash_function() {
    /*
    * Tests ctor taking in a num_buckets and hash function.
    * We use a hash function that distributes similar inputs
    * more randomly across the buckets.
    */
    using K = int;
    using V = char;
    auto identity_shifted = [](const K& key) {
       return (key * 43037 + 52081) % 79229;
    };

    // hey, didn't you use this from assignment 1?
    // now you know what the priority queue decltype comes from!
    HashMap<K, V, decltype(identity_shifted)> map(75, identity_shifted);
    std::unordered_map<K, V> answer;

    for (int i = 0; i < 50; ++i) {
       map.insert({i, 'a' + i});
       answer.insert({i, 'a' + i});
       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}
void H_erase() {
    /*
    * Tests erase operation in combination with basic operations.
    */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;

    for (const auto& kv_pair : vec) {
        answer.insert(kv_pair);
        map.insert(kv_pair);
    }

    // remove one and insert two back at a time
    for (size_t i = 0; i < vec.size(); ++i) {
        auto& [key1, mapped1] = vec[i];
        auto& pair2 = vec[(i+3) % vec.size()];
        answer.erase(key1);
        map.erase(key1);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        answer.erase("Not a key");
        map.erase("Not a key");
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        answer.insert(pair2);
        map.insert(pair2);
    }

    // remove one at a time, until map is empty
    for (size_t i = 0; i < vec.size(); ++i) {
        auto& [key1, mapped1] = vec[(i+3) % vec.size()];
        answer.erase(key1);
        map.erase(key1);

        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}
void I_rehash_basic() {
    /*
    * Verifies external correct after call to M1_Review.
    * Note that this does not actually verify if the linked lists are correct,
    * merely verifies that after call to M1_Review, all method calls are still valid,
    * and bucket_count is correct.
    */
    HashMap<int, int> map;
    std::unordered_map<int, int> answer;
    std::vector<int> vals;

    VERIFY_TRUE(map.bucket_count() == 10, 28);
    for (size_t M1_Review_count : {10, 20, 20, 15, 1000, 2, 1, 2, 1}) {
       map.rehash(M1_Review_count);
       VERIFY_TRUE(map.bucket_count() == M1_Review_count, __LINE__);

       for (size_t i = 0; i < 18; ++i) {
           VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
           map.erase(i);
           answer.erase(i);
       }

       map.clear();
       answer.clear();
       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

       for (size_t i = 1; i < 17; ++i) {
           map.insert({i, i});
           answer.insert({i, i});
       }
    }
    try {
        map.rehash(0);
        VERIFY_TRUE(false, __LINE__); // you didn't throw an exception!
    } catch (const std::out_of_range& e) {
        // Great! You threw the correct exception.
    } catch (const std::exception& e) {
        VERIFY_TRUE(false, __LINE__); // you threw the wrong exception!
    }

}
__attribute__((unused)) void unused_rehash_correct_by_time() {
   /*
    * This was previously a test when rehash was part of the assignment. However, it did not work
    * well with the compiler optimizations, so we will not include this test. If a future lecturer
    * wants to add rehash back into the assignment, they can experiment with this test.
    *
    * Here's to spending lots of time on tests that get removed from the assignment!
    *
    * This test tries to check if you hashed elements to the buckets correctly
    * by adding a specific number of elements, and measuring the time it takes
    * to call contains. The idea is that if bucket 0 is supposed to have 10 times
    * the number of elements as bucket 1, calling contains on an element
    * that should be hashed to bucket 0 but is not present should take 10 times
    * longer that calling contains on an element that should be hashed to bucket 1
    * but is not present.
    *
    * Obviously, this is less than perfect, since it's hard to predict how
    * fast your computer is running linked list operations. There are two parameters
    * below. One is called tolerance (let me know if I'm using the term incorrectly)
    * which determines the amount of leeway we give for any differences from the
    * expected time (0.5 means 50% leeway). The second is trials. You can try
    * increasing that to see if you get better results.
    *
    * Play around with this and let me know if you find something interesting,
    * or if you have suggestions to make this test better. There are also
    * cool number theory results, as hashing is inherently an idea from
    * cryptography. Ask Avery if you are curious!
    */
    using K = int;
    using V = int;
    float tolerance = 0.5;      // makes me feel like an engineer, probably using the term wrong
    int trials = 1000;           // increase if necessary, Central Limit Theorem!

    // in case std::hash<K> is different on your compiler.
    auto identity = [](const K& key) { return key; };

    HashMap<K, V, decltype(identity)> map(6, identity);

    // bucket(elements): 0(0), 1(100), 2(1500), 3(500), 4(1500), 5(6000)
    for (int i = 1; i <= 100; ++i) map.insert({6*i+1, i}); // bucket 1
    for (int i = 1; i <= 1500; ++i) map.insert({6*i+2, i}); // bucket 2
    for (int i = 1; i <= 500; ++i) map.insert({6*i+3, i}); // bucket 3
    for (int i = 1; i <= 1500; ++i) map.insert({6*i+4, i}); // bucket 4
    for (int i = 1; i <= 6000; ++i) map.insert({6*i+5, i}); // bucket 5
    std::unordered_map<int, float> bucket_times_6;

    for (int i = 0; i < 6; ++i) {
        auto start = clock_type::now();
        for (int j = 0; j < trials; ++j) map.contains(i);
        auto end = clock_type::now();
        auto elapsed = std::chrono::duration_cast<ns>(end - start);
        bucket_times_6.insert({i, elapsed.count()});
    }

    map.rehash(3);
    std::unordered_map<int, float> bucket_times_3;
    for (int i = 0; i < 3; ++i) {
        auto start = clock_type::now();
        for (int j = 0; j < trials; ++j) map.contains(i);
        auto end = clock_type::now();
        auto elapsed = std::chrono::duration_cast<ns>(end - start);
        bucket_times_3.insert({i, elapsed.count()});
    }

    map.rehash(2);
    std::unordered_map<int, float> bucket_times_2;
    for (int i = 0; i < 2; ++i) {
        auto start = clock_type::now();
        for (int j = 0; j < trials; ++j) map.contains(i);
        auto end = clock_type::now();
        auto elapsed = std::chrono::duration_cast<ns>(end - start);
        bucket_times_2.insert({i, elapsed.count()});
    }

    // Time to pull out the Chinese Remainder Theorem!
    // take the unique bijection Z/6 -> Z/3 x Z/2
    // bucket(elements) mod 6: 0(0), 1(100), 2(1500), 3(500), 4(1500), 5(6000)
    // bucket(elements) mod 3: 0+3(500), 1+4(1600), 2+5(7500)
    // bucket(elements) mod 2: 0+2+4(3000), 1+3+5(7500)

    float ratio6_10 = float(bucket_times_6[1])/(bucket_times_6[0]+1);
    float ratio6_23 = bucket_times_6[2]/bucket_times_6[3]; // expected: 1500/500
    float ratio6_54 = bucket_times_6[5]/bucket_times_6[4]; // expected: 6000/1500
    float ratio3_10 = bucket_times_3[1]/bucket_times_3[0]; // expected: 1600/500
    float ratio3_21 = bucket_times_3[2]/bucket_times_3[1]; // expected: 7500/1600
    float ratio2_10 = bucket_times_2[1]/bucket_times_2[0]; // expected: 7500/3000

    // experiments are noisy, so let's give you some acceptable tolerance
    VERIFY_TRUE(ratio6_10 > 10, __LINE__);
    VERIFY_TRUE(ratio6_23 < (1+tolerance)*1500/500 && ratio6_23 > 1/(1+tolerance)*1500/500, __LINE__);
    VERIFY_TRUE(ratio6_54 < (1+tolerance)*6000/1500 && ratio6_54 > 1/(1+tolerance)*6000/1500, __LINE__);
    VERIFY_TRUE(ratio3_10 < (1+tolerance)*1600/500 && ratio3_10 > 1/(1+tolerance)*1600/500, __LINE__);
    VERIFY_TRUE(ratio3_21 < (1+tolerance)*7500/1600 && ratio3_21 > 1/(1+tolerance)*7500/1600, __LINE__);
    VERIFY_TRUE(ratio2_10 < (1+tolerance)*7500/3000 && ratio2_10 > 1/(1+tolerance)*7500/3000, __LINE__);

    // fun fact: we had to add an -O0 flag because the compiler was optimizing our code
    // a little too well. Turns out that the runtime all of these is the same with optimization (!)
}
void J_iter_foreach_one_bucket() {
    /* Tests whether iterators are supported via a simple for-each loop
     * Uses begin() and end() of your HashMap, in addition to
     * the ctor and operators =, !=, and ++ of your iterator.
     */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {30, 30}, {140, 140}, {21, 21}
    };

    HashMap<int, int> map10(1);            // can your iterator traverse normal use case?
    for (const auto& pair : questions) {
        map10.insert(pair);
    }
    std::set<std::pair<int, int> > answers10;
    for (const auto& pair : map10) {
        VERIFY_TRUE(answers10.insert(pair).second == true, __LINE__);
    }
    VERIFY_TRUE(questions == answers10, __LINE__);
}
void K_iter_foreach_filled_buckets() {
    /* Tests whether iterators are supported via a simple for-each loop
     * Uses begin() and end() of your HashMap, in addition to
     * the ctor and operators =, !=, and ++ of your iterator.
     */
    std::set<std::pair<int, int> > questions;
    auto identity = [](auto i) {return i;};
    HashMap<int, int, decltype(identity)> map10(6, identity);            // can your iterator traverse normal use case?
    for (int i = 0; i < 10; ++i) {
        map10.insert({i, i});
        questions.insert({i, i});
    }
    std::set<std::pair<int, int> > answers10;
    for (const auto& pair : map10) VERIFY_TRUE(answers10.insert(pair).second == true, __LINE__);
    VERIFY_TRUE(questions == answers10, __LINE__);
}
void L_iter_foreach_split_buckets() {
    /* Tests whether iterators are supported via a simple for-each loop
     * Uses begin() and end() of your HashMap, in addition to
     * the ctor and operators =, !=, and ++ of your iterator.
     */
    std::set<std::pair<int, int> > questions;
    auto identity = [](auto i) {return i;};
    HashMap<int, int, decltype(identity)> map10(6, identity);            // can your iterator traverse normal use case?
    for (int i = 0; i < 20; i += 2) {
        map10.insert({i, i});
        questions.insert({i, i});
    }
    std::set<std::pair<int, int> > answers10;
    for (const auto& pair : map10) VERIFY_TRUE(answers10.insert(pair).second == true, __LINE__);
    VERIFY_TRUE(questions == answers10, __LINE__);
}
void M_iter_foreach_edge() {
    /* Tests a few edge cases for your iterator, such as checking the bounds */
    std::set<std::pair<int, int> > questions {
        {6, 6}, {1, 1}, {2, 2}, {3, 3}, {4, 4}
    };

    HashMap<int, int> map1(1);      // one bucket with all the elements
    HashMap<int, int> map5(5);      // exactly one per bucket
    HashMap<int, int> map10(100);   // a lot of empty buckets
    HashMap<int, int> empty;        // won't add anything to this one

    for (const auto& pair : questions) {
        map1.insert(pair);
        map5.insert(pair);
        map10.insert(pair);
    }
    std::set<std::pair<int, int> > answers1, answers5, answers10;
    for (const auto& pair : map1) VERIFY_TRUE(answers1.insert(pair).second == true, __LINE__);
    for (const auto& pair : map5) VERIFY_TRUE(answers5.insert(pair).second == true, __LINE__);
    for (const auto& pair : map10) VERIFY_TRUE(answers10.insert(pair).second == true, __LINE__);
    for (const auto& pair __attribute__((unused)) : empty) VERIFY_TRUE(false, __LINE__); // this should not run!

    VERIFY_TRUE(questions == answers1, __LINE__);
    VERIFY_TRUE(questions == answers5, __LINE__);
    VERIFY_TRUE(questions == answers10, __LINE__);
}
void N_iter_forward_operators() {
    /* Tests the more advanced operators, that are required
     * since the iterator can be a forward iterator */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 4}, {3, 9}
    };
    // Note to reader: yes, I spent so much time writing these awesome test cases
    // and then decided to make this part optional. What a great use of my spring break.
    // It's not like I have anything else to do, right?


    HashMap<int, int> map;            // can your iterator traverse normal use case?
    for (const auto& pair : questions) {
        map.insert(pair);
    }

    // note: we make no assumptions about which specifc element is in which order!
    auto iter = map.begin();                    // iter->0th element
    VERIFY_TRUE((*iter).first == (*iter).second, __LINE__);   // behavior of * operator
    VERIFY_TRUE(iter-> first * iter->first == iter->second, __LINE__);// behavior of -> operator
    VERIFY_TRUE(iter == iter, __LINE__);                      // behavior of == operator
    VERIFY_TRUE(!(iter != iter), __LINE__);                   // behavior of != operator

    (*iter).second = -1;                        // behavior of * operator as an l-value
    VERIFY_TRUE((*iter).second == -1, __LINE__);    // behavior of * operator as an r-value
    iter->second = -2;                          // behavior of -> operator as an l-value
    VERIFY_TRUE(iter->second == -2, __LINE__);      // behavior of -> operator as an r-value

    // verify correct prefix/postfix behavior (this was very tedious)
    HashMap<int, int>::iterator iter0 = iter; // just to prove why type aliases are helpful
    auto iter1 = ++iter;                      // though auto usually works as well
    auto iter2 = ++iter;
    auto iter3 = ++iter;
    // iter0 = {1, -2}, iter1 = {2, 4}, iter3 = {3, 9}
    VERIFY_TRUE(iter0->first + iter1->first + iter2->first == 6, __LINE__);
    VERIFY_TRUE(iter0->first * iter1->first * iter2->first == 6, __LINE__);
    VERIFY_TRUE(iter0->second + iter1->second + iter2->second == 11, __LINE__);
    VERIFY_TRUE(iter0->second * iter1->second * iter2->second == -72, __LINE__);
    VERIFY_TRUE(iter == map.end(), __LINE__);
    iter = iter0;                       // iter->0
    auto& iter_ref = ++iter;            // iter/iter_ref->1
    VERIFY_TRUE(iter_ref == iter1, __LINE__);
    auto iter_ref_copy = ++iter_ref;    // iter_ref_copy->2, iter/iter_ref->2
    VERIFY_TRUE(iter_ref_copy == iter2, __LINE__);
    VERIFY_TRUE(iter_ref == iter2, __LINE__);

    auto iter_post = iter++;            // iter/iter_ref->3, iter_post->2
    VERIFY_TRUE(iter_post == iter2, __LINE__);
    VERIFY_TRUE(iter == iter3, __LINE__);
    iter_ref = map.begin();             // iter/iter_ref->0
    VERIFY_TRUE(iter == iter0, __LINE__);

    // Big LOL - see if you can actually trace the ++++++ operator.
    auto iter_chain_pre = ++++++iter;   // iter_chain_pre->3, iter/iter_ref->3
    VERIFY_TRUE(iter == iter3, __LINE__);
    VERIFY_TRUE(iter_chain_pre == iter3, __LINE__);
    iter_ref = map.begin();             // iter/iter_ref->0
    auto iter_chain_post = iter++++++;  // iter/iter_ref->1, iter_chain_post->0
    VERIFY_TRUE(iter == iter1, __LINE__);
    VERIFY_TRUE(iter_chain_post == iter0, __LINE__);
    // presumably if you pass the above ones, you probably have it working
    // so I'm not gonna think about what ++iter++++ would be
}
void O_iter_algorithm_copy() {
    /* Stress test of the basic operators of iterators using STL copy */
    HashMap<int, int> map;
    std::unordered_map<int, int> answer;

    for (int i = -17; i < 10; ++i) {
        map.insert({i, i*i});
    }

    std::copy(map.begin(), map.end(), std::inserter(answer, answer.begin()));
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    answer.clear();

    const auto& c_map = map;
    std::copy(c_map.begin(), c_map.end(), std::inserter(answer, answer.begin()));
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
}
void P_iter_algorithm_permutation() {
    /* Stress test of the multipass functionality of forward iterators using STL is_permutation */
    auto identity_shifted = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    HashMap<int, int, decltype(identity_shifted)> map1(1, identity_shifted);
    HashMap<int, int> map2(3);
    std::unordered_map<int, int> answer;

    for (int i = -100; i < 100; ++i) {
        map1.insert({i, i*i});
        map2.insert({i, i*i});
        answer.insert({i, i*i});
    }

    VERIFY_TRUE(std::is_permutation(map1.begin(), map1.end(), answer.begin(), answer.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(answer.begin(), answer.end(), map1.begin(), map1.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map2.begin(), map2.end(), answer.begin(), answer.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(answer.begin(), answer.end(), map2.begin(), map2.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map2.begin(), map2.end(), map1.begin(), map1.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map1.begin(), map1.end(), map2.begin(), map2.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map1.begin(), map1.end(), map1.begin(), map1.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map2.begin(), map2.end(), map2.begin(), map2.end()), __LINE__);


    map1.insert({-1000, 1000});
    map1.rehash(4);
    map2.rehash(5);
    VERIFY_TRUE(!std::is_permutation(map1.begin(), map1.end(), answer.begin(), answer.end()), __LINE__);
    VERIFY_TRUE(!std::is_permutation(answer.begin(), answer.end(), map1.begin(), map1.end()), __LINE__);
    VERIFY_TRUE(!std::is_permutation(map2.begin(), map2.end(), map1.begin(), map1.end()), __LINE__);
    VERIFY_TRUE(!std::is_permutation(map1.begin(), map1.end(), map2.begin(), map2.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map1.begin(), map1.end(), map1.begin(), map1.end()), __LINE__);
    VERIFY_TRUE(std::is_permutation(map2.begin(), map2.end(), map2.begin(), map2.end()), __LINE__);
}
void Q_iter_const_iterator() {
    /* Tests the const-correct of your iterator class by asking for const_iterators via begin() and end() */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {3, 3}
    };

    /* testing const_iterator (iterator to const std::pair) */
    HashMap<int, int> map;
    for (const auto& pair : questions) map.insert(pair);
    const auto& const_map = map;
    std::set<std::pair<int, int> > answers;
    for (const auto& pair : const_map) VERIFY_TRUE(answers.insert(pair).second == true, __LINE__);
    VERIFY_TRUE(questions == answers, __LINE__);

    HashMap<int, int>::const_iterator iter = const_map.begin();

    VERIFY_TRUE((*iter).first == (*iter).second, __LINE__);   // behavior of * operator
    VERIFY_TRUE(iter->first == iter->second, __LINE__);       // behavior of -> operator
    VERIFY_TRUE(iter == iter, __LINE__);                      // behavior of == operator
    VERIFY_TRUE(!(iter != iter), __LINE__);                   // behavior of != operator

    VERIFY_TRUE(iter->second == (*iter).second, __LINE__);
    auto iter1 = ++iter;
    auto iter2 = ++iter;
    auto iter3 = iter++;
    VERIFY_TRUE(iter == const_map.end(), __LINE__);
    VERIFY_TRUE(iter3 == iter2, __LINE__);
    VERIFY_TRUE(iter1 != iter2, __LINE__);

    /* We could have the entire operator tests here, though that feels unnecessary */
}
void R_iter_const_correct() {
    /* Test the distinction between const iterator and const_iterator */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {3, 3}
    };

    HashMap<int, int> map;
    for (const auto& pair : questions) map.insert(pair);

    /* test behavior of const iterator */
    HashMap<int, int>::iterator iter = map.begin();
    const HashMap<int, int>::iterator c_iter = map.begin();
    const HashMap<int, int>::iterator& copy = iter;
    const HashMap<int, int>::iterator& copy_next = ++iter;

    VERIFY_TRUE(map.begin() == c_iter, __LINE__);
    VERIFY_TRUE(copy == iter, __LINE__);
    VERIFY_TRUE(copy_next == iter, __LINE__);
    VERIFY_TRUE(c_iter != iter, __LINE__);

    // the iterator is const, but the stuff the iterator points to is not const.
    (*c_iter).second = -1;                                   // behavior of * operator as an l-value
    VERIFY_TRUE((*c_iter).second == -1, __LINE__);              // behavior of * operator as an r-value
    c_iter->second = -2;                                     // behavior of -> operator as an l-value
    VERIFY_TRUE(c_iter->second == -2, __LINE__);                // behavior of -> operator as an r-value

    // these should not compile:
    // *iter = {0, 0};  // *iter is a std::pair<const K, M>, since K is const, = is deleted
    // ++c_iter;        // ++ is non-const

    VERIFY_TRUE(++++iter == map.end(), __LINE__);

    /* test behavior of const const_iterator */
    const auto& const_map = map;
    HashMap<int, int>::const_iterator const_iter = const_map.begin();
    const HashMap<int, int>::const_iterator c_const_iter_next = ++const_map.begin();
    const HashMap<int, int>::const_iterator c_const_iter = const_map.begin();

    // the key here is that these should compile.
    ++const_iter;
    VERIFY_TRUE((*c_const_iter).second == -2, __LINE__);
    VERIFY_TRUE(c_const_iter->second == -2, __LINE__);
    VERIFY_TRUE(const_iter == c_const_iter_next, __LINE__);
    VERIFY_TRUE(c_const_iter == const_map.begin(), __LINE__);

    // these should not compile:
    // ++c_const_iter;
    // c_const_iter->second = 2;
    // const_iter->second = 2;

    // TODO: create SFINAE tests for testing iterator code that should not compile
}
void S_iter_const_conversion() {
    /* Tests whether an iterator can implicitly convert to a const_iterator */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {3, 3}
    };

    HashMap<int, int> map;
    const auto& cmap = map;
    for (const auto& pair : questions) map.insert(pair);

    HashMap<int, int>::const_iterator c_iter = cmap.begin();
    HashMap<int, int>::iterator iter = map.begin();
    HashMap<int, int>::const_iterator converted_iter = iter; // this results in an implicit conversion to const_iterator
    VERIFY_TRUE(converted_iter == c_iter, __LINE__);
    ++c_iter;
    ++iter;
    ++converted_iter;

    VERIFY_TRUE(static_cast<HashMap<int, int>::const_iterator>(iter) == c_iter, __LINE__);
    VERIFY_TRUE(static_cast<HashMap<int, int>::const_iterator>(iter) == converted_iter, __LINE__);

    ++iter;
    ++iter;
    HashMap<int, int>::const_iterator converted_end = iter;
    VERIFY_TRUE(iter == map.end(), __LINE__);
    VERIFY_TRUE(converted_end == cmap.end(), __LINE__);
}
void T_iter_find_member() {
    /*
    * Verifies functionality of the find member function, which uses the iterator class.
    */
    HashMap<std::string, int> map;

    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
    }

    {
        // test the non-const version of HashMap::find
        auto iter_avery = map.find("Avery");
        VERIFY_TRUE(iter_avery->first == "Avery", __LINE__);
        VERIFY_TRUE(iter_avery->second == 2020, __LINE__);

        auto iter_ali = map.find("Ali");
        VERIFY_TRUE(iter_ali->first == "Ali", __LINE__);
        VERIFY_TRUE(iter_ali->second == 2019, __LINE__);

        iter_ali->second = 2018;
        auto iter_repeat = map.find("Ali");
        VERIFY_TRUE(iter_repeat->first == "Ali", __LINE__);
        VERIFY_TRUE(iter_repeat->second == 2018, __LINE__);
        VERIFY_TRUE(iter_ali == iter_repeat, __LINE__);
        VERIFY_TRUE(iter_avery != iter_ali, __LINE__);
        iter_ali->second = 2019;

        auto iter_not_found = map.find("Not found");
        VERIFY_TRUE(iter_not_found == map.end(), __LINE__);
    }

    {
         // test the const version of HashMap::find
        const auto& cmap = map;
        auto iter_avery = cmap.find("Avery");
        VERIFY_TRUE(iter_avery->first == "Avery", __LINE__);
        VERIFY_TRUE(iter_avery->second == 2020, __LINE__);

        auto iter_ali = cmap.find("Ali");
        VERIFY_TRUE(iter_ali->first == "Ali", __LINE__);
        VERIFY_TRUE(iter_ali->second == 2019, __LINE__);
        auto iter_repeat = cmap.find("Ali");
        VERIFY_TRUE(iter_ali == iter_repeat, __LINE__);
        VERIFY_TRUE(iter_avery != iter_ali, __LINE__);

        auto iter_not_found = cmap.find("Not found");
        VERIFY_TRUE(iter_not_found == cmap.end(), __LINE__);
    }
}
void U_iter_insert_member() {
    /*
    * Verifies functionality of the insert member function, returns an iterator.
    * The previous test B ignores the return value.
    */
    HashMap<std::string, int> map;

    auto [iter_avery0, added0] = map.insert({"Avery", 3});
    VERIFY_TRUE(added0, __LINE__);
    VERIFY_TRUE(iter_avery0 == map.begin(), __LINE__);
    VERIFY_TRUE(iter_avery0->first == "Avery", __LINE__);
    VERIFY_TRUE(iter_avery0->second == 3, __LINE__);

    auto [iter_avery1, added1] = map.insert({"Avery", 3});
    VERIFY_TRUE(!added1, __LINE__);
    VERIFY_TRUE(iter_avery0 == iter_avery1, __LINE__);
    iter_avery1->second = 4;
    VERIFY_TRUE(iter_avery0->second == 4, __LINE__);

    auto [iter_anna2, added2] = map.insert({"Anna", 2});
    VERIFY_TRUE(added2, __LINE__);
    VERIFY_TRUE(iter_anna2->first == "Anna", __LINE__);
    VERIFY_TRUE(iter_anna2->second == 2, __LINE__);
}

void V_iter_erase_member() {
    /*
    * Verifies functionality of the erase and insert. Note that this erase test takes in a parameter
    * that is a const_iterator, different from test G which only takes in a key.
    */
    HashMap<std::string, int> map;

    map.insert({"Avery", 3});
    auto iter = map.erase(map.begin());
    VERIFY_TRUE(iter == map.end(), __LINE__);

    auto [iter_avery0, added0] = map.insert({"Avery", 3});
    VERIFY_TRUE(added0, __LINE__);
    VERIFY_TRUE(iter_avery0 == map.begin(), __LINE__);
    VERIFY_TRUE(iter_avery0->first == "Avery", __LINE__);
    VERIFY_TRUE(iter_avery0->second == 3, __LINE__);

    auto result = map.erase(iter_avery0);
    VERIFY_TRUE(result == map.end(), __LINE__);
    VERIFY_TRUE(map.empty(), __LINE__);

    auto [iter_avery1, added1] = map.insert({"Avery", 4});
    VERIFY_TRUE(added0, __LINE__);
    VERIFY_TRUE(iter_avery1->second == 4, __LINE__);

    auto [iter_anna2, added2] = map.insert({"Anna", 2});
    VERIFY_TRUE(added2, __LINE__);
    VERIFY_TRUE(iter_anna2->first == "Anna", __LINE__);
    VERIFY_TRUE(iter_anna2->second == 2, __LINE__);

    auto next = map.erase(iter_anna2);
    VERIFY_TRUE(next == map.end() || next == iter_avery1, __LINE__);
    VERIFY_TRUE(iter_avery1 == map.begin(), __LINE__);
    auto next_avery = map.erase(iter_avery1);
    VERIFY_TRUE(next_avery == map.end(), __LINE__);
    VERIFY_TRUE(map.begin() == map.end(), __LINE__);
}

#if RUN_TEST_2A
void A_range_ctor_basic() {
    /* Simple test of the range ctor taking in two iterators to another collection */
    std::vector<std::pair<std::string, int>> values {
        {"Ignore me", 100}, {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
    };
    std::unordered_map<std::string, int> answer {values.begin()++, values.end()}; // skips the first "Ignore me" pair
    HashMap<std::string, int> map {values.begin()++, values.end()};

    VERIFY_TRUE(check_map_equal(map, answer), __LINE__); // maps should be equal, if not, you didn't transfer all the elements!
}
#endif

#if RUN_TEST_2B
void B_range_ctor_edge() {
    /* Simple test of the range ctor taking in two iterators to another collection */
    std::vector<std::pair<std::string, int>> values {
        {"Ignore me", 100}
    };
    {
        // range has exactly one element
        std::unordered_map<std::string, int> answer {values.begin(), values.end()};
        HashMap<std::string, int> map {values.begin(), values.end()};
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
    {
        // range has no elements (first == last) because two ends were passed in
        std::unordered_map<std::string, int> answer {values.end(), values.end()};
        HashMap<std::string, int> map {values.end(), values.end()};
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
    {
        // range has no elements (first == last) since the container was empty
        std::vector<std::pair<std::string, int>> empty;
        std::unordered_map<std::string, int> answer {empty.begin(), empty.end()};
        HashMap<std::string, int> map {empty.begin(), empty.end()};
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

}
#endif

#if RUN_TEST_2C
void C_initializer_list_ctor_basic() {
    /* Tests initializer_list via a simple example */
    std::unordered_map<std::string, int> answer {
        {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
    };

    HashMap<std::string, int> map {
        {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
    };

    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
}
#endif

#if RUN_TEST_2D
void D_initializer_list_ctor_edge() {
    /* Tests initializer_list via a few edge cases example */
    std::unordered_map<std::string, int> answer {{"A", 3}};
    HashMap<std::string, int> map {{"A", 3}};
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

    std::unordered_map<std::string, int> empty_answer ({});
    HashMap<std::string, int>  empty ({});
    VERIFY_TRUE(check_map_equal(empty, empty_answer), __LINE__);
}
#endif

#if RUN_TEST_3A
void A_index_operator() {
    /*
     * Tests the indexing operator to ensure it has the functionality of at(),
     * and also supports auto-insertion.
     */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    VERIFY_TRUE(answer["Autoinsert"] == map["Autoinsert"], __LINE__);
    VERIFY_TRUE(answer["Autoinsert"] == int(), __LINE__);
    for (const auto& [key, value] : vec) {
       answer[key] = value;
       map[key] = value;
       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
       VERIFY_TRUE(answer["Autoinsert"] == map["Autoinsert"], __LINE__);
    }
}
#endif

#if RUN_TEST_3B
template <typename... Ts>
void ensure_no_const_index_op(Ts&...) {}

template <typename T>
auto ensure_no_const_index_op(const T& map) -> decltype((void) (map[0]), void()) {
    VERIFY_TRUE(false, __LINE__);
    // The idea of this test is that there are two overloads for ensure_no_const_index_op.
    // The compiler tries this current overload, which only compiles
    // if map[0] compiles, but it is not supposed to
    // because map a const reference and map[0] may autoinsert.

    // If map[0] does not compile (as expected), then
    // the varadic template overload above is called, and no runtime assertion happens.

    // It's tricky to test "this code should not compile" - this is a hacky way to check that.
}

void B_index_const_correct() {
    HashMap<int, int> map;
    ensure_no_const_index_op(map);

    // check for parameter const correct
    const int key = 3;
    map[key] = 6;
    int r_key = 5;
    map[r_key] = 2;
    map[r_key] = key;
}
#endif

#if RUN_TEST_3C
void C_stream_insert_operator() {
    /*
     * Tries to insert the map into various streams.
     * The format is described on the handout:
     * {Avery:2, Anna:3}
     * {}
     * {Avery:2}
     */
    HashMap<std::string, int> map(1);

    // Test 1: print empty map
    std::ostringstream oss1;
    oss1 << map;
    VERIFY_TRUE(oss1.str() == "{}", __LINE__);

    // Test 2: print map with a single element
    std::ostringstream oss2;
    map.insert({"Anna", 2});
    oss2 << map;
    VERIFY_TRUE(oss2.str() == "{Anna:2}", __LINE__);

    // Test 3: print map with two elements
    std::ostringstream oss3;
    map.insert({"Avery", 3});
    oss3 << map;
    VERIFY_TRUE(oss3.str() == "{Avery:3, Anna:2}" || oss3.str() == "{Anna:2, Avery:3}", __LINE__);
    auto s = oss3.str();

    // Test 4: print map after a call to erase
    std::ostringstream oss4;
    map.erase("Anna");
    map.at("Avery") = 4;
    oss4 << map;
    VERIFY_TRUE(oss4.str() == "{Avery:4}", __LINE__);

    // Test 5: print map that is key/value pairs reversed
    std::ostringstream oss5;
    HashMap<int, std::string> reverse;
    reverse.insert({3, "Avery"});
    reverse.insert({2, "Anna"});
    oss5 << reverse;
    VERIFY_TRUE(oss5.str() == "{3:Avery, 2:Anna}" || oss5.str() == "{2:Anna, 3:Avery}", __LINE__);

    // Test 6: check ability to chain printing HashMaps to streams.
    std::ostringstream oss;
    oss << "A" << map << 3 << reverse << '\n' << map << map << std::endl;
    VERIFY_TRUE(oss.str() == "A{Avery:4}3{3:Avery, 2:Anna}\n{Avery:4}{Avery:4}\n" ||
                oss.str() == "A{Avery:4}3{2:Anna, 3:Avery}\n{Avery:4}{Avery:4}\n", __LINE__);

    // Test 7: stress test to see if the correct elements are being printed
    // Uses fixed 3-digit numbers as the key and values, then prints to stream
    // and checks whether output stream has correct length.
    HashMap<int, int> large;
    for (int i = 100; i <= 999; i++) {
        large.insert({i, i});
    }
    // 900 elements of 3-digit numbers
    // 3 x 900 = 2700 characters for keys
    // 3 x 900 = 2700 characters for values
    // 1 x 900 = 900 characters for colons
    // 2 x 899 = 1798 characters for comma and space between each key/value pair
    // 2 x 1 = 2 characters for beginning and end paranthesis

    for (int num_buckets : {1, 3, 7, 10, 100, 10000}) {
        large.rehash(num_buckets);
        std::ostringstream oss7;
        oss7 << large;
        VERIFY_TRUE(oss7.str().size() == 2700 + 2700 + 900 + 1798 + 2, __LINE__);
    }

}
#endif

#if RUN_TEST_3D
void D_stream_insert_const_correct() {
    /*
     * The exact same as the previous test, except the printing
     * happens through const references to non-const streams.
     */
    HashMap<std::string, int> map(1);
    const auto& cmap = map;

    // Test 1: print empty map
    std::ostringstream oss1;
    oss1 << cmap;
    VERIFY_TRUE(oss1.str() == "{}", __LINE__);

    // Test 2: print map with a single element
    std::ostringstream oss2;
    map.insert({"Anna", 2});
    oss2 << cmap;
    VERIFY_TRUE(oss2.str() == "{Anna:2}", __LINE__);

    // Test 3: print map with two elements
    std::ostringstream oss3;
    map.insert({"Avery", 3});
    oss3 << cmap;
    VERIFY_TRUE(oss3.str() == "{Avery:3, Anna:2}" || oss3.str() == "{Anna:2, Avery:3}", __LINE__);
    auto s = oss3.str();

    // Test 4: print map after a call to erase
    std::ostringstream oss4;
    map.erase("Anna");
    map.at("Avery") = 4;
    oss4 << cmap;
    VERIFY_TRUE(oss4.str() == "{Avery:4}", __LINE__);

    // Test 5: print map that is key/value pairs reversed
    std::ostringstream oss5;
    HashMap<int, std::string> reverse;
    const auto& creverse = reverse;
    reverse.insert({3, "Avery"});
    reverse.insert({2, "Anna"});
    oss5 << creverse;
    VERIFY_TRUE(oss5.str() == "{3:Avery, 2:Anna}" || oss5.str() == "{2:Anna, 3:Avery}", __LINE__);

    // Test 6: check ability to chain printing HashMaps to streams.
    std::ostringstream oss;
    oss << "A" << cmap << 3 << creverse << '\n' << cmap << cmap << std::endl;
    VERIFY_TRUE(oss.str() == "A{Avery:4}3{3:Avery, 2:Anna}\n{Avery:4}{Avery:4}\n" ||
                oss.str() == "A{Avery:4}3{2:Anna, 3:Avery}\n{Avery:4}{Avery:4}\n", __LINE__);

    // Test 7: stress test to see if the correct elements are being printed
    // Uses fixed 3-digit numbers as the key and values, then prints to stream
    // and checks whether output stream has correct length.
    HashMap<int, int> large;
    for (int i = 100; i <= 999; i++) {
        large.insert({i, i});
    }
    // 900 elements of 3-digit numbers
    // 3 x 900 = 2700 characters for keys
    // 3 x 900 = 2700 characters for values
    // 1 x 900 = 900 characters for colons
    // 2 x 899 = 1798 characters for comma and space between each key/value pair
    // 2 x 1 = 2 characters for beginning and end paranthesis

    const auto& clarge = large;
    for (int num_buckets : {1, 3, 7, 10, 100, 10000}) {
        large.rehash(num_buckets);
        std::ostringstream oss7;
        oss7 << clarge;
        VERIFY_TRUE(oss7.str().size() == 2700 + 2700 + 900 + 1798 + 2, __LINE__);
    }
}
#endif

#if RUN_TEST_3E
void E_equality_operator() {
    /* Checks functionality of == and != */
    HashMap<int, int> map1(100);
    HashMap<int, int> map2(1);
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);

    // at this point we're assuming that all your operations
    // already work, so we're just testing whether == is correct.

    // Insert exact same elements
    for (int i = -1; i < 100; ++i) {
        map1.insert({i, i*i});
        map2.insert({i, i*i});
        VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
        VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    }

    // Change the two maps' elements in a different order.
    // This means the the maps won't be equal until the very end
    for (int i = 0; i < 99; ++i) {
        map1.at(i) = -i*i;
        map2.at(99-i) = -(99-i)*(99-i);
        map1.rehash(i+1);
        map2.rehash(150+i);
        VERIFY_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2, __LINE__);
        VERIFY_TRUE(!(map1 == map2) && !(map2 == map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    }
    map1.at(99) = -99*99;
    map2.at(0) = 0;
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);

    // Try that same thing again, but insert a ton of rehash calls between them.
    for (int i = 0; i < 99; ++i) {
        map1.erase(i);
        map2.erase(99-i);
        map1.rehash(i+150);
        map2.rehash(1+i);
        VERIFY_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2, __LINE__);
        VERIFY_TRUE(!(map1 == map2) && !(map2 == map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    }
    map1.erase(99);
    map2.erase(0);
    map1.rehash(1);
    map2.rehash(1);
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    // consistency after a call to clear
    map1.clear();
    map2.clear();
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
}
#endif

#if RUN_TEST_3F
void F_equality_const_correct() {
    /*
     * Checks that your HashMap class is const-correct.
     * The hard part about this test is not getting it to pass.
     * It is to get it to compile!
     */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map1;
    HashMap<std::string, int> map2;
    VERIFY_TRUE(check_map_equal(map1, answer), __LINE__);

    for (const auto& kv_value : vec) {
       answer.insert(kv_value);
       map1.insert(kv_value);
       map2.insert(kv_value);
       VERIFY_TRUE(check_map_equal(map1, answer), __LINE__);
    }

    // create const references to the maps
    // to see if these const references work correctly
    const auto& c_ref_map1 = map1;
    const auto& c_ref_map2 = map2;

    // Check const correct of == and != operator

    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(c_ref_map1 == map2 && map2 == c_ref_map1 && c_ref_map1 == c_ref_map1, __LINE__);
    VERIFY_TRUE(c_ref_map2 == map1 && map1 == c_ref_map2 && c_ref_map2 == c_ref_map2, __LINE__);
    VERIFY_TRUE(c_ref_map2 == c_ref_map1 && c_ref_map1 == c_ref_map2, __LINE__);
    VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    VERIFY_TRUE(!(c_ref_map1 != map2) && !(map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map1), __LINE__);
    VERIFY_TRUE(!(c_ref_map2 != map1) && !(map1 != c_ref_map2) && !(c_ref_map2 != c_ref_map2), __LINE__);
    VERIFY_TRUE(!(c_ref_map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map2), __LINE__);

    map1.erase("A");
    VERIFY_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(c_ref_map1 != map2 && map2 != c_ref_map1 && c_ref_map1 == c_ref_map1, __LINE__);
    VERIFY_TRUE(c_ref_map2 != map1 && map1 != c_ref_map2 && c_ref_map2 == c_ref_map2, __LINE__);
    VERIFY_TRUE(c_ref_map2 != c_ref_map1 && c_ref_map1 != c_ref_map2, __LINE__);
    VERIFY_TRUE(!(map1 == map2) && !(map2 == map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    VERIFY_TRUE(!(c_ref_map1 == map2) && !(map2 == c_ref_map1) && !(c_ref_map1 != c_ref_map1), __LINE__);
    VERIFY_TRUE(!(c_ref_map2 == map1) && !(map1 == c_ref_map2) && !(c_ref_map2 != c_ref_map2), __LINE__);
    VERIFY_TRUE(!(c_ref_map2 == c_ref_map1) && !(c_ref_map1 == c_ref_map2), __LINE__);

    map2.erase("A");
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(c_ref_map1 == map2 && map2 == c_ref_map1 && c_ref_map1 == c_ref_map1, __LINE__);
    VERIFY_TRUE(c_ref_map2 == map1 && map1 == c_ref_map2 && c_ref_map2 == c_ref_map2, __LINE__);
    VERIFY_TRUE(c_ref_map2 == c_ref_map1 && c_ref_map1 == c_ref_map2, __LINE__);
    VERIFY_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2), __LINE__);
    VERIFY_TRUE(!(c_ref_map1 != map2) && !(map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map1), __LINE__);
    VERIFY_TRUE(!(c_ref_map2 != map1) && !(map1 != c_ref_map2) && !(c_ref_map2 != c_ref_map2), __LINE__);
    VERIFY_TRUE(!(c_ref_map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map2), __LINE__);
}
#endif

#if RUN_TEST_4A
void A_copy_ctor_basic() {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;

    // populate our initial map
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }
    // create copies via copy constructor
    HashMap<std::string, int> copy_constructed{map};
    std::unordered_map<std::string, int> copy_constructed_answer{answer};
    VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);


    for (const auto& [key, mapped] : vec) {
        copy_constructed.at(key)++;
        copy_constructed_answer.at(key)++;
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        copy_constructed.erase(key);
        copy_constructed_answer.erase(key);
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    copy_constructed.clear();
    copy_constructed_answer.clear();
    VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

    // check integrity of original map and answer
    for (const auto& [key, mapped] : vec) {
        map.at(key)++;
        answer.at(key)++;
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        map.erase(key);
        answer.erase(key);
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}
#endif

#if RUN_TEST_4B
void B_copy_assign_basic() {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }

    HashMap<std::string, int> copy_assign;
    std::unordered_map<std::string, int> copy_assign_answer;
    copy_assign = map;
    copy_assign_answer = answer;
    VERIFY_TRUE(check_map_equal(copy_assign, copy_assign_answer), __LINE__);
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

    for (const auto& [key, mapped] : vec) {
        copy_assign.at(key)++;
        copy_assign_answer.at(key)++;
        VERIFY_TRUE(check_map_equal(copy_assign, copy_assign_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        copy_assign.erase(key);
        copy_assign_answer.erase(key);
        VERIFY_TRUE(check_map_equal(copy_assign, copy_assign_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    copy_assign.clear();
    copy_assign_answer.clear();
    VERIFY_TRUE(check_map_equal(copy_assign, copy_assign_answer), __LINE__);
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

    // check integrity of original map and answer
    for (const auto& [key, mapped] : vec) {
        map.at(key)++;
        answer.at(key)++;
        VERIFY_TRUE(check_map_equal(copy_assign, copy_assign_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        map.erase(key);
        answer.erase(key);
        VERIFY_TRUE(check_map_equal(copy_assign, copy_assign_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}
#endif

#if RUN_TEST_4C
void C_copy_edge() {
    {
        /* 1. When map is empty, copy construct and assign */
        HashMap<std::string, int> map;
        map.rehash(1);
        HashMap<std::string, int> copy_constructed{map};

        std::unordered_map<std::string, int> map_answer;
        map_answer.rehash(1);
        std::unordered_map<std::string, int> copy_constructed_answer{map_answer};

        VERIFY_TRUE(check_map_equal(map, map_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);

        map.insert({"Avery", 3});
        map_answer.insert({"Avery", 3});
        copy_constructed.insert({"Anna", 2});
        copy_constructed_answer.insert({"Anna", 2});

        VERIFY_TRUE(check_map_equal(map, map_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);

        HashMap<std::string, int> copy_assigned;
        std::unordered_map<std::string, int> copy_assigned_answer;
        copy_assigned = copy_constructed;
        copy_assigned_answer = copy_constructed_answer;
        copy_assigned.insert({"Ali", 1});
        copy_assigned_answer.insert({"Ali", 1});

        VERIFY_TRUE(check_map_equal(map, map_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(map, map_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(copy_constructed, copy_constructed_answer), __LINE__);

    }
    {
        /* 2. Basic self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        HashMap<std::string, int> map;
        std::unordered_map<std::string, int> answer;
        map = map;
        answer = answer;

        for (const auto& kv_pair : vec) {
            map.insert(kv_pair);
            answer.insert(kv_pair);
        }
        map = map;
        answer = answer;
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        for (const auto& [key, mapped] : vec) {
            map.at(key)++;
            answer.at(key)++;
            map = map;
            answer = answer;
            VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
        }

        for (const auto& key : {"A", "B", "Not found again!"}) {
            map.erase(key);
            answer.erase(key);
            map = map;
            answer = answer;
            VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
        }

        map.clear();
        answer.clear();
        map = map;
        answer = answer;
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!
    }

    {
        /* 3. Expanded self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        HashMap<std::string, int> map1;
        HashMap<std::string, int> map2;
        map1.insert({"A", 1});
        map1.insert({"B", 2});
        map2.insert({"C", 3});
        std::unordered_map<std::string, int> answer1 {{"A", 1}, {"B", 2}};
        std::unordered_map<std::string, int> answer2 {{"C", 3}};

        map1 = map2 = map2 = map1;
        answer1 = answer2 = answer2 = answer1;
        VERIFY_TRUE(check_map_equal(map1, answer1), __LINE__);
        VERIFY_TRUE(check_map_equal(map2, answer2), __LINE__);

        (map1 = map1) = map1 = (map1 = map1 = map1);
        (answer1 = answer1) = answer1 = (answer1 = answer1 = answer1);
        VERIFY_TRUE(check_map_equal(map1, answer1), __LINE__);
        VERIFY_TRUE(check_map_equal(map2, answer2), __LINE__);

        map1 = map2 = map1 = (map1 = map1 = map2) = (map2 = map1) = map1;
        answer1 = answer2 = answer1 = (answer1 = answer1 = answer2) = (answer2 = answer1) = answer1;
        VERIFY_TRUE(check_map_equal(map1, answer1), __LINE__);
        VERIFY_TRUE(check_map_equal(map2, answer2), __LINE__);

        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!
    }
}
#endif

#if RUN_TEST_4D
void D_move_ctor_basic() {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;

    // populate our initial map
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }
    // create copies via move constructor
    HashMap<std::string, int> move_constructed{std::move(map)};
    std::unordered_map<std::string, int> move_constructed_answer{std::move(answer)};
    VERIFY_TRUE(check_map_equal(move_constructed, move_constructed_answer), __LINE__);

    for (const auto& [key, mapped] : vec) {
        move_constructed.at(key)++;
        move_constructed_answer.at(key)++;
        VERIFY_TRUE(check_map_equal(move_constructed, move_constructed_answer), __LINE__);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        move_constructed.erase(key);
        move_constructed_answer.erase(key);
        VERIFY_TRUE(check_map_equal(move_constructed, move_constructed_answer), __LINE__);
    }

    move_constructed.clear();
    move_constructed_answer.clear();
    VERIFY_TRUE(check_map_equal(move_constructed, move_constructed_answer), __LINE__);


}
#endif

#if RUN_TEST_4E
void E_move_assign_basic() {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }

    HashMap<std::string, int> move_assign;
    std::unordered_map<std::string, int> move_assign_answer;
    move_assign = std::move(map);
    move_assign_answer = std::move(answer);
    VERIFY_TRUE(check_map_equal(move_assign, move_assign_answer), __LINE__);

    for (const auto& [key, mapped] : vec) {
        move_assign.at(key)++;
        move_assign_answer.at(key)++;
        VERIFY_TRUE(check_map_equal(move_assign, move_assign_answer), __LINE__);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        move_assign.erase(key);
        move_assign_answer.erase(key);
        VERIFY_TRUE(check_map_equal(move_assign, move_assign_answer), __LINE__);
    }

    move_assign.clear();
    move_assign_answer.clear();
    VERIFY_TRUE(check_map_equal(move_assign, move_assign_answer), __LINE__);
}
#endif

#if RUN_TEST_4F
void F_move_edge() {
    {
        /* 1. When map is empty, copy construct and assign */
        HashMap<std::string, int> map;
        HashMap<std::string, int> move_constructed{std::move(map)};
        HashMap<std::string, int> move_assign;
        move_assign = std::move(map);

        std::unordered_map<std::string, int> map_answer;
        std::unordered_map<std::string, int> move_constructed_answer{map_answer};
        std::unordered_map<std::string, int> move_assign_answer;
        move_assign_answer = std::move(map_answer);
        VERIFY_TRUE(check_map_equal(move_constructed, move_constructed_answer), __LINE__);
        VERIFY_TRUE(check_map_equal(move_assign, move_assign_answer), __LINE__);
    }
    {
        /* 2. Basic self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-move"
        HashMap<std::string, int> map;
        std::unordered_map<std::string, int> answer;

        map = std::move(map);
        answer = std::move(answer);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
        for (const auto& kv_pair : vec) {
            map.insert(kv_pair);
            answer.insert(kv_pair);
        }
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
        map = std::move(map);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        for (const auto& [key, mapped] : vec) {
            map.at(key)++;
            answer.at(key)++;
            map = std::move(map);
            VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
        }

        for (const auto& key : {"A", "B", "Not found again!"}) {
            map.erase(key);
            answer.erase(key);
            map = std::move(map);
            VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
        }

        map.clear();
        answer.clear();
        map = std::move(map);
        answer = std::move(answer);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!
    }

    {
        /* 3. Expanded self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wself-move"
        HashMap<std::string, int> map1;
        HashMap<std::string, int> map2;
        map1.insert({"A", 1});
        map1.insert({"B", 2});
        map2.insert({"C", 3});
        std::unordered_map<std::string, int> answer1 {{"A", 1}, {"B", 2}};
        std::unordered_map<std::string, int> answer2 {{"C", 3}};

        map1 = std::move(map2) = std::move(map2) = std::move(map1);
        answer1 = std::move(answer2) = std::move(answer2) = std::move(answer1);
        VERIFY_TRUE(check_map_equal(map1, answer1), __LINE__);
        VERIFY_TRUE(check_map_equal(map2, answer2), __LINE__);

        (map1 = std::move(map1)) = std::move(std::move(map1) = (std::move(map1) = map1 = std::move(map1)));
        VERIFY_TRUE(check_map_equal(map1, answer1), __LINE__);
        VERIFY_TRUE(check_map_equal(map2, answer2), __LINE__);
        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!

        // Check that the map is still assignable from.
        HashMap<std::string, int> copy = map1;
        HashMap<std::string, int> temp, new_assign;
        temp = std::move(map1);
        new_assign = std::move(temp); // we aren't supposed to use temp again since we moved from it!
        VERIFY_TRUE(check_map_equal(new_assign, copy), __LINE__); // 3/16 Avery: fixed from temp -> copy
    }
}
#endif

#if RUN_TEST_4G
void G_move_ctor_time() {
    HashMap<int, int> empty_map(1);
    HashMap<int, int> tiny_map(1);
    HashMap<int, int> small_map(1);
    HashMap<int, int> big_map(1);
    HashMap<int, int> huge_map(1);

    for (size_t i = 0; i < 10; ++i) {
        tiny_map.insert({i, i});
    }

    for (size_t i = 0; i < 100; ++i) {
        small_map.insert({i, i});
    }

    for (size_t i = 0; i < 1000; ++i) {
        big_map.insert({i, i});
    }

    for (size_t i = 0; i < 10000; ++i) {
        huge_map.insert({i, i});
    }

    // hey compiler, stop optimizing this out. you made me do this
    __attribute__((unused)) auto start_empty = clock_type::now();
    __attribute__((unused)) HashMap<int, int> move_empty = std::move(empty_map);
    __attribute__((unused)) auto end_empty = clock_type::now();

    auto start_tiny = clock_type::now();
    HashMap<int, int> move_assigned_tiny = std::move(tiny_map);
    auto end_tiny = clock_type::now();
    ns tiny_time = std::chrono::duration_cast<ns>(end_tiny - start_tiny);

    auto start_small = clock_type::now();
    HashMap<int, int> move_assigned_small = std::move(small_map);
    auto end_small = clock_type::now();
    ns small_time = std::chrono::duration_cast<ns>(end_small - start_small);

    auto start_big = clock_type::now();
    HashMap<int, int> move_assigned_big = std::move(big_map);
    auto end_big = clock_type::now();
    ns big_time = std::chrono::duration_cast<ns>(end_big - start_big);

    auto start_huge = clock_type::now();
    HashMap<int, int> move_assigned_huge = std::move(huge_map);
    auto end_huge = clock_type::now();
    ns huge_time = std::chrono::duration_cast<ns>(end_huge - start_huge);

    std::cout << "Move    10 elements: " << tiny_time.count() << " ns" << std::endl;
    std::cout << "Move   100 elements: " << small_time.count() << " ns" << std::endl;
    std::cout << "Move  1000 elements: " << big_time.count() << " ns" << std::endl;
    std::cout << "Move 10000 elements: " << huge_time.count() << " ns" << std::endl;

    // verify that move assigning N elements takes O(1) time
    // you should be able to easily beat this benchmark if you actually implemented move
    VERIFY_TRUE(3*tiny_time.count() >= small_time.count(), __LINE__); // 3/16 Avery: fixed from > to >=
    VERIFY_TRUE(3*small_time.count() >= big_time.count(), __LINE__); // 3/16 Avery: fixed from > to >=
    VERIFY_TRUE(3*big_time.count() >= huge_time.count(), __LINE__); // 3/16 Avery: fixed from > to >=
}
#endif

#if RUN_TEST_4H
void H_move_assign_time() {
    HashMap<int, int> empty_map(1);
    HashMap<int, int> tiny_map(1);
    HashMap<int, int> small_map(1);
    HashMap<int, int> big_map(1);
    HashMap<int, int> huge_map(1);

    for (size_t i = 0; i < 10; ++i) {
        tiny_map.insert({i, i});
    }

    for (size_t i = 0; i < 100; ++i) {
        small_map.insert({i, i});
    }

    for (size_t i = 0; i < 1000; ++i) {
        big_map.insert({i, i});
    }

    for (size_t i = 0; i < 10000; ++i) {
        huge_map.insert({i, i});
    }

    // hey compiler, stop optimizing this out. you made me do this
    __attribute__((unused)) auto start_empty = clock_type::now();
    HashMap<int, int> move_assigned_empty(1);
    move_assigned_empty = std::move(empty_map);
    __attribute__((unused)) auto end_empty = clock_type::now();

    auto start_tiny = clock_type::now();
    HashMap<int, int> move_assigned_tiny(1);
    move_assigned_tiny = std::move(tiny_map);
    auto end_tiny = clock_type::now();
    ns tiny_time = std::chrono::duration_cast<ns>(end_tiny - start_tiny);

    auto start_small = clock_type::now();
    HashMap<int, int> move_assigned_small(1);
    move_assigned_small = std::move(small_map);
    auto end_small = clock_type::now();
    ns small_time = std::chrono::duration_cast<ns>(end_small - start_small);

    auto start_big = clock_type::now();
    HashMap<int, int> move_assigned_big(1);
    move_assigned_big = std::move(big_map);
    auto end_big = clock_type::now();
    ns big_time = std::chrono::duration_cast<ns>(end_big - start_big);

    auto start_huge = clock_type::now();
    HashMap<int, int> move_assigned_huge(1);
    move_assigned_huge = std::move(huge_map);
    auto end_huge = clock_type::now();
    ns huge_time = std::chrono::duration_cast<ns>(end_huge - start_huge);

    std::cout << "Move    10 elements: " << tiny_time.count() << " ns" << std::endl;
    std::cout << "Move   100 elements: " << small_time.count() << " ns" << std::endl;
    std::cout << "Move  1000 elements: " << big_time.count() << " ns" << std::endl;
    std::cout << "Move 10000 elements: " << huge_time.count() << " ns" << std::endl;

    // verify that move assigning N elements takes O(1) time
    // you should be able to easily beat this benchmark if you actually implemented move
    VERIFY_TRUE(3*tiny_time.count() >= small_time.count(), __LINE__); // 3/16 Avery: fixed from > to >=
    VERIFY_TRUE(3*small_time.count() >= big_time.count(), __LINE__); // 3/16 Avery: fixed from > to >=
    VERIFY_TRUE(3*big_time.count() >= huge_time.count(), __LINE__); // 3/16 Avery: fixed from > to >=
}
#endif

std::string print_with_commas(long long int n)
{
    std::string ans = "";

    // Convert the given integer
    // to equivalent string
    std::string num = to_string(n);

    // Initialise count
    int count = 0;

    // Traverse the string in reverse
    for (int i = num.size() - 1;
         i >= 0; i--) {
        count++;
        ans.push_back(num[i]);

        // If three characters
        // are traversed
        if (count == 3) {
            ans.push_back(',');
            count = 0;
        }
    }

    // Reverse the string to get
    // the desired output
    reverse(ans.begin(), ans.end());

    // If the given string is
    // less than 1000
    if (ans.size() % 4 == 0) {

        // Remove ','
        ans.erase(ans.begin());
    }

    return ans;
}

int A_benchmark_insert_erase() {
    cout << "Task: insert then erase N elements, measured in ns." << endl;
    auto good_hash_function = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    std::vector<size_t> my_map_timing;
    std::vector<int> sizes{10, 100, 1000, 10000, 100000, 1000000};
    for (size_t size : sizes) {
        std::vector<int> million;
        for (size_t i = 0; i < size; i++) {
            million.push_back(i);
        }
        auto rng = std::default_random_engine {};
        std::shuffle(million.begin(), million.end(), rng);
        size_t my_map_result, std_map_result;
        {
            auto my_start = clock_type::now();

            HashMap<int, int, decltype(good_hash_function)> my_map(5000000, good_hash_function);
            for (int element : million) {
                my_map.insert({element, element});
            }

            for (int element : million) {
                my_map.erase(element);
            }

            auto my_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(my_end - my_start);

            my_map_result = end.count();
        }

        {
            auto std_start = clock_type::now();

            std::unordered_map<int, int, decltype(good_hash_function)> std_map(size, good_hash_function);
            for (int element : million) {
                std_map.insert({element, element});
            }

            for (int element : million) {
                std_map.erase(element);
            }

            auto std_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(std_end - std_start);

            std_map_result = end.count();
        }

        std::cout << "size "  << std::setw(10) << size;
        std::cout << " | HashMap: " <<  std::setw(13) << print_with_commas(my_map_result);
        std::cout << " | std:unordered_map: "  << std::setw(13) << print_with_commas(std_map_result) << std::endl;
        my_map_timing.push_back(my_map_result);
    }
    VERIFY_TRUE(10*my_map_timing[0] < my_map_timing[3], __LINE__); // Ensure runtime of N = 10 is much faster than N = 10000
    return true;
}

int B_benchmark_find() {
    cout << "Task: find N elements (random hit/miss), measured in ns." << endl;
    auto good_hash_function = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    std::vector<size_t> my_map_timing;
    std::vector<int> sizes{10, 100, 1000, 10000, 100000, 1000000};
    for (size_t size : sizes) {
        std::vector<int> million;
        std::vector<int> lookup;
        for (size_t i = 0; i < 2*size; i++) {
            million.push_back(i);
            lookup.push_back(i);
        }
        auto rng = std::default_random_engine {};
        std::shuffle(million.begin(), million.end(), rng);
        std::shuffle(lookup.begin(), lookup.end(), rng);
        size_t my_map_result, std_map_result;
        {

            HashMap<int, int, decltype(good_hash_function)> my_map(5000000, good_hash_function);
            for (size_t i = 0; i < million.size(); i += 2) {
                int element = million[i];
                my_map.insert({element, element});
            }
            auto my_start = clock_type::now();
            int count = 0;
            for (size_t i = 0; i < lookup.size(); i += 2) {
                int element = lookup[i];
                auto found = my_map.find(element);
                count += (found == my_map.end());
            }

            auto my_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(my_end - my_start);

            my_map_result = end.count();
        }

        {
            std::unordered_map<int, int, decltype(good_hash_function)> std_map(size, good_hash_function);
            for (size_t i = 0; i < million.size(); i += 2) {
                int element = million[i];
                std_map.insert({element, element});
            }
            auto std_start = clock_type::now();
            int count = 0;
            for (size_t i = 0; i < lookup.size(); i += 2) {
                int element = lookup[i];
                auto found = std_map.find(element);
                count += (found == std_map.end());
            }

            auto std_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(std_end - std_start);

            std_map_result = end.count();
        }

        std::cout << "size "  << std::setw(10) << size;
        std::cout << " | HashMap: " <<  std::setw(13) << print_with_commas(my_map_result);
        std::cout << " | std:unordered_map: "  << std::setw(13) << print_with_commas(std_map_result) << std::endl;
        my_map_timing.push_back(my_map_result);
    }
    VERIFY_TRUE(10*my_map_timing[0] < my_map_timing[3], __LINE__); // Ensure runtime of N = 10 is much faster than N = 10000
    return true;
}

int C_benchmark_iterate() {
    cout << "Task: iterate over all N elements, measured in ns." << endl;
    auto good_hash_function = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    std::vector<size_t> my_map_timing;
    std::vector<size_t> std_map_timing;
    std::vector<int> sizes{10, 100, 1000, 10000, 100000, 1000000};
    for (size_t size : sizes) {
        std::vector<int> million;
        for (size_t i = 0; i < size; i++) {
            million.push_back(i);
        }
        auto rng = std::default_random_engine {};
        std::shuffle(million.begin(), million.end(), rng);
        size_t my_map_result, std_map_result;
        {
            HashMap<int, int, decltype(good_hash_function)> my_map(5000000, good_hash_function);
            for (int element : million) {
                my_map.insert({element, element});
            }

            auto my_start = clock_type::now();
            size_t count = 0;
            for (const auto& [key, value] : my_map) {
                count += key;
            }
            auto my_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(my_end - my_start);

            my_map_result = end.count();
        }

        {
            std::unordered_map<int, int, decltype(good_hash_function)> std_map(size, good_hash_function);
            for (int element : million) {
                std_map.insert({element, element});
            }

            auto std_start = clock_type::now();
            size_t count = 0;
            for (const auto& [key, value] : std_map) {
                count += key;
            }
            auto std_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(std_end - std_start);

            std_map_result = end.count();
        }

        std::cout << "size "  << std::setw(10) << size;
        std::cout << " | HashMap: " <<  std::setw(13) << print_with_commas(my_map_result);
        std::cout << " | std:unordered_map: "  << std::setw(13) << print_with_commas(std_map_result) << std::endl;
        my_map_timing.push_back(my_map_result);
    }
    VERIFY_TRUE(10*my_map_timing[0] < my_map_timing[3], __LINE__); // Ensure runtime of N = 10 is much faster than N = 10000
    return true;
}

using std::cout;
using std::endl;
int run_milestone1_tests();
int run_milestone2_tests();
int run_milestone3_tests();
int run_milestone4_tests();
int run_benchmark();

template <typename T>
int run_test(const T& test, const string& test_name) {
    try {
        test();
        cout << "Test "  << std::setw(30) << left << test_name  << right << " PASS " << endl;
        return 1;
    } catch (const VerifyTrueAssertionFailure& e)  {
        cout << "Test "  << std::setw(30) << left << test_name << right
             << " FAIL: VERIFY_TRUE assertion failure at line number " << e.line
             << " in file tests.cpp" << endl;
    } catch (const std::exception& e) {
        cout << "Test "  << std::setw(30) << left << test_name << right
             << " FAIL: exception thrown - " << e.what() << endl;
    } catch (const std::string& e) {
        cout << "Test "  << std::setw(30) << left << test_name  << right
        << " FAIL with string " << e << endl;
    } catch (const char *& e) {
        cout << "Test "  << std::setw(30) << left << test_name  << right
        << " FAIL with string " << e << endl;
    }
    return 0;
}

void skip_test(const string& test_name) {
    cout << "Test "  << std::setw(30) << left << test_name  << right << " SKIP " << endl;
}

void run_test_harness() {
    cout << endl;
    cout << "----- CS 106L SIMPLE TEST HARNESS -----" << endl;
    cout << "Written by Avery Wang (2019-2020 lecturer)" << endl << endl;
    int required_pass = 0;
    int bonus_pass = 0;
    cout << "----- Milestone 1 Tests (Starter) -----" << endl;
    required_pass += run_milestone1_tests();
    cout << endl << "----- Milestone 2 Tests (Optional) -----" << endl;
    bonus_pass += run_milestone2_tests();
    cout << endl << "----- Milestone 3 Tests (Required) -----" << endl;
    required_pass += run_milestone3_tests();
    cout << endl << "----- Milestone 4 Tests (Required) -----" << endl;
    required_pass += run_milestone4_tests();
    cout << endl << "----- Benchmark Tests (Optional) -----" << endl;
    bonus_pass += run_benchmark();
    cout << endl << "----- Test Harness Summary -----" << endl;
    cout << "Required Tests passed: " << required_pass << "/36" << endl;
    cout << "Optional Tests passed: " << bonus_pass << "/7" << endl;

    if (required_pass < 36) {
        cout << "Some required tests were failed or skipped. " << endl;
    } else if (bonus_pass == 7) {
        cout << "You passed all required and optional tests! Awesome job!" << endl;
    } else {
        cout << "You passed all required tests! Great job!" << endl;
    }

    cout << endl << "----- End of Test Harness -----" << endl;
}

int run_milestone1_tests() {
    int passed = 0;
    passed += run_test(A_basic, "A_basic");
    passed += run_test(B_insert, "B_insert");
    passed += run_test(C_clear, "C_clear");
    passed += run_test(D_at, "D_at");
    passed += run_test(E_at_const_correct, "E_at_const_correct");
    passed += run_test(F_custom_bucket_count, "F_custom_bucket_count");
    passed += run_test(G_custom_hash_function, "G_custom_hash_function");
    passed += run_test(H_erase, "H_erase");
    passed += run_test(I_rehash_basic, "I_rehash_basic");
    passed += run_test(J_iter_foreach_one_bucket, "J_iter_foreach_one_bucket");
    passed += run_test(K_iter_foreach_filled_buckets, "K_iter_foreach_filled_buckets");
    passed += run_test(L_iter_foreach_split_buckets, "L_iter_foreach_split_buckets");
    passed += run_test(M_iter_foreach_edge, "M_iter_foreach_edge");
    passed += run_test(N_iter_forward_operators, "N_iter_forward_operators");
    passed += run_test(O_iter_algorithm_copy, "O_iter_algorithm_copy");
    passed += run_test(P_iter_algorithm_permutation, "P_iter_algorithm_permutation");
    passed += run_test(Q_iter_const_iterator, "Q_iter_const_iterator");
    passed += run_test(R_iter_const_correct, "R_iter_const_correct");
    passed += run_test(S_iter_const_conversion, "S_iter_const_conversion");
    passed += run_test(T_iter_find_member, "T_iter_find_member");
    passed += run_test(U_iter_insert_member, "U_iter_insert_member");
    passed += run_test(V_iter_erase_member, "V_iter_erase_member");

    return passed;
}


int run_milestone2_tests() {
    int passed = 0;

    #if RUN_TEST_2A
    passed += run_test(A_range_ctor_basic, "A_range_ctor_basic");
    #else
    skip_test("A_range_ctor_basic");
    #endif

    #if RUN_TEST_2B
    passed += run_test(B_range_ctor_edge, "B_range_ctor_edge");
    #else
    skip_test("B_range_ctor_edge");
    #endif

    #if RUN_TEST_2C
    passed += run_test(C_initializer_list_ctor_basic, "C_initializer_list_ctor_basic");
    #else
    skip_test("C_initializer_list_ctor_basic");
    #endif

    #if RUN_TEST_2D
    passed += run_test(D_initializer_list_ctor_edge, "D_initializer_list_ctor_edge");
    #else
    skip_test("D_initializer_list_ctor_edge");
    #endif

    return passed;
}

int run_milestone3_tests() {

    int passed = 0;

    #if RUN_TEST_3A
    passed += run_test(A_index_operator, "A_index_operator");
    #else
    skip_test("A_index_operator");
    #endif

    #if RUN_TEST_3B
    passed += run_test(B_index_const_correct, "B_index_const_correct");
    #else
    skip_test("B_index_const_correct");
    #endif

    #if RUN_TEST_3C
    passed += run_test(C_stream_insert_operator, "C_stream_insert_operator");
    #else
    skip_test("C_stream_insert_operator");
    #endif

    #if RUN_TEST_3D
    passed += run_test(D_stream_insert_const_correct, "D_stream_insert_const_correct");
    #else
    skip_test("D_stream_insert_const_correct");
    #endif

    #if RUN_TEST_3E
    passed += run_test(E_equality_operator, "E_equality_operator");
    #else
    skip_test("E_equality_operator");
    #endif

    #if RUN_TEST_3F
    passed += run_test(F_equality_const_correct, "F_equality_const_correct");
    #else
    skip_test("F_equality_const_correct");
    #endif
    return passed;
}

int run_milestone4_tests() {
    int passed = 0;
    #if RUN_TEST_4A
    passed += run_test(A_copy_ctor_basic, "A_copy_ctor_basic");
    #else
    skip_test("A_copy_ctor_basic");
    #endif

    #if RUN_TEST_4B
    passed += run_test(B_copy_assign_basic, "B_copy_assign_basic");
    #else
    skip_test("B_copy_assign_basic");
    #endif

    #if RUN_TEST_4C
    passed += run_test(C_copy_edge, "C_copy_edge");
    #else
    skip_test("C_copy_edge");
    #endif

    #if RUN_TEST_4D
    passed += run_test(D_move_ctor_basic, "D_move_ctor_basic");
    #else
    skip_test("D_move_ctor_basic");
    #endif

    #if RUN_TEST_4E
    passed += run_test(E_move_assign_basic, "E_move_assign_basic");
    #else
    skip_test("E_move_assign_basic");
    #endif

    #if RUN_TEST_4F
    passed += run_test(F_move_edge, "F_move_edge");
    #else
    skip_test("F_move_edge");
    #endif

    #if RUN_TEST_4G
    passed += run_test(G_move_ctor_time, "G_move_ctor_time");
    #else
    skip_test("G_move_ctor_time");
    #endif

    #if RUN_TEST_4H
    passed += run_test(H_move_assign_time, "H_move_assign_time");
    #else
    skip_test("H_move_assign_time");
    #endif

    return passed;
}

int run_benchmark() {
    int passed = 0;
    #if RUN_BENCHMARK
    passed += run_test(A_benchmark_insert_erase, "A_benchmark_insert_erase");
    std::cout << std::endl;
    passed += run_test(B_benchmark_find, "B_benchmark_find");
    std::cout << std::endl;
    passed += run_test(C_benchmark_iterate, "C_benchmark_iterate");
    #else
    skip_test("A_benchmark_insert_erase");
    skip_test("B_benchmark_find");
    skip_test("C_benchmark_iterate");
    #endif
    return passed;
}
