simpletest
==========

A simple framework for testing and logging framework in C++.

TESTING:
-------

Have a look at `test1.cpp` file.

~~~
#include "simple_test.hpp"
#include <iostream>
using namespace std;

int main()
{
    /* Following two macros takes two arguments: first is condition and second
     * is message to print on console in case of failture
     */
    EXPECT_TRUE( 1 == 2, "1 == 2 must fail");

    /* TO instroduce colors into user-messages, sorround them with ` */
    EXPECT_TRUE( 1 == 2, "`1 == 2 must fail`");

    EXPECT_FALSE( 2 != 2, "2 != 2 must also fail");


    /* Following macros takes three argument and they try to mimic GTEST macros. */
    EXPECT_EQ(2, 3, "This should fail");

    /* This is colored. */
    EXPECT_NEQ(2, 2, "`I was expecting not equal to 2`");

    int expected = 3;
    EXPECT_GTE(2, expected, "I was expecting greater than equal to" << expected );
    EXPECT_GT(3, expected, "I was expecting greater than " << expected);

    EXPECT_LTE(5, expected, "I was expecting less than equal to " << expected);
    EXPECT_LT(3, expected, "I was expecting less than "<< expected);

    /* If a marco begins with ASSERT_ rather than EXPECT_ then it throws an
     * exception.
     */
    ASSERT_LT(3, expected, "I was expecting less than " << expected );

    return 0;
}

~~~

Following is its output.

~~~
Press ENTER or type command to continue
[EXPECT_FAILURE] 1 == 2 must fail
 + 
[EXPECT_FAILURE] In function: int main() file: test1.cpp:34
 + Expected 2, received 3
 + This should fail
 + 
[EXPECT_FAILURE] In function: int main() file: test1.cpp:35
 + Not expected 2
 + I was expecting not equal to 2
 + 
[EXPECT_FAILURE] In function: int main() file: test1.cpp:38
 + Expected greater than or equal to 2, received 3
 + I was expecting greater than equal to3
 + 
[EXPECT_FAILURE] In function: int main() file: test1.cpp:39
 + Expected greater than 3, received 3
 + I was expecting greater than 3
 + 
[EXPECT_FAILURE] In function: int main() file: test1.cpp:42
 + Expected less than 3, received 3
 + I was expecting less than 3
 + 
[EXPECT_FAILURE] In function: int main() file: test1.cpp:47
 + Expected less than 3, received 3
  1 Write readme and get over with it.                                               
 + I was expecting less than 3
 + 
[ASSERTION_FAILURE] I was expecting less than 3
terminate called after throwing an instance of 'FatalTestFailure'
/bin/bash: line 1: 30297 Aborted                 ./a.out

~~~

The colored output is not displayed in this log.
