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

# --- S4 ---
target_sources(planova_ui PRIVATE
    slicer/WeightRebalance.cpp
    slicer/WeightSlidersPanel.cpp
)

add_executable(slicer_demo ../tools/slicer_demo/main.cpp)
target_include_directories(slicer_demo PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(slicer_demo PRIVATE planova_ui)
# --- end S4 ---

add_executable(planova app/main.cpp)
target_link_libraries(planova PRIVATE planova_ui)