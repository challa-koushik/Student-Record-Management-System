cmake_minimum_required(VERSION 3.10)
project(srms VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt auto-moc, auto-uic, auto-rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5 modules
find_package(Qt5 COMPONENTS
    Widgets
    Sql
    Core
    Gui
REQUIRED)

# Source files
set(SOURCES
    main.cpp
    srmswindow.cpp
    marksdialog.cpp
    attendancedialog.cpp
)

# Header files
set(HEADERS
    srmswindow.h
    marksdialog.h
    attendancedialog.h
)

# Executable target
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Sql
    Qt5::Core
    Qt5::Gui
)

# Windows-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
    )

    # Copy Qt DLLs automatically (optional)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt5::Widgets>
            $<TARGET_FILE:Qt5::Sql>
            $<TARGET_FILE:Qt5::Core>
            $<TARGET_FILE:Qt5::Gui>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
