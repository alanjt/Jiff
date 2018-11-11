
set(SOURCES
    App.cpp
    cmdLineArgs.cpp
    main.cpp
    Plot.cpp
    ScanFile.cpp
    )
	
set(HEADERS
    App.h
    config.h
	)

set( simgear_libs
     ${SIMGEAR_LIBRARIES}
     ${SIMGEAR_CORE_LIBRARY_DEPENDENCIES}
     ${SIMGEAR_SCENE_LIBRARY_DEPENDENCIES})

add_executable(jiff WIN32 ${SOURCES} ${HEADERS})


target_link_libraries(jiff
    ${simgear_libs}
    ${ZLIB_LIBRARIES}
    )

install(TARGETS jiff RUNTIME DESTINATION bin)
