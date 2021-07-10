#ifndef LIST_H
#define LIST_H

#include <iostream>             // for cout
#include <iomanip>              // for setw, setprecision, setfill, right
#include <sstream>              // for istringstream
#include <vector>
#include <algorithm>
#include "list_iterator.h"

// BEGIN STARTER CODE
/*
 * Template class for List<T>
 * 
 * A sequence container backed by a doubly linked list.
 * Insertion and deletion anywhere in the list is fast,
 * but does not provide random access.
 * 
 * T - type that the List will store. Must be a regular type (copyable, movable)
 */
template <typename T>
class List {
public:
    /* Important public aliases used by the STL */
    using value_type = T;
    using iterator = ListIterator<List, false>;
    using const_iterator = ListIterator<List, true>;
    
    /* Allows iterator and list_iterator to access the linked lists */
    friend ListIterator<List, true>;
    friend ListIterator<List, false>;

    /* Default constructor O(1) */
    List() : _front{nullptr}, _back{nullptr}, _size{0} {}

    /* Destructor O(n) */
    ~List() { clear(); };

    /* Returns reference to first element, behavior undefined if empty. O(1) */
    value_type& front() { return _front->_data; }
    const value_type& front() const { return _front->_data; }

    /* Returns reference to last element, behavior undefined if empty. O(1) */
    value_type& back() { return _back->_data; }
    const value_type& back() const { return _back->_data; }

    /* Returns whether list is empty. O(1) */
    bool empty() const { return _size == 0; }

    /* Returns the size of the list. O(1) */
    size_t size() const { return _size;}

    /* Clears the list. O(n) */
    void clear();

    /* Inserts element to the front of the list. O(1) */
    void push_front(const T& value);

    /* Inserts element to the back of the list. O(1) */
    void push_back(const T& value);

    /* Pops the element in the front of the list, behavior undefined if list empty. O(1) */
    void pop_front();

    /* Pops the element in the back of the list, behavior undefined if list empty. O(1) */
    void pop_back();

    /* For testing purposes, checks if elements in list are equal to elements in vector. O(n) */
    bool is_same(const std::vector<T>& elements) const;

    /* Returns an iterator to the first element. O(1) */
    iterator begin() { return {_front, this}; }
    const_iterator begin() const { return {_front, this}; }

    /* Returns an iterator to one past the last element. O(1) */
    iterator end() { return {nullptr, this}; }
    const_iterator end() const { return {nullptr, this}; }

    /* Returns the element with given index. */
    value_type& operator[](size_t index);
    const value_type& operator[](size_t index) const;

    /* 
     * List<T>::erase
     * 
     * Erases the element pointed to by the iterator.
     * 
     * Parameter: iterator to the element that is about to be erased.
     * Return: iterator to the element immediately following the erased element
     *          (or end() if the last element is erased).
     */
    iterator erase(iterator pos);

    /*
     * List<T>::remove_if
     * 
     * Removes all elements in the list on which pred returns true.
     * 
     * Parameter: predicate to evaluate elements to decide whether to erase.
     * Return: the number of elements removed.
     */
    template <typename UniPred>
    size_t remove_if(UniPred pred);

    /* 
     * Copy constructor 
     * 
     * Creates a new List<T> that is an identical but independent copy of other.
     * Runtime: O(n)
     */
    List(const List<T>& other);

    /*
     * Move constructor
     * 
     * Creates a new List<T> that steals and re-uses the nodes in other.
     * Runtime: O(1)
     */
    List(List<T>&& other);

    /*
     * Copy assignment operator
     * 
     * Replaces *this with a List<T> that is a copy of rhs.
     * Runtime: O(n)
     */
    List& operator=(const List<T>& rhs);

    /*
     * Move assignment operator
     * 
     * Replaces *this with a List<T> that steals and re-uses the nodes in rhs.
     * Runtime: O(1)
     */
    List& operator=(List<T>&& rhs);

private:
    /* Private declaration of a node of the linked list.
     * Very similar to what you saw in CS 106B, except
     * The data is an arbitrary template type, and
     * for convenience we also have a constructor.
     */
    struct node {
        T _data;
        node* _prev;
        node* _next;

