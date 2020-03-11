

ADD_LIBRARY(generate SHARED
	Aggregate
	Frame
)

TARGET_LINK_LIBRARIES(generate
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)

INSTALL(TARGETS generate
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

INSTALL(FILES
	Aggregate.h
	Frame.h
	DESTINATION "include/opencog/generate"
)
