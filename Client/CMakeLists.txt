cmake_minimum_required(VERSION 3.15)

project(Client)

# Add client-specific dependencies

# Specify source files
set(CLIENT_SOURCES
    src/main.cpp
    # src/ApplicationLogic/...
    # src/CommunicationLayer/...
    # src/UI/...
)

# Create client executable
add_executable(SageStoreClient ${CLIENT_SOURCES})
