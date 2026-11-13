function(planova_add_test name)
    add_executable(${name} ${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Qt6::Test planova_core)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# --- S4 ---
function(planova_add_ui_test name)
    add_executable(${name} ${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Qt6::Test planova_ui)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

planova_add_test(tst_ScoringWeights)
planova_add_test(tst_WeightedStrategy)
planova_add_test(tst_PizzaPortions)
planova_add_test(tst_SlicerService)
planova_add_ui_test(tst_WeightRebalance)
# --- end S4 ---

planova_add_test(tst_Skeleton)
planova_add_test(tst_FileEventStore)
planova_add_test(tst_EventLog)
planova_add_test(tst_ProjectService)