#ifndef REGISTRY_H
#define REGISTRY_H

#include <vector>
#include <memory>
#include <utility>
#include <optional>
#include <typeinfo>
#include <algorithm>
#include <unordered_map>

#include "types.h"
#include "config.h"
#include "sparse_set.h"

namespace entis
{
    class Registry
    {

    public:

        Registry()
        : current_{MAX_ID},
          entities_{},
          component_managers_{}
        {

        }

        inline id_t create_entity()
        {
            return (current_ == MAX_ID) ? make_new_entity() : recycle_entity();
        }

        inline bool is_alive(const id_t entity) const
        {
            return entity < entities_.size() && entities_[entity] == entity;
        }

        void kill_entity(const id_t entity)
        {
            mark_as_death(entity);
            delete_all_components(entity);
        }

        template <typename T>
        bool has_component(const id_t entity)
        {
            ComponentManager<T> manager = get_component_manager<T>();

            return manager ? manager->has_data(entity) : false;
        }

        template <typename T>
        Component<T> get_component(const id_t entity)
        {
            Component<T> component{};

            ComponentManager<T> manager = get_component_manager<T>();

            if(manager)
            {
                component = manager->get_data(entity);
            }

            return component;
        }

        template <typename T, typename... Args>
        BindResult bind(const id_t entity, Args&&... args)
        {
            ComponentManager<T> manager = get_component_manager<T>();

            if(!manager)
            {
               manager = make_component_manager<T>();
            }

            return manager->bind(entity, args...);
        }

        template <typename T>
        std::optional<T> unbind(const id_t entity)
        {
            ComponentManager<T> manager = get_component_manager<T>();

            std::optional<T> component{};

            if(manager)
            {
                component = manager->unbind(entity);
            }

            return component;
        }

        template<typename T>
        std::vector<id_t> entities_with_component()
        {
            ComponentManager<T> manager = get_component_manager<T>();

            std::vector<id_t> entities{};

            if(manager)
            {
                std::copy_if(entities_.begin(), entities_.end(), std::back_inserter(entities),
                    [&manager](const id_t entity)
                    {
                        return manager->has_data(entity);
                    });
            }

            return entities;
        }

    private:

        id_t current_;
        std::vector<id_t> entities_;
        std::unordered_map<std::string, std::shared_ptr<IComponentManager>> component_managers_;

        inline id_t make_new_entity()
        {
            id_t new_entity = entities_.size();
            entities_.push_back(new_entity);

            return new_entity;
        }

        inline id_t recycle_entity()
        {   
            id_t recycled_entity = current_;
            current_ = entities_[recycled_entity];
            entities_[recycled_entity] = recycled_entity;

            return recycled_entity;
        }

        inline void mark_as_death(const id_t entity)
        {
            entities_[entity] = current_;
            current_ = entity;
        }

        void delete_all_components(const id_t entity)
        {
            for(auto& entry : component_managers_)
            {
                entry.second->delete_component(entity);
            }
        }

        template <typename T>
        inline ComponentManager<T> get_component_manager()
        {
            ComponentManager<T> sparse_set{};

            std::string type_name{typeid(T).name()};

            if(component_managers_.find(type_name) != component_managers_.end())
            {
                sparse_set = std::static_pointer_cast<SparseSet<T>>(component_managers_.at(type_name));
            }

            return sparse_set;
        }

        template <typename T>
        inline ComponentManager<T> make_component_manager()
        {
            std::string type_name{typeid(T).name()};

            component_managers_[type_name] = std::make_shared<SparseSet<T>>();

            return std::static_pointer_cast<SparseSet<T>>(component_managers_.at(type_name));
        }
    };
}

#endif