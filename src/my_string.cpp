// ============================================================================
// 作业 2：String 类的实现文件
//
// 目前本文件是空的：构建时链接阶段会报 "undefined reference to `String::...`"，
// 这是预期现象。请先到 include/my_string.h 中补好私有数据成员，再在这里实现
// 所有声明过的成员函数与运算符。
//
// 如果你想拆成多个 .cpp 文件，请同步修改根目录 CMakeLists.txt 中的
// STRING_SOURCES 列表。
//
// 实现清单（与 include/my_string.h 一一对应）：
//   [ ] String() / String(const char*) / 拷贝构造 / 移动构造 / 析构
//   [ ] 复制赋值 operator=(const String&) / 移动赋值 operator=(String&&)
//   [ ] operator+ / operator[]（含 const 版本）/ at（含 const 版本）
//   [ ] size / capacity
//   [ ] insert / push_back
//   [ ] c_str / operator const char*
//   [ ] swap
//   [ ] friend operator<< / operator>>
//
// 完成后按 docs/build-and-test.md 的步骤构建、运行测试并做 ASan/UBSan 检查。
// ============================================================================

// ============================================================================
// 作业 2：String 类的实现文件
//
// 目前本文件是空的：构建时链接阶段会报 "undefined reference to `String::...`"，
// 这是预期现象。请先到 include/my_string.h 中补好私有数据成员，再在这里实现
// 所有声明过的成员函数与运算符。
//
// 如果你想拆成多个 .cpp 文件，请同步修改根目录 CMakeLists.txt 中的
// STRING_SOURCES 列表。
//
// 实现清单（与 include/my_string.h 一一对应）：
//   [ ] String() / String(const char*) / 拷贝构造 / 移动构造 / 析构
//   [ ] 复制赋值 operator=(const String&) / 移动赋值 operator=(String&&)
//   [ ] operator+ / operator[]（含 const 版本）/ at（含 const 版本）
//   [ ] size / capacity
//   [ ] insert / push_back
//   [ ] c_str / operator const char*
//   [ ] swap
//   [ ] friend operator<< / operator>>
//
// 完成后按 docs/build-and-test.md 的步骤构建、运行测试并做 ASan/UBSan 检查。
// ============================================================================

#include "my_string.h"

#include <cstring>
#include <stdexcept>
#include <utility>
#include <istream>
#include <ostream>

//构造 / 析构

String::String()
{
    capacity_ = 16;
    size_ = 0;

    data_ = new char[capacity_ + 1];
    data_[0] = '\0';
}

String::String(const char* str)
{
    if (str == nullptr) {
        capacity_ = 16;
        size_ = 0;

        data_ = new char[capacity_ + 1];
        data_[0] = '\0';
        return;
    }

    size_ = std::strlen(str);

    capacity_ = 16;
    while (capacity_ < size_) {
        capacity_ *= 2;
    }

    data_ = new char[capacity_ + 1];
    std::strcpy(data_, str);
}

String::String(const String& other)
{
    size_ = other.size_;
    capacity_ = other.capacity_;

    data_ = new char[capacity_ + 1];
    std::strcpy(data_, other.data_);
}

String::~String()
{
    delete[] data_;
}


// 移动构造
String::String(String&& other) noexcept
{
    size_ = other.size_;
    capacity_ = other.capacity_;
    data_ = other.data_;

    other.data_ = nullptr;
}


//赋值

String& String::operator=(const String& other)
{
    // 防止自己给自己赋值
    if (this == &other) {
        return *this;
    }

    // 先申请
    char* new_data = new char[other.capacity_ + 1];

    std::strcpy(new_data, other.data_);

    // 准备好后，再删除旧空间
    delete[] data_;

    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}


// 移动赋值
String& String::operator=(String&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    delete[] data_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;

    return *this;
}


//operator+

String String::operator+(const String& other) const
{
    std::size_t new_size = size_ + other.size_;

    char* new_data = new char[new_size + 1];

    // 左边
    std::memcpy(new_data, data_, size_);

    // 右边
    std::memcpy(new_data + size_, other.data_, other.size_);

    // 字符串结束符
    new_data[new_size] = '\0';

    String result;

    delete[] result.data_;

    result.data_ = new_data;
    result.size_ = new_size;
    result.capacity_ = new_size;

    return result;
}


//operator[]

char& String::operator[](std::size_t index) noexcept
{
    return data_[index];
}

const char& String::operator[](std::size_t index) const noexcept
{
    return data_[index];
}


//at

char& String::at(std::size_t index)
{
    if (index >= size_) {
        throw std::out_of_range("index out of range");
    }

    return data_[index];
}

const char& String::at(std::size_t index) const
{
    if (index >= size_) {
        throw std::out_of_range("index out of range");
    }

    return data_[index];
}


//size / capacity

std::size_t String::size() const noexcept
{
    return size_;
}

std::size_t String::capacity() const noexcept
{
    return capacity_;
}


//insert

void String::insert(std::size_t pos, const String& str)
{
    if (pos > size_) {
        throw std::out_of_range("index out of range");
    }

    std::size_t new_size = size_ + str.size_;

    // 计算新的 capacity
    std::size_t new_capacity = capacity_;

    while (new_capacity < new_size) {
        new_capacity *= 2;
    }

    // 先申请新空间
    char* new_data = new char[new_capacity + 1];

    // 前半部分
    std::memcpy(
        new_data,
        data_,
        pos
    );

    // 插入的字符串
    std::memcpy(
        new_data + pos,
        str.data_,
        str.size_
    );

    // 后半部分
    std::memcpy(
        new_data + pos + str.size_,
        data_ + pos,
        size_ - pos
    );

    // '\0'
    new_data[new_size] = '\0';

    // 全部成功后，再替换旧数据
    delete[] data_;

    data_ = new_data;
    size_ = new_size;
    capacity_ = new_capacity;
}


//push_back

void String::push_back(char ch)
{
    if (size_ == capacity_) {

        std::size_t new_capacity = capacity_ * 2;

        char* new_data = new char[new_capacity + 1];

        std::strcpy(new_data, data_);

        delete[] data_;

        data_ = new_data;
        capacity_ = new_capacity;
    }

    data_[size_] = ch;
    size_++;

    data_[size_] = '\0';
}


//c_str

const char* String::c_str() const noexcept
{
    // 普通对象直接返回 data_
    // 移动后的对象暂时可能为 nullptr
    if (data_ == nullptr) {
        return "";
    }

    return data_;
}


//类型转换

String::operator const char*() const noexcept
{
    return c_str();
}

//swap,交换成员变量的内容

void String::swap(String& other) noexcept
{
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}


//输出(cout)

std::ostream& operator<<(std::ostream& os, const String& str)
{
    if (str.data_ != nullptr) {
        os << str.data_;
    }

    return os;
}


//输入(cin)

std::istream& operator>>(std::istream& is, String& str)
{
    char temp[256];
    is >> temp;
    if (is) {
        str = temp;
    }
    return is;
}