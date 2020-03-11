

ADD_LIBRARY(generate SHARED
	aggregate
)

TARGET_LINK_LIBRARIES(generate
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)

INSTALL(TARGETS generate
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

INSTALL(FILES
	aggregate.h
	DESTINATION "include/opencog/generate"
)
