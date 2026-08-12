file(GLOB SRC src/*.cpp)
add_library(algo_dynamic_programming_kadane STATIC ${SRC})
target_include_directories(algo_dynamic_programming_kadane PUBLIC include)

target_compile_features(algo_dynamic_programming_kadane PUBLIC cxx_std_23)
add_library(algo::dynamic_programming::kadane ALIAS algo_dynamic_programming_kadane)
