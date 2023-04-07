file(GLOB TradingAlgosSRC_FILES    
    "*.h"
)
add_subdirectory(States)
target_sources(main PUBLIC
    ${TradingAlgosSRC_FILES}
)