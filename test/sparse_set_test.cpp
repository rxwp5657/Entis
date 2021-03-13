#include <string>
#include <limits>
#include <optional>
#include <iostream>

#include <gtest/gtest.h>

#include <entis/sparse_set.h>

TEST(SparseSetTest, CanBindData)
{
    using BIND_RESULT = std::optional<entis::BindError>;

    entis::SparseSet<std::string> set{};

    BIND_RESULT bind_res0 = set.bind(0, std::string{"first"});
    BIND_RESULT bind_res1 = set.bind(1, std::string{"second"});
    BIND_RESULT bind_err = set.bind(entis::MAX_ID, std::string{"error"});

    ASSERT_FALSE(bind_res0.has_value());
    ASSERT_FALSE(bind_res1.has_value());
    ASSERT_TRUE(bind_err.has_value());
}

TEST(SparseSetTest, CanCheckIfHasData)
{
    entis::SparseSet<std::string> set{};

    entis::id_t e0 = 0;
    entis::id_t e1 = 1;
    entis::id_t e_max = entis::MAX_ID;

    set.bind(e0, std::string{"first"});
    set.bind(e1, std::string{"second"});
    set.bind(e_max, std::string{"error"});

    ASSERT_TRUE(set.has_data(e0));
    ASSERT_TRUE(set.has_data(e1));
    ASSERT_FALSE(set.has_data(e_max));  
}

TEST(SparseSetTest, CanRetrieveData)
{
    entis::SparseSet<std::string> set{};

    entis::id_t e0 = 0;
    entis::id_t e1 = 1;
    entis::id_t e_max = entis::MAX_ID;

    set.bind(e0, std::string{"first"});
    set.bind(e1, std::string{"second"});

    ASSERT_EQ(set.get_data(e0).value().get(), std::string{"first"});
    ASSERT_EQ(set.get_data(e1).value().get(), std::string{"second"});
    ASSERT_FALSE(set.get_data(e_max).has_value());

    set.bind(e0, std::string{"new"});

    ASSERT_EQ(set.get_data(e0).value().get(), std::string{"new"});
}

TEST(SparseSetTest, CanUnbindData)
{
    entis::SparseSet<std::string> set{};

    entis::id_t e0 = 0;
    entis::id_t e1 = 1;
    entis::id_t e2 = 2;

    set.bind(e0, std::string{"first"});
    set.bind(e1, std::string{"second"});

    ASSERT_TRUE(set.has_data(e0));
    ASSERT_TRUE(set.has_data(e1));

    ASSERT_TRUE(set.unbind(e0).has_value());
    ASSERT_TRUE(set.unbind(e1).has_value());

    ASSERT_FALSE(set.unbind(e2).has_value());

    ASSERT_FALSE(set.get_data(e0).has_value());
    ASSERT_FALSE(set.get_data(e1).has_value());
}