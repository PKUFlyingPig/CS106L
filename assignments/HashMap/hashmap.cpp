/*
* Assignment 2: HashMap template implementation (STARTER CODE)
*      TODO: write a comment here.
*/

#include "hashmap.h"

// See milestone 2 about delegating constructors (when HashMap is called in the initalizer list below)
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : HashMap{kDefaultBuckets} { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash) :
    _size{0},
    _hash_function{hash},
    _buckets_array{bucket_count, nullptr} { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
    clear();
}

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const noexcept {
    return _size;
}

template <typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const noexcept {
    return size() == 0;
}

template <typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const noexcept {
    return static_cast<float>(size())/bucket_count();
};

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const noexcept {
    return _buckets_array.size();
};

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
    auto [prev, node_found] = find_node(key);
            if (node_found == nullptr) {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    return node_found->value.second;
}

template <typename K, typename M, typename H>
const M& HashMap<K, M, H>::at(const K& key) const {
    // see static_cast/const_cast trick explained in find().
    return static_cast<const M&>(const_cast<HashMap<K, M, H>*>(this)->at(key));
}

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const noexcept {
    return find_node(key).second != nullptr;
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::clear() noexcept {
    for (auto& curr : _buckets_array) {
        while (curr != nullptr) {
            auto trash = curr;
            curr = curr->next;
            delete trash;
        }
    }
    _size = 0;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::find(const K& key) {
    return make_iterator(find_node(key).second);
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::find(const K& key) const {
    // This is called the static_cast/const_cast trick, which allows us to reuse
    // the non-const version of find to implement the const version.
    // The idea is to cast this so it's pointing to a non-const HashMap, which
    // calls the overload above (and prevent infinite recursion).
    // Also note that we are calling the conversion operator in the iterator class!
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H>*>(this)->find(key));
}

template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::iterator, bool> HashMap<K, M, H>::insert(const value_type& value) {
    const auto& [key, mapped] = value;
    auto [prev, node_to_edit] = find_node(key);
    size_t index = _hash_function(key) % bucket_count();

    if (node_to_edit != nullptr) {
        return {make_iterator(node_to_edit), false};
    }

    auto temp = new node(value, _buckets_array[index]);
    _buckets_array[index] = temp;

    ++_size;
    return {make_iterator(temp), true};
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const {
    size_t index = _hash_function(key) % bucket_count();
    node* curr = _buckets_array[index];
    node* prev = nullptr; // if first node is the key, return {nullptr, front}
    while (curr != nullptr) {
        const auto& [found_key, found_mapped] = curr->value;
        if (found_key == key) {
            return {prev, curr};
        }
        prev = curr;
        curr = curr->next;
    }
    return {nullptr, nullptr}; // key not found at all.
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::begin() noexcept {
    size_t index = first_not_empty_bucket();
    if (index == bucket_count()) {
        return end();
    }
    return make_iterator(_buckets_array[index]);
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::end() noexcept {
    return make_iterator(nullptr);
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::begin() const noexcept {
    // see static_cast/const_cast trick explained in find().
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H>*>(this)->begin());
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::end() const noexcept {
    // see static_cast/const_cast trick explained in find().
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H>*>(this)->end());
}

template <typename K, typename M, typename H>
size_t HashMap<K, M, H>::first_not_empty_bucket() const noexcept {
    auto isNotNullptr = [ ](const auto& v){
        return v != nullptr;
    };

    auto found = std::find_if(_buckets_array.begin(), _buckets_array.end(), isNotNullptr);
    return found - _buckets_array.begin();
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::make_iterator(node* curr) {
    if (curr == nullptr) {
        return {&_buckets_array, curr, bucket_count()};
    }
    size_t index = _hash_function(curr->value.first) % bucket_count();
    return {&_buckets_array, curr, index};
}

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    auto [prev, node_to_erase] = find_node(key);
    if (node_to_erase == nullptr) {
        return false;
    }
    size_t index = _hash_function(key) % bucket_count();
    (prev ? prev->next : _buckets_array[index]) = node_to_erase->next;
    --_size;
    return true;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::erase(typename HashMap<K, M, H>::const_iterator pos) {
    erase(pos++->first);
    return make_iterator(pos._node); // unfortunately we need a regular iterator, not a const_iterator
}

template <typename K, typename M, typename H>
    void HashMap<K, M, H>::debug() const {
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ')
          << "Printing debug information for your HashMap implementation\n"
          << "Size: " << size() << std::setw(15) << std::right
          << "Buckets: " << bucket_count() << std::setw(20) << std::right
          << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

    for (size_t i = 0; i < bucket_count(); ++i) {
        std::cout << "[" << std::setw(3) << i << "]:";
        node* curr = _buckets_array[i];
        while (curr != nullptr) {
            const auto& [key, mapped] = curr->value;
            // next line will not compile if << not supported for K or M
            std::cout <<  " -> " << key << ":" << mapped;
            curr = curr->next;
        }
        std::cout <<  " /" <<  '\n';
    }
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ');
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_bucket_count) {
if (new_bucket_count == 0) {
    throw std::out_of_range("HashMap<K, M, H>::rehash: new_bucket_count must be positive.");
}

std::vector<node*> new_buckets_array(new_bucket_count);
    for (auto& curr : _buckets_array) { // short answer question is asking about this 'curr'
        while (curr != nullptr) {
            const auto& [key, mapped] = curr->value;
            size_t index = _hash_function(key) % new_bucket_count;

            auto temp = curr;
            curr = temp->next;
            temp->next = new_buckets_array[index];
            new_buckets_array[index] = temp;
        }
    }
    _buckets_array = std::move(new_buckets_array);
}

/* begin student code */

// Milestone 2 (optional) - iterator-based constructors
// You will have to type in your own function headers in both the .cpp and .h files.
/*
 * Range constructor
 * Creates a HashMap with the elements in the range [first, last).
 *
 * Requirements: InputIt must be iterators to a container whose elements are pair<K, M>.
 *
 * Usage:
 *      std::vector<std::pair<char, int>> vec {{'a', 3}, {'b', 5}, {'c', 7}};
 *      HashMap<char, int> map{vec.begin(), vec.end()};
 *
 * Complexity: O(N), where N = std::distance(first, last);
 */
template <typename K, typename M, typename H>
template <typename InputIt>
HashMap<K, M, H>::HashMap(InputIt first, InputIt last, size_t bucket_count, const H& hash) : HashMap(bucket_count, hash) {
    auto iter = first;
    while (iter != last) {
        insert({iter->first, iter->second});
        ++iter;
    }
}

/*
 * Initializer list constructor
 * Creates a HashMap with the elements in the initializer list init
 *
 * Requirements: init must be an initializer_list whose elements are pair<K, M>.
 *
 * Usage:
 *      HashMap<char, int> map{{'a', 3}, {'b', 5}, {'c', 7}};
 *
 * Complexity: O(N), where N = init.size();
 *
 * Notes: you may want to do some research on initializer_lists. The most important detail you need
 * to know is that they are very limited, and have three functions: init.begin(), init.end(), and init.size().
 * There are no other ways to access the elements in an initializer_list.
 * As a result, you probably want to leverage the range constructor you wrote in the previous function!
 *
 * Also, you should check out the delegating constructor note in the .cpp file.
 */
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(std::initializer_list<value_type> init, size_t bucket_count, const H& hash) : HashMap(init.begin(), init.end(), bucket_count, hash){}


// Milestone 3 (required) - operator overloading
// The function headers are provided for you.
template <typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
    /*
     * The following code is returning a reference to memory that is on the stack.
     * This is a very common mistake, do not make the same mistake!
     * We wanted the starter code to compile without warnings, which is why
     * you see these weird compiler directives.
     * Please remove them before you start writing code.
     */

    // BEGIN STARTER CODE (remove these lines before you begin)
    (void) key;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-stack-address"

    M trash; // trash lives on the stack
    return trash; // return value is a reference to a variable that has been freed :(

#pragma GCC diagnostic pop

    // END STARTER CODE

    // complete the function implementation (1 line of code)
    // isn't it funny how the bad starter code is longer than the correct answer?
}

template <typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {

    // BEGIN STARTER CODE (remove these lines before you begin)
    (void) lhs, (void) rhs;
    return true;
    // END STARTER CODE

    // complete the function implementation (~4-5 lines of code)
}

template <typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {

    // BEGIN STARTER CODE (remove these lines before you begin)
    (void) lhs, (void) rhs;
    return true;
    // END STARTER CODE

    // complete the function implementation (1 line of code)
}

template <typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& os, const HashMap<K, M, H>& rhs) {

    // BEGIN STARTER CODE (remove these lines before you begin)
    (void) rhs;
    return os;
    // END STARTER CODE

    // complete the function implementation (~7 lines of code)
}

// Milestone 4 (required) - special member functions
// You will have to type in your own function headers in both the .cpp and .h files.

// provide the function headers and implementations (~35 lines of code)

/* end student code */
