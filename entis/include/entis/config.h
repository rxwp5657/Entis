#ifndef ENTIS_CONFIG_H
#define ENTIS_CONFIG_H

#include <cstdint>
#include <limits>

namespace entis
{
    typedef uint32_t id_t;
    const id_t MAX_ID = std::numeric_limits<id_t>::max();
}

#endif