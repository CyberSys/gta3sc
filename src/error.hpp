#pragma once
#include "stdinc.h"

class CompilerError : public std::exception
{
public:
    template<typename... Args>
    CompilerError(const std::string& msg, Args&&... args)
        : what_(fmt::format(msg, std::forward<Args>(args)...))
    {}

    const char* what() const override
    {
        return what_.data();
    }

private:
    std::string what_;
};

using DecompilerError = CompilerError;

using ConfigError = CompilerError;

class BadAlternator : public CompilerError
{
public:
    template<typename... Args>
    BadAlternator(const std::string& msg, Args&&... args)
        : CompilerError(msg, std::forward<Args>(args)...)
    {}
};
