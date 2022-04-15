#include <Tether/Input/KeyCharInfo.hpp>

using namespace Tether::Input;

KeyCharInfo::KeyCharInfo(
    char key,
    bool autoRepeat
)
    :
    key(key),
    autoRepeat(autoRepeat)
{}

KeyCharInfo::KeyCharInfo(const KeyCharInfo& ref)
{
    key = ref.key;
    autoRepeat = ref.autoRepeat;
}

char KeyCharInfo::GetKey()
{
    return key;
}

bool KeyCharInfo::IsAutoRepeat()
{
    return autoRepeat;
}
