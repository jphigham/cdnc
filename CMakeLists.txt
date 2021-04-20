cmake_minimum_required(VERSION 3.1.0)

project(cdnc LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
  set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)

if (WIN32)
  set(BUILD_SHARED_LIBS OFF)
  set(BUILD_STATIC_LIBS ON)
endif()

#------#
# jpeg #
#------#

set(WITH_JPEG8 ON)
set(JPEG_TURBO_SOURCE_DIR "third_party/libjpeg-turbo")
set(CMAKE_INSTALL_DOCDIR "${CMAKE_BINARY_DIR}/${JPEG_TURBO_SOURCE_DIR}/doc")
add_subdirectory(${JPEG_TURBO_SOURCE_DIR})

#------#
# curl #
#------#

if (WIN32)
  set(CMAKE_USE_SCHANNEL ON)
endif()
add_subdirectory(third_party/curl)

#-----#
# glm #
#-----#

set(GLM_SOURCE_DIR "third_party/glm")
add_subdirectory(${GLM_SOURCE_DIR})

#------#
# glfw #
#------#

set(GLFW_SOURCE_DIR "third_party/glfw")
add_subdirectory(${GLFW_SOURCE_DIR})

#-----------#
# rapidjson #
#-----------#

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
set(BUILD_GTEST OFF)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF)
set(RAPIDJSON_BUILD_CXX17 ON)
add_subdirectory(third_party/rapidjson)

#------#
# cdnc #
#------#

if (WIN32)
    set(GLFW_ICON ${GLFW_SOURCE_DIR}/examples/glfw.rc)
endif()

set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad_gl.c")

add_executable(cdnc
	${GLFW_ICON}
	${GLAD_GL}
	main.cpp
)

target_include_directories(cdnc PRIVATE ${GLM_SOURCE_DIR})

target_include_directories(cdnc PRIVATE ${GLFW_SOURCE_DIR}/include)
target_include_directories(cdnc PRIVATE ${GLFW_SOURCE_DIR}/deps)

target_include_directories(cdnc PRIVATE third_party/curl/include)
target_include_directories(cdnc PRIVATE third_party/rapidjson/include)

target_include_directories(cdnc PRIVATE ${JPEG_TURBO_SOURCE_DIR})
target_include_directories(cdnc PRIVATE ${CMAKE_BINARY_DIR}/${JPEG_TURBO_SOURCE_DIR})

target_link_libraries(cdnc
  PUBLIC
    # OpenGL::GL
    # OpenGL::GLU
    libcurl
	glfw
	jpeg-static
)
