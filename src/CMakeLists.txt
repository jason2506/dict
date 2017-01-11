find_package(MsgPack 2.1.0 REQUIRED)

add_library(${PROJECT_NAME}
    permutation.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE MsgPack::MsgPack)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

string(TOLOWER ${PROJECT_NAME} OUTPUT_NAME)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME ${OUTPUT_NAME}
        CLEAN_DIRECT_OUTPUT 1
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
