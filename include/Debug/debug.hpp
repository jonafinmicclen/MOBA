#pragma once
#include <iostream>
#include <string>

#ifdef DEBUG
    #if defined(_MSC_VER)
        #define FUNC_SIG __FUNCSIG__
    #else
        #define FUNC_SIG __PRETTY_FUNCTION__
    #endif

    inline std::string extractClassMethod(const std::string& funcSig) {
        // 1. Remove arguments: find first '('
        auto parenPos = funcSig.find('(');
        std::string noArgs = (parenPos != std::string::npos) ? funcSig.substr(0, parenPos) : funcSig;

        // 2. Trim return type: find last space before method name
        auto lastSpace = noArgs.rfind(' ');
        std::string classAndMethod = (lastSpace != std::string::npos) ? noArgs.substr(lastSpace + 1) : noArgs;

        // 3. Optional: if templates introduce weird spacing, remove any leading/trailing spaces
        size_t first = classAndMethod.find_first_not_of(" \t");
        size_t last = classAndMethod.find_last_not_of(" \t");
        if (first != std::string::npos && last != std::string::npos)
            classAndMethod = classAndMethod.substr(first, last - first + 1);

        return classAndMethod;
    }

    #define DEBUG_LOG(x) \
        std::cout << "[" << FUNC_SIG << "] " << x << std::endl

#else
    #define DEBUG_LOG(x) do {} while(0)
#endif
