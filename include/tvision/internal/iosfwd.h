#ifndef TVISION_IOSFWD_H
#define TVISION_IOSFWD_H

#ifdef __BORLANDC__

class _EXPCLASS ostream;

#else

#include <iosfwd>

using std::ostream;

#endif // __BORLANDC__

#endif // TVISION_IOSFWD_H
