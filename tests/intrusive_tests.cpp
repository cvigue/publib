
#include "gtest/gtest.h"


#include "intrusive.h"

using namespace ClvLib;

struct test_struct
{
    test_struct() = default;
    test_struct(int a, int b)
        : i1(a), i2(b)
    {
    }
    int i1 = 42;
    int i2 = 43;
};

using SharedInherit = SharedThis<test_struct>;

/* This function takes a SharedInherit::ptr object 'p' and an integer 'i'.
 * It first checks that the use_count() of 'p' is 2, indicating that there are two shared pointers
 * pointing to the same object. It then sets the 'i1' and 'i2' members of the object pointed to by 'p'
 * to the value of 'i'.
 */
void test_fn(SharedInherit::ptr p, int i)
{
    EXPECT_EQ(p.use_count(), 2);
    p->i1 = i;
    p->i2 = i;
}

/* This function takes a 'test_struct' object 'b' by value.
 * It checks that the 'i1' member of 'b' is equal to 66, and that the 'i2' member of 'b' is equal to 99.
 */
void test_fn_slice(test_struct b)
{
    EXPECT_EQ(b.i1, 66);
    EXPECT_EQ(b.i2, 99);
}

/* This function takes a reference to a 'test_struct' object 'b' and an integer 'i'.
 * It first checks that the 'i1' member of 'b' is equal to 66, and that the 'i2' member of 'b' is equal to 99.
 * It then sets the 'i1' and 'i2' members of 'b' to the value of 'i'.
 */
void test_fn_slice_ref(test_struct &b, int i)
{
    EXPECT_EQ(b.i1, 66);
    EXPECT_EQ(b.i2, 99);
    b.i1 = i;
    b.i2 = i;
}

/* TEST(intrusive, copy_in)
 * Tests creating an intrusive shared_ptr from a copy of a test_struct object.
 * Creates a test_struct, verifies its values, creates an intrusive shared_ptr from a copy of it,
 * and verifies that the shared_ptr's values match the original object.
 */
TEST(intrusive, copy_in)
{
    auto ts = test_struct(42, 43);
    EXPECT_EQ(ts.i1, 42);
    EXPECT_EQ(ts.i2, 43);
    auto s = make_intrusive<test_struct>(ts);
    EXPECT_EQ(s->i1, 42);
    EXPECT_EQ(s->i2, 43);
}

/* TEST(intrusive, move_in)
 * Tests creating an intrusive shared_ptr from a moved test_struct object.
 * Creates a test_struct, verifies its values, creates an intrusive shared_ptr by moving the
 * test_struct into it, and verifies that the shared_ptr's values match the original object.
 */
TEST(intrusive, move_in)
{
    auto ts = test_struct(42, 43);
    EXPECT_EQ(ts.i1, 42);
    EXPECT_EQ(ts.i2, 43);
    auto s = make_intrusive<test_struct>(std::move(ts));
    EXPECT_EQ(s->i1, 42);
    EXPECT_EQ(s->i2, 43);
}

/* TEST(intrusive, raw_create)
 * Tests creating an intrusive shared_ptr using the Create() method of the SharedInherit class.
 * Creates an intrusive shared_ptr using SharedInherit::Create(), and verifies that its values
 * match the default values of the test_struct.
 */
TEST(intrusive, raw_create)
{
    auto s = SharedInherit::Create();
    EXPECT_EQ(s->i1, 42);
    EXPECT_EQ(s->i2, 43);
}

/* TEST(intrusive, make_intrusive)
 * Tests creating an intrusive shared_ptr using the make_intrusive helper function with no
 * arguments. Creates an intrusive shared_ptr using make_intrusive<test_struct>(), and verifies
 * that its values match the default values of the test_struct.
 */
TEST(intrusive, make_intrusive)
{
    auto s = make_intrusive<test_struct>();
    EXPECT_EQ(s->i1, 42);
    EXPECT_EQ(s->i2, 43);
}

/* TEST(intrusive, make_intrusive_args)
 * Tests creating an intrusive shared_ptr using the make_intrusive helper function with arguments.
 * Creates an intrusive shared_ptr using make_intrusive<test_struct>(66, 99), and verifies that its
 * values match the arguments passed to the constructor.
 */
TEST(intrusive, make_intrusive_args)
{
    auto s = make_intrusive<test_struct>(66, 99);
    EXPECT_EQ(s->i1, 66);
    EXPECT_EQ(s->i2, 99);
}

/* TEST(intrusive, make_intrusive_get)
 * Tests the GetShared() method of the intrusive shared_ptr.
 * Creates an intrusive shared_ptr, verifies its use_count() is 1, creates another shared_ptr
 * from the first using GetShared(), and verifies that the use_count() of the first shared_ptr
 * is now 2.
 */
TEST(intrusive, make_intrusive_get)
{
    auto s = make_intrusive<test_struct>();
    EXPECT_EQ(s.use_count(), 1);
    auto s2 = s->GetShared();
    EXPECT_EQ(s.use_count(), 2);
}

/* TEST(intrusive, make_intrusive_fn)
 * Tests passing an intrusive shared_ptr to a function that modifies its values.
 * Creates an intrusive shared_ptr, verifies its use_count() is 1, passes it to a function
 * that modifies its values, verifies that the use_count() is still 1 after the function
 * call, and verifies that the values were modified correctly.
 */
TEST(intrusive, make_intrusive_fn)
{
    auto s = make_intrusive<test_struct>();
    EXPECT_EQ(s.use_count(), 1);
    test_fn(s, 1939);
    EXPECT_EQ(s.use_count(), 1);
    EXPECT_EQ(s->i1, 1939);
    EXPECT_EQ(s->i2, 1939);
}
/* TEST(intrusive, make_intrusive_slice)
 * Tests passing an intrusive shared_ptr to a function that takes it by value.
 * Creates an intrusive shared_ptr, passes it to a function that takes test_struct by value,
 * and verifies that the use_count() of the shared_ptr is still 1 after the function call.
 */
TEST(intrusive, make_intrusive_slice)
{
    auto s = make_intrusive<test_struct>(66, 99);
    test_fn_slice(*s);
    EXPECT_EQ(s.use_count(), 1);
}

/* TEST(intrusive, make_intrusive_slice_ref)
 * Tests passing an intrusive shared_ptr to a function that takes it by reference.
 * Creates an intrusive shared_ptr, verifies its use_count() is 1, passes it to a function
 * that takes test_struct by reference and modifies its values, verifies that the use_count()
 * is still 1 after the function call, and verifies that the values were modified correctly.
 */
TEST(intrusive, make_intrusive_slice_ref)
{
    auto s = make_intrusive<test_struct>(66, 99);
    EXPECT_EQ(s.use_count(), 1);
    test_fn_slice_ref(*s, 1939);
    EXPECT_EQ(s.use_count(), 1);
    EXPECT_EQ(s->i1, 1939);
    EXPECT_EQ(s->i2, 1939);
}
