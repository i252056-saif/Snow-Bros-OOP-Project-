set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/Users/Saif Ali/Downloads/SFML-2.6.1-windows-vc17-64-bit/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window audio system REQUIRED)

set(SOURCES
    main.cpp
    AudioManager.cpp
    Database.cpp
    Enemy.cpp
    Game.cpp
    GameWorld.cpp
    HUD.cpp
    HitBox.cpp
    LevelConfig.cpp
    Platform.cpp
    Player.cpp
    PowerUp.cpp
    Projectile.cpp
)

add_executable(snowbros ${SOURCES})

target_link_libraries(snowbros PRIVATE
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
)

if(MSVC)
    target_compile_options(snowbros PRIVATE /wd26495)
endif()

add_custom_command(TARGET snowbros POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
        "$<TARGET_FILE_DIR:snowbros>/assets"
    COMMENT "Copying assets..."
)

add_custom_command(TARGET snowbros POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/Users/Saif Ali/Downloads/SFML-2.6.1-windows-vc17-64-bit/SFML-2.6.1/bin"
        "$<TARGET_FILE_DIR:snowbros>"
    COMMENT "Copying SFML DLLs..."
)