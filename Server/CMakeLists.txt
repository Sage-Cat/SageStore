cmake_minimum_required(VERSION 3.15)

project(Server)

# Add server-specific dependencies

# Specify source files
set(SERVER_SOURCES
    src/main.cpp
    # src/API_Endpoints/...
    # src/BusinessLogic/...
    # src/DataAccessLayer/...
    # src/Database/...
)

# Create server executable
add_executable(SageStoreServer ${SERVER_SOURCES})
