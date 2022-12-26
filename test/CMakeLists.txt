project(test)

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(test Day1.cpp Day2.cpp Day3.cpp Day4.cpp Day5.cpp Day6.cpp Day7.cpp Day8.cpp Day9.cpp Day10.cpp Day11.cpp Day12.cpp Day13.cpp Day14.cpp Day15.cpp position.h span_list_test.cpp span_list.h Day16.cpp util.h Day17.cpp Day18.cpp pos3.h Day19.cpp Day20.cpp Day21.cpp)
target_link_libraries(test GTest::gtest_main)
#add_test(NAME test_test COMMAND test)

include(GoogleTest)
gtest_discover_tests(test)

#add_subdirectory(lib)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})