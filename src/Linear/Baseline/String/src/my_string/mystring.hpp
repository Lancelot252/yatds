#ifndef DSAAC_MYSTRING_HPP
#define DSAAC_MYSTRING_HPP

#include <cstddef>

namespace dsaac {

class MyString {
public:
    MyString();
    explicit MyString(const char* str);
    ~MyString();

    MyString(const MyString& other);
    MyString& operator=(const MyString& other);

    std::size_t length() const;
    MyString substr(std::size_t pos, std::size_t n) const;
    void append(const MyString& other);
    
    // 你可以在此声明更多基本方法，例如查找特定子串、替换子串等

private:
    char* data_;
    std::size_t len_;
    std::size_t cap_;
};

} // namespace dsaac

using MyString = dsaac::MyString;

#endif // DSAAC_MYSTRING_HPP
