file(GLOB StreamsSRC_FILES    
    "*.h"
)

target_sources(main PUBLIC
    ${StreamsSRC_FILES}
)