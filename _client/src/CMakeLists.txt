# Specify resource files
set(RESOURCE_FILES "UI/qml/qml.qrc")

set(CLIENT_SOURCES
    ${APPLICATION_LOGIC_SOURCES}
    ${COMMUNICATION_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)

# Create client executable
add_executable(SageStoreClient ${CLIENT_SOURCES} ${RESOURCE_FILES})

# Include directories
target_include_directories(SageStoreClient PUBLIC ${CLIENT_SOURCES_DIR})

# Link libraries
target_link_libraries(SageStoreClient Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Quick spdlog::spdlog)

# Set the destination directory for the executable
set_target_properties(SageStoreClient PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CLIENT_BUILD_DIR})
