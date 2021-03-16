#include <vector>
#include <string>
#include <iostream>
#include <optional>

#include <gtest/gtest.h>

#include <entis/registry.h>
#include <entis/sparse_set.h>

// Utily structs used for testing purposes.

struct Vec2 
{
    Vec2(const int8_t x, const int8_t y)
    : x{x},
      y{y}
    {

    }

    Vec2(const Vec2& other)
    : x{other.x},
      y{other.y}  
    {

    }

    int8_t x;
    int8_t y;
};

struct Vec3
{
    Vec3(const int8_t x, const int8_t y, const int8_t z)
    : x{x},
      y{y},
      z{z}
    {

    }

    Vec3(const Vec3& other)
    : x{other.x},
      y{other.y},
      z{other.z}
    {

    }

    int8_t x;
    int8_t y;
    int8_t z;
};

// Ease comparison.

bool operator==(const Vec2& a, const Vec2& b)
{
    return a.x == b.x && a.y == b.y;
}

bool operator==(const Vec3& a, const Vec3& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

TEST(RegistryTest, CanCreateEntity)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();
    const entis::id_t e1 = registry.create_entity();
    const entis::id_t e2 = registry.create_entity();

    ASSERT_EQ(e0, 0);
    ASSERT_EQ(e1, 1);
    ASSERT_EQ(e2, 2);
}

TEST(RegistryTest, CanBindComponents)
{
    using BIND_RESULT = std::optional<entis::BindError>;

    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();

    BIND_RESULT bind_res0 = registry.bind<uint>(e0, 5);
    BIND_RESULT bind_res1 = registry.bind<Vec2>(e0, 0, 1);

    ASSERT_FALSE(bind_res0.has_value());
    ASSERT_FALSE(bind_res1.has_value());
}

TEST(RegistryTest, CanRetrieveComponent)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();
    const entis::id_t e1 = registry.create_entity();

    const Vec2 vec2_1{0, 1};
    const Vec2 vec2_2{1, 1};
    const Vec3 vec3_1{1, 2, 3};

    registry.bind<Vec2>(e0, vec2_1);
    registry.bind<Vec2>(e1, vec2_2);
    registry.bind<Vec3>(e1, vec3_1);

    auto e0_vec2 = registry.get_component<Vec2>(e0);
    auto e1_vec2 = registry.get_component<Vec2>(e1);
    auto e1_vec3 = registry.get_component<Vec3>(e1);

    ASSERT_EQ(e0_vec2.value(), vec2_1);
    ASSERT_EQ(e1_vec2.value(), vec2_2);
    ASSERT_EQ(e1_vec3.value(), vec3_1);
}

TEST(RegistryTest, CanUpdateComponent)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();

    const Vec2 vec2_1{0, 1};
    const Vec2 vec2_2{1, 1};

    registry.bind<Vec2>(e0, vec2_1);

    ASSERT_EQ(registry.get_component<Vec2>(e0).value(), vec2_1);

    registry.bind<Vec2>(e0, vec2_2);

    ASSERT_EQ(registry.get_component<Vec2>(e0).value(), vec2_2);
}

TEST(RegistryTest, CanGetAllEntitiesWithAComponentT)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();
    const entis::id_t e1 = registry.create_entity();

    const std::vector<entis::id_t> expected_vec2{e0, e1};
    const std::vector<entis::id_t> expected_vec3{e1};
    const std::vector<entis::id_t> expected_empty{};

    registry.bind<Vec2>(e0, 0.0f, 0.0f);

    registry.bind<Vec2>(e1, 1.0f, 1.0f);
    registry.bind<Vec3>(e1, 3.0f, 3.0f, 3.0f);

    ASSERT_EQ(registry.entities_with_component<Vec2>(), expected_vec2);
    ASSERT_EQ(registry.entities_with_component<Vec3>(), expected_vec3);
    ASSERT_EQ(registry.entities_with_component<uint>(), expected_empty);
}

TEST(RegistryTest, CanRemoveComponent)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();
    const entis::id_t e1 = registry.create_entity();

    const Vec2 vec2{0, 0};
    const Vec3 vec3{3, 3, 3};

    registry.bind<Vec2>(e0, vec2);
    registry.bind<Vec3>(e0, vec3);

    const std::optional<Vec2> result1 = registry.unbind<Vec2>(e0);
    const std::optional<Vec3> result2 = registry.unbind<Vec3>(e0);

    ASSERT_EQ(result1.value(), vec2);
    ASSERT_EQ(result2.value(), vec3);

    ASSERT_FALSE(registry.unbind<Vec2>(e1).has_value());
}

TEST(RegistryTest, CanCheckIfEntityHasComponent)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();
    const entis::id_t e1 = registry.create_entity();

    registry.bind<Vec2>(e0, 0, 0);
    registry.bind<Vec3>(e1, 3, 3, 3);

    ASSERT_TRUE(registry.has_component<Vec2>(e0));
    ASSERT_TRUE(registry.has_component<Vec3>(e1));

    ASSERT_FALSE(registry.has_component<Vec3>(e0));
    ASSERT_FALSE(registry.has_component<uint>(e0));
}

TEST(RegistryTest, CanKillEntity)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();

    const Vec2 vec2{1, 2};
    const Vec3 vec3{3, 4, 5};

    registry.bind<Vec2>(e0, vec2);
    registry.bind<Vec3>(e0, vec3);

    ASSERT_EQ(registry.get_component<Vec2>(e0).value(), vec2);
    ASSERT_EQ(registry.get_component<Vec3>(e0).value(), vec3);

    registry.kill_entity(e0);

    ASSERT_FALSE(registry.get_component<Vec2>(e0).has_value());
    ASSERT_FALSE(registry.get_component<Vec3>(e0).has_value());

    ASSERT_FALSE(registry.is_alive(e0));
}

TEST(RegistryTest, CanRecycleEntity)
{
    entis::Registry registry{};

    const entis::id_t e0 = registry.create_entity();
    const entis::id_t e1 = registry.create_entity();
    const entis::id_t e2 = registry.create_entity();
    const entis::id_t e3 = registry.create_entity();

    registry.kill_entity(e2);
    registry.kill_entity(e0);
    registry.kill_entity(e1);
    registry.kill_entity(e3);

    ASSERT_EQ(registry.create_entity(), 3);
    ASSERT_EQ(registry.create_entity(), 1);
    ASSERT_EQ(registry.create_entity(), 0);
    ASSERT_EQ(registry.create_entity(), 2);
}
