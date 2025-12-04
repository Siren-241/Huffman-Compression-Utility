cmake_minimum_required(VERSION 3.14)

project(HuffmanCompressor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- 1. AUTOMATICALLY DOWNLOAD LIBRARIES ---
include(FetchContent)

# Download GLFW (Window Manager)
message(STATUS "Fetching GLFW...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# Download Dear ImGui (The GUI Library)
message(STATUS "Fetching ImGui...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking # 'docking' branch is best for modern features
)
FetchContent_MakeAvailable(imgui)

# --- 2. SETUP SOURCES ---

# We need to tell CMake where the "Backends" are (The bridge between ImGui and GLFW)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Your project files
set(PROJECT_SOURCES
    "src/main.cpp"
    "src/lib/HuffmanCoding.cpp"
)

# --- 3. CREATE EXECUTABLE ---
add_executable(Compressor ${PROJECT_SOURCES} ${IMGUI_SOURCES})

# --- 4. LINK LIBRARIES ---
target_include_directories(Compressor PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(Compressor PRIVATE glfw ${GLFW_LIBRARIES})

# Link OpenGL (Required for drawing)
if(WIN32)
    target_link_libraries(Compressor PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(Compressor PRIVATE "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
else()
    target_link_libraries(Compressor PRIVATE GL)
endif()

message(STATUS "Build setup complete. Run 'cmake --build .' to compile.")
