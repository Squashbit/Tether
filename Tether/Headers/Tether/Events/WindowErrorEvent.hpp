#pragma once

#include <Tether/Common/Types.hpp>
#include <Tether/Common/Defs.hpp>

#include <iostream>

namespace Tether::Events
{
    class TETHER_EXPORT WindowErrorEvent
    {
    public:
        WindowErrorEvent() = default;
		WindowErrorEvent(ErrorCode error, ErrorSeverity severity,
            std::string funcName);
        
        ErrorCode GetCode() const;
        ErrorSeverity GetSeverity() const;
        std::string GetFunctionName() const;
    private:
        ErrorCode error;
        ErrorSeverity severity;
        std::string funcName;
    };
}
