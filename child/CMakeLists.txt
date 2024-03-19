file(GLOB_RECURSE _src *.cpp *.h)

qt5_generate_repc(_src ../demo.rep SOURCE)

add_executable(child ${_src})

target_link_libraries(child Qt5::Core Qt5::RemoteObjects Qt5::Concurrent)

target_include_directories(child PRIVATE ${CMAKE_CURRENT_BINARY_DIR})