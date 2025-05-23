if(APPLE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "AppleClang compiler detected: using system libunwind.")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            if("${CMAKE_CXX_COMPILER}" MATCHES "/opt/homebrew/")
                set(LIBUNWIND_PREFIX "${LLVM_LIBRARY_DIR}/unwind")
                message(STATUS "Homebrew Clang compiler detected: using libunwind from ${LIBUNWIND_PREFIX}")
            elseif("${CMAKE_CXX_COMPILER}" MATCHES "/opt/local/")
                set(LIBUNWIND_PREFIX "${LLVM_LIBRARY_DIR}/libunwind")
                message(STATUS "MacPorts Clang compiler detected: using libunwind from ${LIBUNWIND_PREFIX}")
            endif()
        elseif("${CMAKE_CXX_COMPILER}" MATCHES "/usr/local/opt/" OR "${CMAKE_CXX_COMPILER}" MATCHES "/usr/local/Cellar/")
            set(LIBUNWIND_PREFIX "${LLVM_LIBRARY_DIR}")
            message(STATUS "Homebrew Clang compiler detected: using libunwind from ${LIBUNWIND_PREFIX}")
        elseif("${CMAKE_CXX_COMPILER}" MATCHES "/opt/local/")
                set(LIBUNWIND_PREFIX "${LLVM_LIBRARY_DIR}/libunwind")
                message(STATUS "MacPorts Clang compiler detected: using libunwind from ${LIBUNWIND_PREFIX}")
        endif()
        set(LIBUNWIND_LIBRARY ${LIBUNWIND_PREFIX}/libunwind.dylib)
    endif()
endif(APPLE)
