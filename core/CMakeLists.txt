add_library(planova_core STATIC
    src/PlanovaCore.cpp
    src/common/ChangeBus.cpp
    src/common/Ids.cpp
    src/project/ProjectProjection.cpp
    src/project/ProjectService.cpp
    src/store/FileEventStore.cpp
    src/store/EventLog.cpp
    include/planova/team/SkillCategory.h
    include/planova/team/Member.h
    include/planova/team/MemberProjection.h
    include/planova/team/MemberService.h
    src/team/MemberService.cpp
)
target_include_directories(planova_core PUBLIC include)
target_link_libraries(planova_core PUBLIC Qt6::Core)

# --- S4 ---
target_sources(planova_core PRIVATE
    src/slicer/WeightedScoringStrategy.cpp
    src/slicer/PizzaPortionCalculator.cpp
    src/slicer/SlicerService.cpp
)
# --- end S4 ---
