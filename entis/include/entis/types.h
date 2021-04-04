#ifndef ENTIS_TYPES_H
#define ENTIS_TYPES_H

#include <tuple>
#include <memory>
#include <utility>
#include <optional>

#include "type_list.h"
#include "sparse_set.h"
#include "error.h"

namespace entis
{
    using BindResult = std::optional<error::BindError>;

    template <typename T>
    using ComponentManager = std::shared_ptr<SparseSet<T>>;

    template <typename T>
    using Component = std::optional<std::reference_wrapper<const T>>;

    template <typename T>
    using Components = typing::cast<typing::transform<
                                        typing::transform<T, typing::add_reference_wrapper>, 
                                        typing::add_optional>, 
                                    std::tuple>;
}

#endif