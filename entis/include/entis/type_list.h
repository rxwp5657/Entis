#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include <utility>

namespace entis
{
    namespace typing
    {
        /**
         * T data structure used to store a number of types.
         * 
         * @tparam Types a packed list of types (e.g. <int, char, float>).
         */ 
        template <typename... Types>
        class type_list_t
        {
        };

        /**
         * T type trait used to get the number of elements of a 
         * type_list_t.
         * 
         * @tparam List default type implementation.
         */
        template <typename List>
        class type_list_size
        {
        public:
            static constexpr size_t value = 0;
        };

        /**
         * type_list_t type_list_size trait implementation.
         * 
         * @tparam List a packed list of types (e.g. <int, char, double>).
         */
        template <typename... List>
        class type_list_size<type_list_t<List...>>
        {
        public:
            static constexpr size_t value = sizeof...(List);
        };

        /**
         * T type trait used for knowing if a container is empty
         * 
         * @tparam List default type implementation. 
         */
        template <typename List>
        class is_empty
        {
        public:
            static constexpr bool value = false;
        };

        /**
         * type_list_t is_empty type trait implementation.
         */
        template <>
        class is_empty<type_list_t<>>
        {
        public:
            static constexpr bool value = true;
        };

        /**
         * Checks if two type_list_t are the same based on the sequence of
         * types it contains.
         * 
         * @tparam T the first type_list_t to be used on the comparison.
         * @tparam U the second type_list_t to be used on the comparison.
         * 
         * @return true if both type_list_t contain the exact same sequence of types
         * (the same types, the same order and number of types).
         */
        template <typename T, typename U>
        inline constexpr bool is_equal()
        {
            return typeid(T) == typeid(U);
        }

        /**
         * Get the number of types on a type_list_t.
         * 
         * @tparam T the type_list_t whose size we want.
         * 
         * @returns the number of types on a type_list_t.
         */
        template <typename T>
        inline constexpr size_t size()
        {
            return type_list_size<T>::value;
        }


        template <typename List>
        class front_t;

        template <>
        class front_t<type_list_t<>>
        {
        public:
            using Type = type_list_t<>;
        };

        template <typename Head, typename... Tail>
        class front_t<type_list_t<Head, Tail...>>
        {
        public:
            using Type = Head;
        };

        /**
         * Get the first type on a type_list_t.
         * 
         * @tparam List a type_list_t declaration.
         * 
         * @returns the first element on a type_list_t
         * (e.g. <int, char, double> -> int).
         */
        template <typename List>
        using front = typename front_t<List>::Type;


        template <typename List, typename NewType>
        class push_back_t;

        template <typename... List, typename NewType>
        class push_back_t<type_list_t<List...>, NewType>
        {
        public:
            using Type = type_list_t<List..., NewType>;
        };

        /**
         * Create a new type_list_t with a new type added on its tail 
         * (last position).
         * 
         * @tparam List a type_list_t declaration that will be used as a base
         * for the new type_list_t.
         * 
         * @tparam NewType the type to append to the new type_list_t.
         * 
         * @returns a new type_list_t with the elements of List and NewType
         * added on the last position (e.g. <int, char>, double -> <int, char, double>).
         */
        template <typename List, typename NewType>
        using push_back = typename push_back_t<List, NewType>::Type;


        template <typename List, typename NewType>
        class push_front_t;

        template <typename... List, typename NewType>
        class push_front_t<type_list_t<List...>, NewType>
        {
        public:
            using Type = type_list_t<NewType, List...>;
        };

        /**
         * Creates a new type_list_t with a type added at the beginning
         * 
         * @tparam List a type_list_t declaration that will be used as a base
         * for the new type_list_t.
         * 
         * @tparam NewType the type to append to the new type_list_t.
         * 
         * @returns a new type_list_t with NewType at the beginning
         * (e.g. <char, double>, int -> <int, char, double>).
         */ 
        template <typename List, typename NewType>
        using push_front = typename push_front_t<List, NewType>::Type;


        template <typename List>
        class pop_front_t;

        template <>
        class pop_front_t<type_list_t<>>
        {
        public:
            using Type = type_list_t<>;
        };

        template <typename Head, typename... Tail>
        class pop_front_t<type_list_t<Head, Tail...>>
        {
        public:
            using Type = type_list_t<Tail...>;
        };

        /**
         * Creates a new type_list_t with a type added at the beginning
         * 
         * @tparam List a type_list_t declaration that will be used as a base
         * for the new type_list_t.
         * 
         * @tparam NewType the type to append to the new type_list_t.
         * 
         * @returns a new type_list_t with NewType at the beginning
         * (e.g. <char, double>, int -> <int, char, double>).
         */
        template <typename List>
        using pop_front = typename pop_front_t<List>::Type;


