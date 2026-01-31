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
        // Find the last space (before return type)
        auto pos = funcSig.rfind(" ");
        std::string trimmed = (pos != std::string::npos) ? funcSig.substr(pos + 1) : funcSig;

        // Remove argument list
        auto parenPos = trimmed.find("(");
        if (parenPos != std::string::npos)
            trimmed = trimmed.substr(0, parenPos);

        return trimmed;
    }

    #define DEBUG_LOG(x) \
        std::cout << "[" << extractClassMethod(FUNC_SIG) << "] " << x << std::endl

#else
    #define DEBUG_LOG(x) do {} while(0)
#endif
