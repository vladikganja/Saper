set(ProjectId code)
project(${ProjectId})

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.hpp)

add_executable(${ProjectId} ${ALL_SOURCE_FILES})
target_include_directories(${ProjectId} PUBLIC .)
target_link_libraries(${ProjectId} sfml-graphics)
