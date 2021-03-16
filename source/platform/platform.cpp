#include <internal/platform.h>

NullPlatform NullPlatform::instance;
PlatformStrategy* PlatformStrategy::instance = &NullPlatform::instance;
