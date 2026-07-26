file(GLOB SRC src/*.cpp)
add_library(algo_algebra_totient STATIC ${SRC})
target_include_directories(algo_algebra_totient PUBLIC include)

target_compile_features(algo_algebra_totient PUBLIC cxx_std_23)
add_library(algo::algebra::totient ALIAS algo_algebra_totient)
