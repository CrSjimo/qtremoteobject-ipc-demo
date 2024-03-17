file(GLOB_RECURSE _src *.cpp *.h)

qt5_generate_repc(_src ../demo.rep REPLICA)

add_executable(parent ${_src})

target_link_libraries(parent Qt5::Core Qt5::Gui Qt5::Widgets Qt5::RemoteObjects)

target_include_directories(parent PRIVATE ${CMAKE_CURRENT_BINARY_DIR})