        node(const T& data = T(), node* prev = nullptr, node* next = nullptr) : 
            _data{data}, _prev{prev}, _next{next} {}
    };

    /* Pointer to front of linked list (as you saw in CS 106B) */
    node* _front;

    /* Pointer to back of the linked list */
    node* _back;

    /* Number of elements, so we can implement size() in O(1) time */
    size_t _size;
};

template <typename T>
void List<T>::clear() {
    while (_front != nullptr) {
        node* trash = _front;
        _front = _front->_next;
        delete trash;
    }
    _back = nullptr;
    _size = 0;
}

template <typename T>
void List<T>::push_front(const T& value) {
    if (empty()) {
        _front = _back = new node(value, nullptr, nullptr);
    } else {
        _front = _front->_prev = new node(value, nullptr, _front);
    }
    ++_size;
}

template <typename T>
void List<T>::push_back(const T& value) {
    if (empty()) {
        _front = _back = new node(value, nullptr, nullptr);
    } else {
        _back = _back->_next = new node(value, _back, nullptr);
    }
    ++_size;
}

template <typename T>
void List<T>::pop_front() {
    if (size() == 1) {
        _front = _back = nullptr;
    } else {
        node* trash = _front;
        _front = _front->_next;
        _front->_prev = nullptr;
        delete trash;
    }
    --_size;
}

template <typename T>
void List<T>::pop_back() {
    if (size() == 1) {
        _front = _back = nullptr;
    } else {
        node* trash = _back;
        _back = _back->_prev;
        _back->_next = nullptr;
        delete trash;
    }
    --_size;
}

template <typename T>
bool List<T>::is_same(const std::vector<T>& elements) const {
    return std::equal(elements.begin(), elements.end(), begin(), end());
}

// END STARTER CODE

template <typename T>
typename List<T>::iterator List<T>::erase(iterator pos) {
    iterator pos_next = pos; 
    ++pos_next;                 // the iterator right after pos, which we will return
    node* ptr_node = pos._node; // pointer to node you are erasing
    // you can use ptr_node->_data, ptr_node->_prev, and ptr_node->_next

    /*
    * Nugget #1
    * TODO: finish the implementation of the erase function (~8 lines).
    */
   (void) ptr_node; // just here so starter code compiles

    return pos_next;
}

template <typename T>
template <typename UniPred>
size_t List<T>::remove_if(UniPred pred) {
    /*
     * Nugget #2
     * TODO: fix the bug (~1-3 lines)
     */
    size_t count = 0;
    for (auto iter = begin(); iter != end(); ++iter) {
        if (pred(*iter)) {
            iter = erase(iter);
            ++count;
        } 
    }
    return count;
}

/*
 * bool operator==(const List<T>&, const List<T>&)
 * 
 * Usage: lhs == rhs
 * Parameter: two lists lhs and rhs that we are comparing.
 * Return: true if the elements of lhs and rhs are identical, false otherwise.
 */
template <typename T>
bool operator==(const List<T>& lhs, const List<T>& rhs) {
    (void) lhs, (void) rhs; // just here so starter code compiles
    /*
     * Nugget #3
     * TODO: implement this function (~1 line)
     */
    return true; // just here so starter code compiles
}

/*
* Nugget #4
* TODO: implement the operators <, >, <=, >=, and != (~3 lines each)
*       They aren't declared yet, so you need to declare them.
* 
* Lexicographical ordering is the same as alphabetical order. You compare
* the first element. If equal, then you compare the next element, and so on.
* If A is a prefix of B, then A < B.
* 
* Examples:
*      {1, 2, 3} < {3, 2, 1}
*      {1, 2, 3} < {1, 2, 4}
*      {1, 2} < {1, 2, 3}
*      {} < {1}
*/

