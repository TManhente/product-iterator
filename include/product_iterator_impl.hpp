#ifndef __PRODUCT_ITERATOR_IMPL_HPP__
#define __PRODUCT_ITERATOR_IMPL_HPP__

#include "product_iterator.hpp"

template <class... Containers>
product_iterator<Containers...>::product_iterator():
  current_tuple_(nullptr) { }

template <class... Containers>
product_iterator<Containers...>::product_iterator(
    product_iterator const& other):
  current_tuple_(nullptr) {
    begin_ = other.begin_;
    end_ = other.end_;
    current_ = other.current_;
}

template <class... Containers>
product_iterator<Containers...>::product_iterator(
    Containers const&... containers):
  current_tuple_(nullptr) {
    copy_iterator<0>(containers...);
}

template <class... Containers>
product_iterator<Containers...>::~product_iterator() {
  if (current_tuple_ != nullptr)
    delete current_tuple_;
}

template <class... Containers>
product_iterator<Containers...> const&
product_iterator<Containers...>::operator=(product_iterator const& other) {
  if (current_tuple_ != nullptr) {
    delete current_tuple_;
    current_tuple_ = nullptr;
  }
  begin_ = other.begin_;
  end_ = other.end_;
  current_ = other.current_;

  return *this;
}

template <class... Containers>
product_iterator<Containers...>
product_iterator<Containers...>::get_end() const {
  product_iterator<Containers...> ret(*this);
  ret.current_ = begin_;
  std::get<0>(ret.current_) = std::get<0>(ret.end_);
  return ret;
}

template <class... Containers>
template <size_t I>
typename
std::tuple_element<I,typename product_iterator<Containers...>::value_type>::type
const &
product_iterator<Containers...>::get() const {
  // As only a single value is needed, gathers it from its iterator.
  return *std::get<I>(current_);
}

template <class... Containers>
template <size_t I, class T1, class... Types>
void product_iterator<Containers...>::copy_iterator(T1 const& container,
    Types const&... containers) {
  std::get<I>(current_) = container.cbegin();
  std::get<I>(begin_) = container.cbegin();
  std::get<I>(end_) = container.cend();
  copy_iterator<I+1>(containers...);
}

template <class... Containers>
template <size_t I, class T1>
void product_iterator<Containers...>::copy_iterator(T1 const& container) {
  std::get<I>(current_) = container.cbegin();
  std::get<I>(begin_) = container.cbegin();
  std::get<I>(end_) = container.cend();
}

template <class... Containers>
void product_iterator<Containers...>::increment() {
  // Avoids incrementing if we have already reached the end.
  if (std::get<0>(current_) == std::get<0>(end_))
    return;

  advance<sizeof...(Containers)>();

  if (current_tuple_ != nullptr) {
    delete current_tuple_;
    current_tuple_ = nullptr;
  }
}

template <class... Containers>
bool product_iterator<Containers...>::equal(
    product_iterator<Containers...> const& other) const {
  return current_ == other.current_;
}

template <class... Containers>
typename product_iterator<Containers...>::value_type const&
product_iterator<Containers...>::dereference() const {
  if (current_tuple_ == nullptr)
    current_tuple_ = new value_type(make_value_type(
          typename tuple_sequence_generator<value_type>::type()));
  return *current_tuple_;
}

template <class... Containers>
template <size_t... S>
typename product_iterator<Containers...>::value_type
product_iterator<Containers...>::make_value_type(sequence<S...>) const {
  return value_type(*std::get<S>(this->current_)...);
}

template <class... Containers>
template <size_t I>
void product_iterator<Containers...>::advance(
    typename std::enable_if<(I == 0), int>::type) {
}

template <class... Containers>
template <size_t I>
void product_iterator<Containers...>::advance(
    typename std::enable_if<(I > 0), int>::type) {
  ++std::get<I-1>(current_);

  if (std::get<I-1>(current_) == std::get<I-1>(end_)) {
    if (I == 1)
      return;

    std::get<I-1>(current_) = std::get<I-1>(begin_);
    advance<I-1>();
  }
}

#endif
