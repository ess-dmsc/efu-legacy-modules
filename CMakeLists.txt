#=============================================================================
# Add Legacy ESS detector modules
#=============================================================================

include_directories(.)

add_subdirectory(gdgem)
add_subdirectory(multiblade)
add_subdirectory(sonde)
add_subdirectory(jalousie)


# Example functionality below to add custom targets to run the efu with different modules

# add_custom_target(copycfgs
#    COMMAND ${CMAKE_COMMAND} -E make_directory #${PROJECT_BINARY_DIR}/configs
#    COMMAND ${CMAKE_COMMAND} -E copy ${MG_CFG_FILE} #${PROJECT_BINARY_DIR}/configs/
#    COMMAND ${CMAKE_COMMAND} -E copy ${MB_CFG_FILE} #${PROJECT_BINARY_DIR}/configs/
#    COMMENT "Copying ${MB_CFG_FILE} to ${PROJECT_BINARY_DIR}/configs"
#    COMMAND ${CMAKE_COMMAND} -E copy ${GDGEM_CFG_FILE} #${PROJECT_BINARY_DIR}/configs/
#    COMMAND ${CMAKE_COMMAND} -E copy ${JALOUSIE_CFG_FILE} ${PROJECT_BINARY_DIR}/configs/
#    COMMAND ${CMAKE_COMMAND} -E copy ${DREAM_CFG_FILE} ${PROJECT_BINARY_DIR}/configs/
#    COMMAND ${CMAKE_COMMAND} -E copy ${JALOUSIE_CSV_FILE} ${PROJECT_BINARY_DIR}/configs/
#    COMMAND ${CMAKE_COMMAND} -E copy ${LOKI_CFG_FILE} ${PROJECT_BINARY_DIR}/configs/
#    COMMAND ${CMAKE_COMMAND} -E copy ${FREIA_CFG_FILE} ${PROJECT_BINARY_DIR}/configs/
#    COMMENT "Copying json configuration files to ${PROJECT_BINARY_DIR}/configs"
#)

# add_custom_target(runefu
#    COMMAND efu "-d" "../modules/mgmesytec" "-f" "../configs/ILL_mappings.json" "-s" "1" "--dumptofile" "deleteme_" "--nohwcheck"
#    COMMAND efu "-d" "../modules/sonde" "-s" "1" "--dumptofile" "deleteme_" "--nohwcheck"
#    COMMAND efu "-d" "../modules/mbcaen" "-f" "../configs/MB18Estia.json"  "-s" "1" "--dumptofile" "deleteme_" "--nohwcheck"
#    COMMAND efu "-d" "AdcReadout" "-s" "1" "--nohwcheck"
#    COMMAND efu "-d" "../modules/loki" "-f" "../configs/STFCTest.json" "-s" "1" "--nohwcheck" "--dumptofile" "deleteme_"
#    COMMAND efu "-d" "../modules/gdgem" "-f" "../configs/vmm3.json" "-s" "2" "--nohwcheck"
#    COMMAND efu "-d" "../modules/perfgen" "-s" "1" "--nohwcheck"
#    COMMAND efu "-d" "../modules/jalousie" "-f" "../configs/v20_mappings.json" "-s" "2" "--nohwcheck"
#    COMMAND efu "-d" "../modules/dream" "-f" "../configs/DreamEndcap.json" "-s" "2" "--nohwcheck"
#    COMMAND efu "-d" "../modules/freia" "-f" "../configs/freia.json" "-s" "2" "--nohwcheck"
#    DEPENDS copycfgs efu mgmesytec gdgem sonde mbcaen AdcReadout jalousie dream loki freia
#)