#include "mystring.hpp"

#include <stdexcept>
#include <cstring>

/* START: mystring implementation */

dsaac::MyString::MyString()
{
    cap_ = 1;
    data_ = new char[cap_];
    data_[0] = '\0';
    len_ = 0;
}

dsaac::MyString::MyString(const char* str)
{
    len_ = 0;
    while (str[len_] != '\0') {
        len_++;
    }
    cap_ = len_ + 1;
    data_ = new char[cap_];
    for (std::size_t i = 0; i < len_; ++i) {
        data_[i] = str[i];
    }
    data_[len_] = '\0';
}

dsaac::MyString::~MyString()
{
    delete[] data_;
}

dsaac::MyString::MyString(const MyString& other)
{
    len_ = other.len_;
    cap_ = other.cap_;
    data_ = new char[cap_];
    for (std::size_t i = 0; i <= len_; ++i) {
        data_[i] = other.data_[i];
    }
}

dsaac::MyString& dsaac::MyString::operator=(const MyString& other)
{
    if (this != &other) {
        delete[] data_;
        len_ = other.len_;
        cap_ = other.cap_;
        data_ = new char[cap_];
        for (std::size_t i = 0; i <= len_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    return *this;
}

std::size_t dsaac::MyString::length() const
{
    return len_;
}

dsaac::MyString dsaac::MyString::substr(std::size_t pos, std::size_t n) const
{
    if (pos >= len_ && len_ > 0) {
        throw std::out_of_range("pos out of range");
    }
    std::size_t subLen = (pos + n > len_) ? (len_ - pos) : n;
    
    char* temp = new char[subLen + 1];
    for (std::size_t i = 0; i < subLen; i++) {
        temp[i] = data_[pos + i];
    }
    temp[subLen] = '\0';
    
    dsaac::MyString result(temp);
    delete[] temp;
    return result;
}

void dsaac::MyString::append(const MyString& other)
{
    // 参考上述实现，补充当容量不够时的动态扩容与拼接逻辑
}

/* END */