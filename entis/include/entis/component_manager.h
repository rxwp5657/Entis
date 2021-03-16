#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "config.h"

namespace entis
{
    struct IComponentManager
    {
        virtual ~IComponentManager() = default;

        /**
         * Delete the association between an entity and its component 
         * if any.
         * 
         * @param entity the entity whose association we want to delete.
         */
        virtual void delete_component(const id_t entity) = 0;
    };
}

#endif