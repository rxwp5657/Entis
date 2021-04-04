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
    /**
     * An optional that could possibly have a BindError.
     */
    using BindResult = std::optional<error::BindError>;

    /**
     * A pointer to a SparseSet that manages the type T.
     * 
     * @tparam T the type that the SparseSet is managing.
     */
    template <typename T>
    using ComponentManager = std::shared_ptr<SparseSet<T>>;

    /**
     * An optional that could contain a reference to the specified
     * type T.
     * 
     * @tparam T the type of the component.
     */
    template <typename T>
    using Component = std::optional<std::reference_wrapper<const T>>;

    /**
     * A tuple containig optionals to references to the types specified on a
     * type_list_t.
     * 
     * @tparam List a type_list_t declaration containing the types that we want.
     */ 
    template <typename List>
    using Components = typing::cast<typing::transform<
                                        typing::transform<List, typing::add_reference_wrapper>, 
                                        typing::add_optional>, 
                                    std::tuple>;
    /**
     * A tuple containig references to the types specified on a
     * type_list_t if any.
     * 
     * @tparam List a type_list_t declaration containing the types that we want.
     */ 
    template <typename List>
    using QueryResult = std::vector<typing::cast<typing::transform<List, typing::add_reference_wrapper>, std::tuple>>;
}

#endif