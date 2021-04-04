#include <tuple>
#include <optional>
#include <iostream>

#include <gtest/gtest.h>

#include <entis/type_list.h>

TEST(TypeListTest, CanGetSizeOfTypeList)
{
    using test_A = entis::typing::type_list_t<uint32_t, float, double>;
    using test_B = entis::typing::type_list_t<uint32_t, float>;
    using test_C = entis::typing::type_list_t<>;

    ASSERT_EQ(entis::typing::size<test_A>(), 3);
    ASSERT_EQ(entis::typing::size<test_B>(), 2);
    ASSERT_EQ(entis::typing::size<test_C>(), 0);
}

TEST(TypeListTest, CanReverse)
{
    using expected = entis::typing::type_list_t<double, float, uint32_t>;
    using test = entis::typing::type_list_t<uint32_t, float, double>;

    using result = entis::typing::reverse<test>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}

TEST(TypeListTest, CanPushFront)
{
    using expected = entis::typing::type_list_t<double, uint32_t, float>;
    using test = entis::typing::type_list_t<uint32_t, float>;

    using result = entis::typing::push_front<test, double>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}

TEST(TypeListTest, CanPushBack)
{
    using expected = entis::typing::type_list_t<double, uint32_t, float>;
    using test = entis::typing::type_list_t<double, uint32_t>;

    using result = entis::typing::push_back<test, float>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}

TEST(TypeListTest, CanPopFront)
{
    using expected = entis::typing::type_list_t<float>;
    using test = entis::typing::type_list_t<uint32_t, float>;

    using result = entis::typing::pop_front<test>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}

TEST(TypeListTest, CanPopBack)
{
    using expected = entis::typing::type_list_t<uint32_t>;
    using test = entis::typing::type_list_t<uint32_t, float>;

    using result = entis::typing::pop_back<test>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}

TEST(TypeListTest, CanGetFrontType)
{
    using test = entis::typing::type_list_t<uint32_t, float>;
    using result = entis::typing::front<test>;
 
    ASSERT_TRUE((entis::typing::is_equal<result, uint32_t>()));
}

TEST(TypeListTest, CanGetTopType)
{
    using test = entis::typing::type_list_t<uint32_t, float>;
    using result = entis::typing::top<test>;
 
    ASSERT_TRUE((entis::typing::is_equal<result, float>()));
}

TEST(TypeListTest, CanGetTypeAtIndex)
{
    using test = entis::typing::type_list_t<double, uint32_t, float>;

    ASSERT_TRUE((entis::typing::is_equal<entis::typing::at<test, 0>, double>()));
    ASSERT_TRUE((entis::typing::is_equal<entis::typing::at<test, 1>, uint32_t>()));
    ASSERT_TRUE((entis::typing::is_equal<entis::typing::at<test, 2>, float>()));
}

TEST(TypeListTest, CanConvertToTuple)
{
    using test = entis::typing::type_list_t<double, uint32_t, float>;

    const std::tuple<double, uint32_t, float> expected{};

    const entis::typing::cast<test, std::tuple> result{};

    ASSERT_EQ(typeid(expected), typeid(result));
}

TEST(TypeListTest, CanAddOptional)
{
    using test = double;

    using expected = std::optional<double>;

    using result = entis::typing::add_optional<test>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}

TEST(TypeListTest, CanTransformList)
{
    using test = entis::typing::type_list_t<double, uint32_t, float>;

    using expected = entis::typing::type_list_t<std::optional<double>,
                                                std::optional<uint32_t>,
                                                std::optional<float>>;

    using result = entis::typing::transform<test, entis::typing::add_optional>;

    ASSERT_TRUE((entis::typing::is_equal<result, expected>()));
}