        template <typename List, size_t Index>
        class at_t : public at_t<pop_front<List>, Index - 1>
        {
        };

        template <typename List>
        class at_t<List, 0> : public front_t<List>
        {
        };

        /**
         * Get the type at a certain index in a type_list_t.
         * 
         * @tparam List the list whose type we want to retrieve.
         * 
         * @tparam Index a positive integer that will be used as the index
         * for retrieving the type.
         * 
         * @returns the type at index Index (e.g. <int, char, double>, 1 -> char).
         */
        template <typename List, size_t Index>
        using at = typename at_t<List, Index>::Type;


        template <typename List>
        class top_t;

        template <typename... List>
        class top_t<type_list_t<List...>>
        {
        public:
            using Type = at<type_list_t<List...>, sizeof...(List) - 1>;
        };

        /**
         * Get the last type on a type_list_t.
         * 
         * @tparam List the type_list_t whose last element we want to retrieve.
         * 
         * @returns the last element on a type_list_t (e.g. <char, int> -> int).
         */
        template <typename List>
        using top = typename top_t<List>::Type;


        template <typename List, bool IsLast = is_empty<List>::value>
        class reverse_t;

        /**
         * Create a new type_list_t with the elements of the original type_list_t
         * reversed.
         * 
         * @tparam List the type_list_t whose elements we want to reverse.
         * 
         * @returns a new type_list_t with the elements of the original type_list_t
         * reversed (e.g. <char, int> -> <int, char>).
         */
        template <typename List>
        using reverse = typename reverse_t<List>::Type;

        template <typename List>
        class reverse_t<List, false> : public push_back_t<reverse<pop_front<List>>, front<List>>
        {

        };

        template <typename List>
        class reverse_t<List, true>
        {
        public:
            using Type = List;
        };

        /**
         * Create a new type_list_t without its last element.
         * 
         * @tparam List is a type_list_t declaration.
         * 
         * @returns a new type_list_t without its last element
         * (e.g. <int, char double> -> <int, char>)
         */
        template<typename List>
        using pop_back = reverse<pop_front<reverse<List>>>;


        template<typename T, template <typename...> typename U>
        struct cast_t;

        template<template<typename...> typename T, typename... V, template <typename...> typename U>
        struct cast_t<T<V...>, U> {
            using Type = U<V...>;
        };

        /**
         * Use the types on a type_list_t to create a new container.
         * (e.g. std::tuple)
         * 
         * @tparam T the type_list_t whose types we want.        
         * @tparam U the new container we want to create using the type_list_t.
         * 
         * @returns the specified container U using the types on type_list_t.
         * (e.g. type_list_t<int, char double> -> std::tuple<int, char double>)
         */
        template<class T, template<typename...> typename U>
        using cast = typename cast_t<T, U>::Type;


        template <typename T>
        class add_optional_t
        {
        public:
            using Type = std::optional<T>; 
        };

        /**
         * Wrapps the supplied type into a optional.
         * 
         * @tparam T the type we want to wrap.
         * 
         * 
         * @returns an optional of type T (e.g. <double> -> std::optional<double>).
         */
        template <typename T>
        using add_optional = typename add_optional_t<T>::Type;


        template <typename T>
        class add_reference_wrapper_t
        {
        public:
            using Type = std::reference_wrapper<const T>; 
        };

        /**
         * Wrapps the supplied type into a const reference wrapper.
         * 
         * @tparam T the type we want to wrap.
         * 
         * 
         * @returns an optional of type T (e.g. <double> -> std::reference_wrapper<double>).
         */
        template <typename T>
        using add_reference_wrapper = typename add_reference_wrapper_t<T>::Type;


        template <typename List, template<typename T> class MetaFun, bool Empty = is_empty<List>::value>
        class transform_t;

        template <typename List, template<typename T> class MetaFun>
        class transform_t<List, MetaFun, false> 
            : public push_front_t<typename transform_t<pop_front<List>,
                                   MetaFun>::Type,
                                  MetaFun<front<List>>>
        {
        };

        template <typename List, template<typename T> class MetaFun>
        class transform_t<List, MetaFun, true>
        {
        public:
            using Type = List;
        };

        /**
         * Applies a MetaFun to all the types on a type_list_t.
         * 
         * @tparam List a type_list_t.
         * @tparam MetaFun a meta function such as add_optional that
         * will be applied to all the types on a type_list_t.
         * 
         * @return a new type_list_t with the result of applying the
         * MetaFun to all the previous types (e.g. <double, int>, add_optional ->
         * <std::optional<double>, std::optional<int>>).
         */
        template <typename List, template<typename T> class MetaFun>
        using transform = typename transform_t<List, MetaFun>::Type;
    }
}

#endif