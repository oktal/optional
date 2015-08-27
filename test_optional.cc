#include "gtest/gtest.h"

#include "optional.h"
#include <vector>
#include <algorithm>
#include <memory>

TEST(optional_test, construct_test)
{
    Optional<int> opt1;

    ASSERT_TRUE(opt1.isEmpty());
    ASSERT_EQ(opt1.getOrElse(0), 0);

    opt1 = Some(10);
    ASSERT_FALSE(opt1.isEmpty());
    ASSERT_EQ(opt1.getOrElse(0), 10);
    ASSERT_EQ(opt1.get(), 10);

    Optional<int> opt2 = Some(20);
    ASSERT_FALSE(opt1.isEmpty());
    ASSERT_EQ(opt2.getOrElse(0), 20);
    ASSERT_EQ(opt2.get(), 20);

    Optional<int> opt3 = None();
    ASSERT_TRUE(opt3.isEmpty());
    ASSERT_EQ(opt3.getOrElse(0), 0);

    Optional<int> opt4 = std::move(opt2);
    ASSERT_TRUE(opt2.isEmpty());
    ASSERT_FALSE(opt4.isEmpty());
    ASSERT_EQ(opt4.getOrElse(0), 20);
    ASSERT_EQ(opt2.getOrElse(0), 0);

    int value = 20;
    Optional<int> opt5 = Some(value);
    ASSERT_FALSE(opt5.isEmpty());
    ASSERT_EQ(opt5.getOrElse(0), 20);
}

TEST(optional_test, copy_test)
{
    Optional<int> opt1 = Some(20);

    Optional<int> opt2(opt1);
    ASSERT_FALSE(opt1.isEmpty());
    ASSERT_FALSE(opt2.isEmpty());
    ASSERT_EQ(opt2.getOrElse(0), 20);

    Optional<int> opt3 = Some(10);
    opt2 = opt3;
    ASSERT_FALSE(opt2.isEmpty());
    ASSERT_FALSE(opt2.isEmpty());
    ASSERT_EQ(opt2.getOrElse(0), 10);

    Optional<int> opt4 = None();
    opt1 = opt4;
    ASSERT_TRUE(opt1.isEmpty());
    ASSERT_TRUE(opt4.isEmpty());

    struct Call_Exit {
        Call_Exit(std::function<void (void)> func) : func(func) { }
        ~Call_Exit() { func(); }

        std::function<void (void)> func;
    };

    bool destroyed = false;
    bool destroyed2 = false;
    Optional<Call_Exit> opt5 = None();
    Optional<Call_Exit> opt6(opt5);

    /* Make sure that dtors are correctly called */
    Optional<Call_Exit> opt7 = Some(Call_Exit([&] { destroyed = true; }));
    Optional<Call_Exit> opt8 = Some(Call_Exit([] { }));

    opt7 = opt8;
    ASSERT_TRUE(destroyed);

    Optional<Call_Exit> opt9 = Some(Call_Exit([&] { destroyed2 = true; }));
    opt9 = None();
    ASSERT_TRUE(opt9.isEmpty());
    ASSERT_TRUE(destroyed2);

    Optional<std::string> strOpt1 = Some(std::string("Hello"));
    Optional<std::string> strOpt2(strOpt1);

    ASSERT_EQ(strOpt2.getOrElse(""), "Hello");

    strOpt2 = None();
    ASSERT_EQ(strOpt2.getOrElse(""), "");
}

