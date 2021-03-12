#ifndef SPARSE_SET_H
#define SPARSE_SET_H

#include <vector>
#include <utility>
#include <optional>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_map>

#include "./config.h"


namespace entis
{
    /**
     * Possibe errors that could arise during
     * binding of a key (id_t) and some value T.
     */
    enum class BindError
    {
        /// Key used to bind some value T is equal to the MAX_ID.
        INVALID_KEY
    };

    /// "Long" descriptions of the errors defined on BindError enum.
    const std::unordered_map<BindError, const char*> BIND_ERROR_DESC =
    {
        { BindError::INVALID_KEY, "key must be less than MAX_ID" }
    };

    /**
     * A generic sparse set container which associates a key (id_t)
     * with some value T.
     * 
     * A sparse set is a data structure that enables O(1) insertion,
     * lookup, retrieval and deletion of the elemets it stores. Also
     * the advantage of this data structure is that it tightly packages
     * elements of type T in contiguous memory so we can take advange of
     * the CPU cache at the cost of having a sparse array for the keys.
     * 
     * @tparam T the type of the data that the container will store.
     */
    template <typename T>
    class SparseSet
    {
    public:

        /**
         * Create an empty sparse set that will store 
         * values of type T.
         * 
         * @tparam T the type of the data that the container will store.
         */
        SparseSet()
        : _sparse{},
          _dense{},
          _data{}
        { 

        }

        /**
         * Check if the key has any data assiciated to it.
         * 
         * @param key the key that we want to test.
         * 
         * @returns true if it has any value associated to it,
         * false otherwise.
         */
        inline bool has_data(const id_t key) noexcept
        {
            bool result = true;

            if(out_of_bounds(key) || is_null_key(_sparse[key]))
                result = false;

            return result;
        }

        /**
         * Get the value associated to the supplied key if any.
         * 
         * @param key the key whose value we want to retrieve.
         * 
         * @returns an optional with a reference_wrapper (const T&) to 
         * the type T when the key has data associated with it and an 
         * empty optional otherwise. 
         */
        std::optional<std::reference_wrapper<const T>> get_data(const id_t key)
        {
            std::optional<std::reference_wrapper<const T>> result{};

            if(has_data(key))
                result = std::cref(_data[_sparse[key]]);

            return result;
        }

        /**
         * Associate a key with a new instace of T if it doesn't
         * have a value associated with it already, update the 
         * value otherwise.  
         * 
         * Instead of changing ownership of a value T (std::move)
         * or making a copy of it (copy/move constructors?), we pass 
         * the necessary parameters for creating a new instance thus 
         * making the sparse set the sole owner of it.
         * 
         * @param key the key that will be associated to a new instance
         * of T.
         * 
         * @tparam ...Args a packed list of values that will be 
         * perfectly-forwarded to the constructor of T.
         * 
         * @returns an optional with a BindError when the key we want
         * to associate is the max value (MAX_ID) that can be represented 
         * with an id_t. On the other hand an empty optional when the 
         * operation was successful.
         * 
         * @throws an exception whenever std::vector can't grow.
         */
        template<typename... Args>
        std::optional<BindError> bind(const id_t key, const Args&&... args)
        {
            // avoid binding the null key (MAX_ID).
            if(is_null_key(key))
                return std::optional<BindError>{ BindError::INVALID_KEY };

            // increase the size of the sparse set to hold new key.
            if(out_of_bounds(key))
                rezise(key);

            // create new association and instance.
            if(!has_data(key))
            {
                _sparse[key] = _dense.size();
                _dense.push_back(key);
                _data.push_back({ std::forward<const Args>(args)... });
            }
            // update the current association.
            else
            {
                _data[_sparse[key]] = T{ std::forward<const Args>(args)... };
            }

            return std::optional<BindError>{};
        }

        /**
         * Delete the association between a key and its value
         * if any. 
         * 
         * When there's an association pass ownership of the
         * value that used to be associated with the key to the
         * caller.
         * 
         * @param key the key whose association we want to delete.
         * 
         * @returns an optional with the instance that used to be 
         * associated with the specified key if any, an empty optional
         * otherwise.
         */
        std::optional<T> unbind(const id_t key)
        {
            std::optional<T> result{};

            if(has_data(key))
            {
                const id_t packed_index = _sparse[key];

                _sparse[_dense.back()] = packed_index;
                _sparse[key] = MAX_ID;

                std::swap(_dense[packed_index], _dense.back());
                std::swap(_data[packed_index], _data.back());

                // pass ownership to the optional
                result = std::move(_data.back());

                _data.pop_back();
                _dense.pop_back();
            }

            return result;
        }

    private:

        std::vector<id_t> _sparse;
        std::vector<id_t> _dense;
        std::vector<T> _data;

        /**
         * Check if the sparse array has enough space
         * to hold the specified key.
         * 
         * @param the key that we want to test.
         * 
         * @returns true if there's enough space on the
         * sparse array so the key can be used to index it,
         * false otherwise.
         */
        inline bool out_of_bounds(const id_t key) noexcept
        {
            return key >= _sparse.size();
        }

        /**
         * Check if the key is not the null key
         * 
         * @param the key we want to test.
         * 
         * @returns true if the key is the MAX_ID and false
         * otherwise.
         */
        inline bool is_null_key(const id_t key) noexcept
        {
            return key == MAX_ID;
        }

        /**
         * Allocate new space on the sparse array to
         * hold the specified key.
         * 
         * @param key the key that we want the sparse set to be
         * able to store.
         * 
         * @returns how much new space was allocated (number
         * of items).
         */
        size_t rezise(const id_t key)
        {
            const size_t from = _sparse.size();
            const size_t to = key - _sparse.size();

            _sparse.resize(key + 1);

            std::fill(_sparse.begin() + from, _sparse.end() + to, MAX_ID);

            return to;
        }
    };
}

#endif