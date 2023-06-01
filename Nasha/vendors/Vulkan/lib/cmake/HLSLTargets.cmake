
        message(WARNING "Using `HLSLTargets.cmake` is deprecated: use `find_package(glslang)` to find glslang CMake targets.")

        if (NOT TARGET glslang::HLSL)
            include("/Users/lunarg/Dev/macos-sdk-build/Glslang/build/install/lib/cmake/glslang/glslang-targets.cmake")
        endif()

        add_library(HLSL ALIAS glslang::HLSL)
    