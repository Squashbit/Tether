#include <Tether/Events/WindowErrorEvent.hpp>

using namespace Tether;
using namespace Tether::Events;

WindowErrorEvent::WindowErrorEvent(
    ErrorCode error, ErrorSeverity severity,
    std::string funcName
)
    :
    error(error),
    severity(severity),
    funcName(funcName)
{}

ErrorCode WindowErrorEvent::GetCode() const
{
    return error;
}

ErrorSeverity WindowErrorEvent::GetSeverity() const
{
    return severity;
}

std::string WindowErrorEvent::GetFunctionName() const
{
    return funcName;
}