#ifndef REGISTRY_H
#define REGISTRY_H

#include <tuple>
#include <vector>
#include <memory>
#include <utility>
#include <optional>
#include <typeinfo>
#include <algorithm>
#include <unordered_map>

#include "types.h"
#include "config.h"
#include "type_list.h"
#include "sparse_set.h"

namespace entis
{
    /**
     * Manages all the entities with their components by
     * providing a centralized interface for:
     * + Creating new entities.
     * + Checking if an entity is alive.
     * + Killing an entity (mark it as dead and delete all of its components).
     * + Adding a new component to an entity.
     * + Getting a specific component.
     * + Deleting a component of an entity.
     * + Checking if an entity has a component.
     * + Getting all entities that have some component.
     */ 
    class Registry
    {

    public:
        /**
         * Create a new empty Registry.
         */
        Registry()
        : current_{MAX_ID},
          entities_{},
          component_managers_{}
        {

        }

        /**
         * Create a new entity.
         * 
         * It returns an entity that is either a recycled one
         * (an entity that was killed) or brand new one (an entity
         * that hasn't been used before).
         * 
         * @returns a new entity that has no components attached to it.
         */ 
        inline id_t make_entity()
        {
            return (current_ == MAX_ID) ? make_new_entity() : recycle_entity();
        }

        /**
         * Check if an entity is alive (its currently available to be used).
         * 
         * @param entity the entity we want to test.
         * 
         * @return whether or not it is alive. 
         */
        inline bool is_alive(const id_t entity) const
        {
            return entity < entities_.size() && entities_[entity] == entity;
        }

        /**
         * Mark as dead the specified alive entity and remove all 
         * of its components.
         * 
         * @param entity the entity to kill.
         */ 
        void kill_entity(const id_t entity)
        {   
            if(is_alive(entity))
            {
                mark_as_death(entity);
                delete_all_components(entity);
            }
        }

        /**
         * Check if an entity has a component T. 
         * 
         * 
         * @tparam T the type of the component we want to check.
         * 
         * @param entity the entity we want to test.
         * 
         * @returns true if the entity has a component T, false otherwise.
         */
        template <typename T>
        bool has_component(const id_t entity) const
        {
            ComponentManager<T> manager = get_component_manager<T>();

            return manager ? manager->has_data(entity) : false;
        }

        /**
         * Get the T component of an entity if any.
         * 
         * @tparam T the type of the component we want to retrieve.
         * 
         * @param entity the entity whose component T we want to retrieve.
         * 
         * @returns an optional to a reference to the component T when it exists 
         * and an empty optional otherwise.
         */
        template <typename T>
        Component<T> get_component(const id_t entity) const
        {
            Component<T> component{};

            ComponentManager<T> manager = get_component_manager<T>();

            if(manager)
            {
                component = manager->get_data(entity);
            }

            return component;
        }

        /**
         * Associate an alive entity with a new instace of T if it 
         * doesn't have a component associated with it already, update 
         * the component otherwise.  
         * 
         * Instead of changing ownership of a component T (std::move)
         * or making a copy of it, we pass the necessary parameters 
         * for creating a new instance.
         * 
         * @tparam T the type of the component we want to bind.
         * @tparam ...Args a packed list of values that will be 
         * perfectly-forwarded to the constructor of T.
         * 
         * @param entity the entity we want to associate to the T component.
         * 
         * @returns an empty optional when the binding operation is successful but,
         * it contains a BindError::INVALID_KEY when trying to bind the null entity
         * or a BindError::DEAD_ENTITY when trying to bind a component to a dead entity.
         */ 
        template <typename T, typename... Args>
        BindResult bind(const id_t entity, Args&&... args)
        {
            if (!is_alive(entity))
            {
                return std::optional<error::BindError>{error::BindError::DEAD_ENTITY};
            }

            ComponentManager<T> manager = get_component_manager<T>();

            if(!manager)
            {
               manager = make_component_manager<T>();
            }

            return manager->bind(entity, args...);
        }

        /**
         * Delete the association between an entity and its component
         * T if any.
         * 
         * @tparam T the type of the component whose association we want to delete.
         *
         * @param entity the entity whose association we want to delete.
         * 
         * @returns an optional containing the component T whose association
         * we just deleted (if any).
         */
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

        /**
         * Get a list of entities that have a component T.
         * 
         * @tparam the type of the component we want our entities 
         * to have.
         * 
         * @return a list of entities that have a component T.
         */
        template<typename T>
        std::vector<id_t> entities_with_component() const
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

        /**
         * Get all the specified components of an entity if any.
         * 
         * @tparam Components all the components of the specified 
         * entity that we want to retrieve.
         * 
         * @param entity the entity whose components we want to retieve.
         * 
         * @returns a tuple of optionals of references to the specified types.
         * (e.g. tuple<optional<reference_wrapp<int>, optional<reference_wrapp<string>>).
         */
        template <typename... Components>
        auto get_components(const id_t entity) const
        {
            return std::make_tuple(get_component<Components>(entity) ...);
        }

