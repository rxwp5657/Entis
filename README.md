# Entis
A header-only Entity-component-system (ECS) library whose implementation is based on the sparse set data structure.

## Installation

### Requirements

* A C++17 compiler.

### Submodule & CMake

If you want to use it on your project you just have to add it as a submodule:

```bash
git submodule add https://github.com/rxwp5657/Entis.git lib/Entis
```

Then, on you CMakeLists file add the following line:

```make
add_subdirectory(path_to_lib/lib/entis lib/entis)
```
## Entities & Components

To begin with, the first step for working with the `entis` library is to include the `core` header which includes all the necessary definitions:

```cpp
#include <entis/core.h>
```

Next, we can instantiate a Registry which is the main entry point of the library as it enables you to:

+ Create new entities.
+ Kill entities.
+ Bind/Unbind components to entities.
+ Get the components of an entity (either a single or multiple ones).
+ Queries (get all the components of entities that satisfy a query).
+ Get all the entities that have a component of the specified type.
+ Check if an entity is alive.
+ Check if an entity has a component.

```
entis::Registry registry{};
```

Now, the primary building block of any ECS is the Entity which allows you to represent "objects" in your games (e.g. players, enemies, NPCs, props, etc.). However, entites by themself are not pretty useful as they lack any data nor functionality (on OOP, as an analogy, they would be a class without attributes or methods) but fear not, we can add them in the form of components and systems respectively.

In the case of `entis` the entities are represented through the the `id_t` type defined on the `config.h` header file which is only a type alias for a `uint32_t` (default behavior). Moreover, it is important to notice that you can configure the type of the entities by changing the type of the alias. Also, you must take into account that this type determines:

* The amount of entities that can be managed by the ECS.
* The amount of components of a single type.
* The null entity (e.g. MAX(id_t), this is, the max value that can be represented by said type).

Furthermore, in `entis` entities are recycled, this is, whenever an entity is killed (all of its components are deleted and it's marked as dead) it will be reutilized in the future when calling `make_entity`. For instance, some of the operations on entities include:

```cpp
const entis::id_t player = registry.make_entity(); // create an entity

registry.is_alive(player);

registry.kill(player); // delete all of its components and mark it as dead.
```

Consequently, in `entis` components are plain structs (classes are alled too but structs are much better) that store data and can be binded to entities but there are some considerations to take into account:

* When binding a component to an entity that already has a component of the same type an update occurs thus, you can't mutate a component of an entity only fully update it with a new instance.
* When binding a component you must pass the parameters necessary create a new instance as they will be perfectly forwarded to the constructor of the specified type. Also, you can pass an instance if and only if the constructor of the specified type defines either a copy or move constructor.

Also, appart of binding a component to an entity you can unbind components, query a single o multiple components, check if an entity has a component and get all the entities that have a specific type of component:

```cpp
registry.bind<Position>(player, 0.0f, 0.0f, 0.0f);
registry.bind<Mesh>(player, ...);

// Array of entities that have a Position component
std::vector<entis::id_t> result = registry.entities_with_component<Position>();

bool has_position = registry.has_component<Position>(player); // yields true

std::optional<std::reference_wrapper<const Position>> player_pos = registry.get_component<Position>(player);

std::tuple<std::optional<std::reference_wrapper<const Position>>,
           std::optional<std::reference_wrapper<const Mesh>>,
           std::optional<std::reference_wrapper<const IA>>> player_comps = 
           registry.get_components<Position, Mesh, IA>(player);

registry.unbind<Position>(player);
```

Something important to notice about this example are the `get_component` the `get_components` functions and the long type annotations. This functions return `optionals` becuase the specified entity may or may not contain the requested components thus we need a way to represent this behavior. Finally, the long annotations can be shortened by using `auto` but the `entis` library provides convenient type declarations defined on the `types.h` header:

```cpp
entis::Component<Position> player_pos = registry.get_component<Position>(player);

using PlayerComponents = entis::typing::type_list_t<Position, Mesh, IA>;

entis::Components<PlayerComponents> player_comps = registry.get_components<Position, Mesh, IA>(player);
```

## typing, the metaprogramming library

Becuase `entis` heavily relies on the usage of types and since it doesn't implement a custom `reflection system` nor Cpp's `Run Time Type Information (RTTI)` is enough 
for our purposes a metaprogramming library was created. In this case, `entis` metaprogramming library is defined on the `type_list.h` header and it can be accessed through the `entis::typing` name space and defines the `type_list_t` data structure with its operations:

* Append a type at the start of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int>;
using Result = entis::typing::push_front<TypeList, double>; // yields type_list_t<double,int>
```
* Append a type at the end of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int>;
using Result = entis::typing::push_back<TypeList, double>; // yields type_list_t<int, double>
```
* Remove the first type of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::pop_front<TypeList>; // yields type_list_t<double>
```
* Remove the last type of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::pop_back<TypeList>; // yields type_list_t<int>
```
* Get the first type of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::front<TypeList>; // yields int
```
* Get the last type of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::front<TypeList>; // yields int
```
* Get the nth type of a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::top<TypeList>; // yields double
```
* Reverse a `type_list_t`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::reverse<TypeList>; // yields type_list_t<double, int>
```
* Get the size of a type list.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
auto result = entis::typing::size<TypeList>(); // yields 2
```
* Check if the `type_list_t` is empty.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
auto result = entis::typing::is_empty<TypeList>::value; // yields false
```
* Convert a `type_list_t` into a `std::tuple`.
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::cast<TypeList, std::tuple>; // yields std::tuple<int, double>
```
* Apply a transformation to a `type_list_t` (e.g. convert all types into const, ptr, optional, etc).
```cpp
using TypeList = entis::typing::type_list_t<int, double>;
using Result = entis::typing::transform<TypeList, entis::typing::add_optional>; // yields type_list_t<std::optional<double>, std::optional<int>>
```
* Check if two `type_list_t` are the same.
```cpp
using TypeList_A = entis::typing::type_list_t<int, double>;
using TypeList_B = entis::typing::type_list_t<int, double>; 
bool same = entis::typing::is_equal<TypeList_A, TypeList_B>()// yields true.
```

## Queries

Some times we would like to process entities and their components that satisfy some conditions so, because of this, `entis` implements a rather simple yet "powerful" query system which allows us to process the components of entities that satisfy the following conditions:

* *WithComponents*: specify what components an entity **must have**.
* *WithoutComponents*: specify what components an entity **mustn't have**.

For example:

```cpp
using WithComp = entis::typing::type_list_t<uint32_t, Vec2>;
using WithoutComp = entis::typing::type_list_t<char>;

registry.bind<uint32_t>(e0, ...);
registry.bind<uint32_t>(e1, ...);

registry.bind<Vec2>(e0, ...);
registry.bind<Vec2>(e1, ...);

registry.bind<char>(e0, ...);

// yields a std::vector of tuples of const references to uint32_t and Vect2 of size 2
// (one tuple for e0 and one for e1).
const entis::QueryResult<WithComp> with_comp = registry.query<WithComp>();

// yields the same type as the previous result but this time the vector is of size 1
// since e0 doesn't satisfy the query (it has a char component).
const entis::QueryResult<WithComp> without_comp = registry.query<WithComp, WithoutComp>();
```

## Learning Resources

* [Metaprogramming](http://www.tmplbook.com)
* [ECS - general](https://en.wikipedia.org/wiki/Entity_component_system)
* [ECS - design ideas (Unity)](https://docs.unity3d.com/Packages/com.unity.entities@0.4/manual/index.html)
* [ECS - design ideas (ECS back and forth)](https://skypjack.github.io)