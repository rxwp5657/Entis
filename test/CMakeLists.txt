set(TEST_NAME entis_test)

add_subdirectory(../lib/gtest lib/gtest)

add_executable(
    ${TEST_NAME} 
    main.cpp 
    sparse_set_test.cpp
    registry_test.cpp
    type_list_test.cpp
)

target_link_libraries(
    ${TEST_NAME}
    ${PROJECT_NAME}
    gtest 
)
