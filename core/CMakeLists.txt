add_library(planova_core STATIC
    src/PlanovaCore.cpp
    src/common/ChangeBus.cpp
    src/common/Ids.cpp
    src/project/ProjectProjection.cpp
    src/project/ProjectService.cpp
    src/store/FileEventStore.cpp
    src/store/EventLog.cpp
)
target_include_directories(planova_core PUBLIC include)
target_link_libraries(planova_core PUBLIC Qt6::Core)