        /**
         * Get the specified components of all the entities that satisfy the query params,
         * this is, the list of components that the entities must have and the ones it
         * mustn't have.
         * 
         * @tparam WithComponents a type_list_t declaration containing the types of the
         * components that the entities must have.
         * 
         * @tparam WithoutComponents a type_list_t declaration containing the types of the
         * components that the entities mustn't have.
         * 
         * @return a vector of tuples of references to the specified components of the entities
         * that satisfy the query.
         */
        template <typename WithComponents, typename WithoutComponents = typing::type_list_t<>>
        QueryResult<WithComponents> query() const
        {
            // This vectors should be already sorted as the entity array appends values
            // incrementally (e.g. 0, 1, 2, 3, etc.). Also, note that when killing 
            // an entity the entity array is not sorted anymore but, since they don't
            // have any component, the evaluation will always be false thus leaving
            // only sorted results.
            std::vector<id_t> desired_entities = with_components<WithComponents>();
            std::vector<id_t> undesirable_entities = with_components<WithoutComponents>();

            std::vector<id_t> required_entities{};

            // leave only the desired entities.
            std::set_difference(desired_entities.begin(), desired_entities.end(),
                                undesirable_entities.begin(), undesirable_entities.end(),
                                std::inserter(required_entities, required_entities.begin()));

            QueryResult<WithComponents> result{};

            std::transform(required_entities.begin(), required_entities.end(), std::back_inserter(result),
            [this](const id_t entity)
            {
                return TypeListHelper<WithComponents>::convert_and_unwrapp(*this, entity);
            });

            return result;
        }

    private:

        id_t current_; // last deleted entity (mainly used on implicit list).
        std::vector<id_t> entities_;
        std::unordered_map<std::string, std::shared_ptr<IComponentManager>> component_managers_;

        /**
         * Create a brand new entity and add it to the
         * vector of entities.
         * 
         * @returns the newly created entity.
         */
        inline id_t make_new_entity()
        {
            id_t new_entity = entities_.size();
            entities_.push_back(new_entity);

            return new_entity;
        }

        /**
         * Get the next entity of the implicit
         * list of dead entities for reuse.
         * 
         * @returns the latest killed entity. 
         */
        inline id_t recycle_entity()
        {   
            id_t recycled_entity = current_;
            current_ = entities_[recycled_entity];
            entities_[recycled_entity] = recycled_entity;

            return recycled_entity;
        }

        /**
         * Add the specified entity to the implicit list
         * of dead entities.
         * 
         * @param entity the entity we want to kill.
         */
        inline void mark_as_death(const id_t entity)
        {
            entities_[entity] = current_;
            current_ = entity;
        }

        /**
         * Deletes all the components of the specified entity.
         * 
         * @param entity the entity whose component to delete.
         */
        void delete_all_components(const id_t entity)
        {
            for(auto& entry : component_managers_)
            {
                entry.second->delete_component(entity);
            }
        }

        /**
         * Get the corresponding manager for a component T.
         * 
         * @tparam T the type whose manager we want to retrieve
         * 
         * @returns a shared_ptr to the corresponding manager (SparseSet<T>),
         * if no manager has been created the pointer is empty.
         */
        template <typename T>
        inline ComponentManager<T> get_component_manager() const
        {
            ComponentManager<T> sparse_set{};

            std::string type_name{typeid(T).name()};

            if(component_managers_.find(type_name) != component_managers_.end())
            {
                sparse_set = std::static_pointer_cast<SparseSet<T>>(component_managers_.at(type_name));
            }

            return sparse_set;
        }

        /**
         * Make a new component manager (SparseSet<T>).
         * 
         * @tparam T the type of the component that the manager 
         * will administrate.
         * 
         * @returns the newly created manager.
         */
        template <typename T>
        inline ComponentManager<T> make_component_manager()
        {
            std::string type_name{typeid(T).name()};

            component_managers_[type_name] = std::make_shared<SparseSet<T>>();

            return std::static_pointer_cast<SparseSet<T>>(component_managers_.at(type_name));
        }

        /**
         * Get all entities that have all the components on a type_list_t.
         * 
         * @tparam Components a type_list_t declaration that contain the expected components.
         * 
         * @returns a sorted vector of entities where each of them is guaranteed to have
         * all the components specified on the type_list_t.
         */
        template <typename Components>
        inline std::vector<id_t> with_components() const
        {
            std::vector<id_t> result{};

            if(!typing::is_empty<Components>::value)
            {
                std::copy_if(entities_.begin(), entities_.end(), std::back_inserter(result),
                [this](const id_t entity)
                {
                    return TypeListHelper<Components>::has_components(*this, entity);
                });
            }

            return result;
        }

        /**
         * Abstract class used to apply registry functions on a type_list_t.
         * 
         * @tparam List a type_list_t definition.
         */
        template <typename List>
        class TypeListHelper
        {
        public:
            /**
             * Create a tuple containing references to the types specified on a type_list_t. 
             * For example: 
             * 
             * type_list_t<int, string> -> 
             * tuple<reference_wrapper<int>, reference_wrapper<string>>
             * 
             * @tparam List a type_list_t definition.
             * 
             * @param registry a registry instance used to get component instances.
             * @param entity an entity whose components we want to retrieve.
             * 
             * @returns a tuple of reference wrappers to the specified types.
             */
            static auto convert_and_unwrapp(const Registry& registry, const id_t entity);

            /**
             * Check if an entity has all components specified on a type_list_t.
             * 
             * @tparam List a type_list_t definition.
             * 
             * @param registry a registry instance used to get check if an 
             * entity has some components.
             * @param entity the entity that we will test.
             * 
             * @returns true if the specified entity has all the specified components and
             * false otherwise.
             */
            static bool has_components(const Registry& registry, const id_t entity);
        };

        /**
         * Specialization used to convert a type_list_t into a parameter pack. 
         * 
         * @tparam List parameter pack containing the list of types specified on
         * a type_list_t.
         */
        template <typename... List>
        class TypeListHelper<typing::type_list_t<List...>>
        {
        public:
            static auto convert_and_unwrapp(const Registry& registry, const id_t entity)
            {
                return std::make_tuple(registry.get_component<List>(entity).value() ...);
            }

            static bool has_components(const Registry& registry, const id_t entity)
            {
                return (registry.has_component<List>(entity) && ...);
            }
        };
    };
}

#endif