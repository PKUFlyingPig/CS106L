#ifndef LISTITERATOR_H
#define LISTITERATOR_H

#include <iterator>     // for std::forward_iterator_tag
#include <functional>   // for std::conditional_t

// forward declaration for the HashList class
template <typename T> class List;

template <typename List, bool IsConst = true>
class ListIterator {

public:
    using value_type        =   std::conditional_t<IsConst, const typename List::value_type, typename List::value_type>;
    using iterator_category =   std::bidirectional_iterator_tag;
    using difference_type   =   std::ptrdiff_t;
    using pointer           =   value_type*;
    using reference         =   value_type&;

    using const_iterator    = ListIterator<List, true>;
    using iterator          = ListIterator<List, false>;

    friend List;
    friend iterator;
    friend const_iterator;

    operator const_iterator() const {
        return const_iterator(_node);
    }

    reference operator*() const { return _node->_data; }
    pointer operator->() const { return &(_node->_data); }

    ListIterator<List, IsConst>& operator++();
    ListIterator<List, IsConst> operator++(int);
    ListIterator<List, IsConst>& operator--();
    ListIterator<List, IsConst> operator--(int);

    template <typename List_, bool IsConst_>
    friend bool operator==(const ListIterator<List_, IsConst_>& lhs, 
                            const ListIterator<List_, IsConst_>& rhs) { return lhs._node == rhs._node; }

    template <typename List_, bool IsConst_>
    friend bool operator!=(const ListIterator<List_, IsConst_>& lhs, 
                            const ListIterator<List_, IsConst_>& rhs) { return !(lhs == rhs); }

    ListIterator(const ListIterator<List, IsConst>& rhs) = default;
    ListIterator<List, IsConst>& operator=(const ListIterator<List, IsConst>& rhs) = default;

    ListIterator(ListIterator<List, IsConst>&& rhs) = default;
    ListIterator<List, IsConst>& operator=(ListIterator<List, IsConst>&& rhs) = default;


private:
    using node = typename List::node;
    node* _node;
    const List* _list;

    ListIterator(node* node, const List* list) : _node{node}, _list{list} {}
};

template <typename List, bool IsConst>
ListIterator<List, IsConst>& ListIterator<List, IsConst>::operator++() {
    _node = _node->_next; // _node can't be nullptr - that would be incrementing end()
    return *this;
}

template <typename List, bool IsConst>
ListIterator<List, IsConst> ListIterator<List, IsConst>::operator++(int) {
    auto copy = *this; // calls the copy constructor to create copy
    ++(*this);
    return copy;
}

template <typename List, bool IsConst>
ListIterator<List, IsConst>& ListIterator<List, IsConst>::operator--() {
    if (_node == nullptr) {
        _node = _list->_back;
    } else {
        _node = _node->_prev; 
    }
    return *this;
}

template <typename List, bool IsConst>
ListIterator<List, IsConst> ListIterator<List, IsConst>::operator--(int) {
    auto copy = *this; // calls the copy constructor to create copy
    --(*this);
    return copy;
}

#endif // ListIterator_H
