#ifndef _TETHER_WINDOWERROREVENT_HPP
#define _TETHER_WINDOWERROREVENT_HPP

#include <Tether/Common/Types.hpp>
#include <Tether/Common/Defs.hpp>

#include <iostream>

namespace Tether::Events
{
    class TETHER_EXPORT WindowErrorEvent
    {
    public:
        WindowErrorEvent() {}
        WindowErrorEvent(ErrorCode error, ErrorSeverity severity,
            std::string funcName);
        
        WindowErrorEvent(const WindowErrorEvent& ref);
		
        ErrorCode GetCode() const;
        ErrorSeverity GetSeverity() const;
        std::string GetFunctionName() const;
    private:
        ErrorCode error;
        ErrorSeverity severity;
        std::string funcName;
    };
}

#endif //_TETHER_WINDOWERROREVENT_HPP