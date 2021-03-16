#ifndef ENTIS_ERROR_H
#define ENTIS_ERROR_H

#include <unordered_map>

namespace entis
{
    namespace error
    {
    /**
     * Possibe errors that could arise during
     * binding of a key (id_t) and some value T.
     */
    enum class BindError
    {
        /// Key used to bind some value T is equal to the MAX_ID.
        INVALID_KEY,

        /// The entity we are trying to bind is dead.
        DEAD_ENTITY
    };

    /// "Long" descriptions of the errors defined on BindError enum.
    const std::unordered_map<BindError, const char*> BIND_ERROR_DESC =
    {
        { BindError::INVALID_KEY, "key must be less than MAX_ID" },
        { BindError::DEAD_ENTITY, "entity must be alive" }
    };
    }
}

#endif