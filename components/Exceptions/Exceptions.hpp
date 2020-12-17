#pragma once

#include <exception>
#include <string>

using namespace std;

class Notice: public exception {
private:
    std::string message_;
public:
    explicit Notice(const string& message);
    const char* what() const noexcept override {
        return message_.c_str();
    }
};
Notice::Notice(const std::string& message) : message_(message) {}

class Error: public exception {
private:
    std::string message_;
public:
    explicit Error(const string& message);
    const char* what() const noexcept override {
        return message_.c_str();
    }
};
Error::Error(const std::string& message) : message_(message) {}

class Warn: public exception {
private:
    std::string message_;
public:
    explicit Warn(const string& message);
    const char* what() const noexcept override {
        return message_.c_str();
    }
};
Warn::Warn(const std::string& message) : message_(message) {}
