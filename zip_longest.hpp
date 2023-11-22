#pragma once

#include <tuple>
#include <utility>
#include <iterator>
#include <algorithm>

template <typename... Ts>
class zip_iterator {
    using indices = std::index_sequence_for<Ts...>;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using iterator_type     = std::tuple<Ts...>;
    using value_type        = std::tuple<typename std::iterator_traits<Ts>::value_type...>;
    using pointer           = value_type*;
    using reference         = value_type&;

    constexpr zip_iterator(std::tuple<Ts...>&& begin_iterator_tuple, std::tuple<Ts...>&& end_iterator_tuple)
        : iterator_tuple_{ std::forward<std::tuple<Ts...>>(begin_iterator_tuple) },
          end_iterator_tuple_{ std::forward<std::tuple<Ts...>>(end_iterator_tuple) },
          value_tuple_{ make(indices()) }
    {}

    constexpr zip_iterator(std::tuple<Ts...>&& end_iterator_tuple)
        : iterator_tuple_{ std::forward<std::tuple<Ts...>>(end_iterator_tuple) }
    {}

    [[nodiscard]] constexpr auto operator*() noexcept {
        return value_tuple_;
    }

    [[nodiscard]] constexpr auto operator->() noexcept {
        return &value_tuple_;
    }

    constexpr zip_iterator& operator++() noexcept {
        next(indices());
        value_tuple_ = make(indices());
        return *this;
    }

    constexpr zip_iterator operator++(int) noexcept {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    [[nodiscard]] constexpr bool operator!=(zip_iterator const& rhs) const noexcept {
        return iterator_tuple_ != rhs.iterator_tuple_;
    }

    [[nodiscard]] constexpr bool operator==(zip_iterator const& rhs) const noexcept {
        return iterator_tuple_ == rhs.iterator_tuple_;
    }

private:
    template <std::size_t... Is>
    [[nodiscard]] constexpr auto make(std::index_sequence<Is...>) noexcept {
        return std::make_tuple(
            (
                std::get<Is>(iterator_tuple_) == std::get<Is>(end_iterator_tuple_) ?
                typename std::iterator_traits<decltype(std::prev(std::get<Is>(iterator_tuple_)))>::value_type{} :
                *std::get<Is>(iterator_tuple_)
            )...
        );
    }

    template <std::size_t... Is>
    constexpr void next(std::index_sequence<Is...>) noexcept {
        ((
            std::get<Is>(iterator_tuple_) == std::get<Is>(end_iterator_tuple_) ?
            std::get<Is>(iterator_tuple_) :
            ++std::get<Is>(iterator_tuple_)
        ),...);
    }

private:
    iterator_type iterator_tuple_;
    iterator_type end_iterator_tuple_;
    value_type value_tuple_;
};

template <typename... Ts>
zip_iterator<Ts...> make_zip_iterator(std::tuple<Ts...>&& end_tuple) {
    return zip_iterator<Ts...>{ std::forward<std::tuple<Ts...>>(end_tuple) };
}

template <typename... Ts>
zip_iterator<Ts...> make_zip_iterator(std::tuple<Ts...>&& begin_tuple, std::tuple<Ts...>&& end_tuple) {
    return zip_iterator<Ts...>{
        std::forward<std::tuple<Ts...>>(begin_tuple),
        std::forward<std::tuple<Ts...>>(end_tuple)
    };
}

template <typename... Range>
class zip_longest_range {
    using indices = std::index_sequence_for<Range...>;

public:
    explicit constexpr zip_longest_range(Range&&... ranges)
        : ranges_{ std::forward<Range>(ranges)... },
          size_{ max_size(indices()) }
    {}

    [[nodiscard]] constexpr auto begin() const noexcept {
        return make_zip_iterator(begin_tuple(indices()), end_tuple(indices()));
    }

    [[nodiscard]] constexpr auto end() const noexcept {
        return make_zip_iterator(end_tuple(indices()));
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return size_;
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return 0 == size();
    }

private:
    template <std::size_t... Is>
    [[nodiscard]] auto begin_tuple(std::index_sequence<Is...>) const noexcept {
        return std::make_tuple(std::begin(std::get<Is>(ranges_))...);
    }

    template <std::size_t... Is>
    [[nodiscard]] auto end_tuple(std::index_sequence<Is...>) const noexcept {
        return std::make_tuple(std::end(std::get<Is>(ranges_))...);
    }

    template <std::size_t... Is>
    [[nodiscard]] constexpr std::size_t max_size(std::index_sequence<Is...>) const noexcept {
        if constexpr (0 == sizeof...(Range)) {
            return 0;
        } else {
            return std::max({ size(std::get<Is>(ranges_))... });
        }
    }

    template <typename T>
    [[nodiscard]] constexpr std::size_t size(T&& range) const noexcept {
        return std::distance(std::begin(range), std::end(range));
    }

private:
    std::tuple<Range...> ranges_;
    std::size_t size_;
};

template <typename... Range>
[[nodiscard]] constexpr zip_longest_range<Range...> zip_longest(Range&&... ranges) {
    return zip_longest_range<Range...>(
        std::forward<Range>(ranges)...
    );
}