TEST(optional_test, basic_test)
{
    Optional<int> opt1 = Some(10);
    int value = 0;
    optionally_do(opt1, [&](int val) {
        value = val;
    });

    ASSERT_EQ(value, 10);

    auto even = optionally_filter(opt1, [](int val) {
        return val % 2 == 0;
    });

    ASSERT_FALSE(even.isEmpty());
    ASSERT_EQ(even.get(), opt1.get());
    ASSERT_EQ(even.getOrElse(0), opt1.get());

    auto power_2 = optionally_filter(opt1, [](int val) {
        return !(val & (val - 1));
    });

    ASSERT_TRUE(power_2.isEmpty());
    ASSERT_EQ(power_2.getOrElse(-1), -1);

    Optional<std::string> opt2 = Some(std::string("Hello"));
    auto hello_world = optionally_map(opt2, [](std::string s) {
        return s + " World";
    });

    ASSERT_FALSE(hello_world.isEmpty());
    ASSERT_EQ(hello_world.getOrElse(""), "Hello World");

    std::function<int(int)> func = [](int val) {
        return val * 2;
    };

    auto getOptional = []() -> Optional<int> { return Some(5); };
    auto opt3 = getOptional();
    auto mul_by_2 = optionally_map(opt3, func);
    ASSERT_FALSE(mul_by_2.isEmpty());
    ASSERT_EQ(mul_by_2.getOrElse(0), 10);

    Optional<int> opt4 = None();
    ASSERT_TRUE(optionally_map(opt4, func).isEmpty());
    ASSERT_EQ(optionally_map(opt4, func).getOrElse(-1), -1);
}

TEST(optional_test, non_pod)
{
    typedef std::vector<int> Vec;
    static_assert(!std::is_pod<Vec>::value, "POD Type");

    Optional<Vec> opt1 = None();
    ASSERT_TRUE(opt1.isEmpty());
    ASSERT_EQ(opt1.getOrElse(Vec { }), Vec { });

    Vec v1 { 1, 2, 3, 4, 5 };
    Optional<Vec> opt2 = Some(v1);
    ASSERT_FALSE(opt2.isEmpty());
    ASSERT_EQ(opt2.getOrElse(Vec { }), v1);

    auto opt3 = optionally_map(opt2, [](Vec vec) {
        vec.erase(std::remove_if(std::begin(vec), std::end(vec),
                                [](int val) { return val % 2 == 0; }), std::end(vec));
        return vec;
    });

    ASSERT_FALSE(opt3.isEmpty());
    Vec expected { 1, 3, 5 };
    ASSERT_EQ(opt3.getOrElse(Vec { }), expected);

}

TEST(optional_test, move_only)
{
    struct Moveable {
        Moveable(int val) : val(val) { }
        Moveable(const Moveable &other) = delete;
        Moveable(Moveable &&other) = default;

        int val;
    };

    Optional<Moveable> opt1 = None();
    ASSERT_TRUE(opt1.isEmpty());

    Moveable m1(10);

    Optional<Moveable> opt2 = Some(std::move(m1));
    ASSERT_FALSE(opt2.isEmpty());

    optionally_do(opt2, [](const Moveable& m) {
        ASSERT_EQ(m.val, 10);
    });

    /* TODO: does not compile yet, figure out a way to move the object
     * whan calling the lambda
     */
#if 0
    optionally_do(opt2, [](Moveable &&m) { });
#endif

}

TEST(optional_test, fmap) {
    auto func = [](int val) -> Optional<int> {
        if (val > 10) {
            return Some(val * 2);
        }

        return None();
    };

    Optional<int> opt1 = None();
    auto opt2 = optionally_fmap(opt1, func);
    ASSERT_TRUE(opt2.isEmpty()); 

    Optional<int> opt3 = Some(10);
    auto opt4 = optionally_fmap(opt3, func);
    ASSERT_TRUE(opt4.isEmpty());

    opt3 = Some(20);
    auto opt5 = optionally_fmap(opt3, func);
    ASSERT_FALSE(opt5.isEmpty());
    ASSERT_EQ(opt5.getOrElse(0), 40);
}

TEST(optional_test, struct_member) {
    struct InnerStruct {
        Optional<int> value;
        std::string str;
    };

    InnerStruct s;
    s.value =  Some(10);
    auto dyns = std::make_shared<InnerStruct>();

    struct OuterStruct {
        Optional<InnerStruct> inner;
    };

    OuterStruct outer;
    std::unique_ptr<OuterStruct> dyno(new OuterStruct);
}
