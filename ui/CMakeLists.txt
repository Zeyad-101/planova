add_library(planova_ui STATIC
    theme/ThemeManager.cpp
    app/CoreBridge.cpp
    app/MainWindow.cpp
    welcome/WelcomePage.cpp
    welcome/ProjectsHubPage.cpp
    ../resources/planova.qrc
    welcome/SetupWizard.cpp
)
target_include_directories(planova_ui PUBLIC .)
target_link_libraries(planova_ui PUBLIC Qt6::Widgets planova_core)

add_executable(planova app/main.cpp)
target_link_libraries(planova PRIVATE planova_ui)