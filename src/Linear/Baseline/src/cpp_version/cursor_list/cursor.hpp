#ifndef DSAAC_CURSOR_LIST_HPP
#define DSAAC_CURSOR_LIST_HPP

#include <cstddef>
#include <vector>

namespace dsaac {

template <typename T, std::size_t SpaceSize = 100>
class CursorList {
public:
    CursorList();

    bool empty() const;
    void clear();
    void push_back(const T& value);
    bool contains(const T& value) const;
    bool erase(const T& value);
    std::vector<T> to_vector() const;

private:
    struct Node {
        T value{};
        std::size_t next = static_cast<std::size_t>(-1);
    };

    static constexpr std::size_t npos = static_cast<std::size_t>(-1);
    static constexpr std::size_t header_ = 0;

    std::vector<Node> nodes_;
    std::size_t free_ = 1;

    std::size_t allocate();
    void release(std::size_t index);
};

} // namespace dsaac

template <typename T, std::size_t SpaceSize = 100>
using CursorList = dsaac::CursorList<T, SpaceSize>;

#endif