template <typename T>
bool operator<(const List<T>& lhs, const List<T>& rhs) {
    (void) lhs, (void) rhs; // just here so starter code compiles
    /*
     * Nugget #4
     * TODO: implement this function (~1 line)
     */
    return true; // just here so starter code compiles
}

template <typename T>
bool operator>(const List<T>& lhs, const List<T>& rhs) {
    (void) lhs, (void) rhs; // just here so starter code compiles
    /*
     * Nugget #4
     * TODO: implement this function (~1 line)
     */
    return true; // just here so starter code compiles
}

template <typename T>
bool operator<=(const List<T>& lhs, const List<T>& rhs) {
    (void) lhs, (void) rhs; // just here so starter code compiles
    /*
     * Nugget #4
     * TODO: implement this function (~1 line)
     */
    return true; // just here so starter code compiles
}

template <typename T>
bool operator>=(const List<T>& lhs, const List<T>& rhs) {
    (void) lhs, (void) rhs; // just here so starter code compiles
    /*
     * Nugget #4
     * TODO: implement this function (~1 line)
     */
    return true; // just here so starter code compiles
}

template <typename T>
bool operator!=(const List<T>& lhs, const List<T>& rhs) {
    (void) lhs, (void) rhs; // just here so starter code compiles
    /*
     * Nugget #4
     * TODO: implement this function (~1 line)
     */
    return true; // just here so starter code compiles
}

/*
 * Nugget #5
 * TODO: 
 *      - fix the const correctness issue by adding a const reference to list.
 *      - see tests.cpp, and add a test case that catches this error.
 * 
 * ostream& operator<<(ostream&, const List<T>&)
 * 
 * Usage: os << list
 * Parameter: stream we are printing to, and the list we are printing
 * Return: reference to the stream we printed to.
 * 
 * The format of operator<< is exactly what the stanford library prints.
 * 
 * Examples (do not print the quotation marks)
 *      empty list: "{}"
 *      one element: "{1}"
 *      two elements: "{1, 2}"
 *      many elements: "{1, 2, 3, 4, 5, 6}"
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, List<T>& list) {
    os << "{";
    std::ostringstream oss;
    std::copy(list.begin(), list.end(), std::ostream_iterator<T>(oss, ", "));
    os << oss.str().erase(oss.str().size()-2);
    os << "}";
    return os;
}

/*
 * Nugget #6
 * Is it good class design to implement these two operators for List<T>?
 * ANSWER: [write yes or no]
 * REASON: [one sentence justification] 
 */
template <typename T>
typename List<T>::value_type& List<T>::operator[](size_t index) {
    auto iter = begin();
    std::advance(iter, index); // repeatedly increments iter index times.
    return *iter;
}

template <typename T>
const typename List<T>::value_type& List<T>::operator[](size_t index) const {
    auto iter = begin();
    std::advance(iter, index); // repeatedly increments iter index times.
    return *iter;
}

/*
 * Nugget #7
 * TODO: fix the bug (~1 line)
 */
template <typename T>
List<T>::List(const List<T>& other) : _front{nullptr}, _back{nullptr}, _size{0} {
    std::copy(other.begin(), other.end(), begin());
}

/*
 * Nugget #8
 * TODO: fix the bug (~3 lines)
 */
template <typename T>
List<T>::List(List<T>&& other) : _front{std::move(other._front)},
                                 _back{std::move(other._back)},
                                _size{std::move(other._size)} { }

template <typename T>
List<T>& List<T>::operator=(const List<T>& rhs) {
    /* 
     * Nugget #9
     * TODO: implement the copy and move assignment operators (~5 lines)
     */
    (void) rhs; // just here so starter code compiles
    return *this;
}

template <typename T>
List<T>& List<T>::operator=(List<T>&& rhs) {
    /* 
     * Nugget #9
     * TODO: implement the copy and move assignment operators (~5 lines)
     */
    (void) rhs; // just here so starter code compiles
    return *this;
}

// See Nugget #10 in tests.cpp

#endif // LIST_H
