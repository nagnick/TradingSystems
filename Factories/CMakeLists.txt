file(GLOB FactoriesSRC_FILES    
    "*.h"
)
target_sources(main PUBLIC
    ${FactoriesSRC_FILES}
)