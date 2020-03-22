

ADD_LIBRARY(generate SHARED
	Aggregate
	DefaultCallback
)

TARGET_LINK_LIBRARIES(generate
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)

INSTALL(TARGETS generate
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

INSTALL(FILES
	Aggregate.h
	DefaultCallback.h
	Dictionary.h
	Frame.h
	GenerateCallback.h
	DESTINATION "include/opencog/generate"
)
