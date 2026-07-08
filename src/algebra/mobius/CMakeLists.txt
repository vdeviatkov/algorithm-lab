file(GLOB SRC src/*.cpp)
add_library(algo_algebra_mobius STATIC ${SRC})
target_include_directories(algo_algebra_mobius PUBLIC include)

target_compile_features(algo_algebra_mobius PUBLIC cxx_std_23)
add_library(algo::algebra::mobius ALIAS algo_algebra_mobius)
