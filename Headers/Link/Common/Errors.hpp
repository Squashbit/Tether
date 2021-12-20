#ifndef _LINK_ERRORS_HPP
#define _LINK_ERRORS_HPP

namespace Link
{
    enum class ErrorCode
    {
        SUCCESS = 0,
        DOUBLE_INIT = 1,
        DISPLAY_NOT_FOUND = 2,
        VISUAL_NOT_FOUND = 3,
        GL_INIT_FAILED = 4
    };
}

#endif //_LINK_ERRORS_HPP