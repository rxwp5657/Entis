#ifndef ENTIS_TYPES_H
#define ENTIS_TYPES_H

#include <memory>
#include <utility>
#include <optional>

#include "sparse_set.h"

namespace entis
{
    template <typename T>
    using ComponentManager = std::shared_ptr<SparseSet<T>>;

    template <typename T>
    using Component = std::optional<std::reference_wrapper<const T>>;

    using BindResult = std::optional<BindError>;
}

#endif