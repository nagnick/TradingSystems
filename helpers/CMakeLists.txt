file(GLOB helpersSRC_FILES    
    "*.h"
)

target_sources(main PUBLIC
    ${helpersSRC_FILES}
)