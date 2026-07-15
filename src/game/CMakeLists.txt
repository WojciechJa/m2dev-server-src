file(GLOB_RECURSE GAME_SOURCES "*.h" "*.c" "*.cpp")

add_executable(game ${GAME_SOURCES})

target_compile_definitions(game PRIVATE GIT_DESCRIBE="${GIT_DESCRIBE_VERSION}")

target_link_libraries(game
	common
	libgame
	libpoly
	libsql
	libthecore
	liblua

	# external
	mariadbclient
	sodium
)

if (WIN32)
    target_link_libraries(game ws2_32)
    # Suppress C4244 from STL <utility> template instantiations with Lua number types
    target_compile_options(game PRIVATE /wd4244)
else()
	target_link_libraries(game pthread)
	if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
		find_library(CRYPT_LIBRARY crypt REQUIRED)
		target_link_libraries(game ${CRYPT_LIBRARY})
	endif()
	if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
		target_link_libraries(game md)
    endif()
endif()

if(BUILD_TESTING)
	add_executable(auth_password_test
		auth_password.cpp
		${CMAKE_SOURCE_DIR}/tests/auth_password_test.cpp
	)
	target_include_directories(auth_password_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(auth_password_test sodium)

	if(NOT WIN32)
		target_link_libraries(auth_password_test pthread)
		if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
			target_link_libraries(auth_password_test ${CRYPT_LIBRARY})
		endif()
	endif()

	add_test(NAME auth_password_test COMMAND auth_password_test)
endif()
