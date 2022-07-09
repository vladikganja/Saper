set(CMAKE_CXX_STANDARD 17)

if ( WIN32 )
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W3 -WX -w44834 -w34189 -w34100 -w34715 -w34244 -EHsc" )
endif ( WIN32 )

if ( UNIX )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-unused-result")
endif ( UNIX )
