add_executable(test_dynamic_programming_kadane kadane/test_kadane.cpp)
target_link_libraries(test_dynamic_programming_kadane PRIVATE
        algo_dynamic_programming_kadane
        GTest::gtest_main
)
add_test(NAME dynamic_programming.kadane COMMAND test_dynamic_programming_kadane)
