

ADD_LIBRARY(generate SHARED
	Aggregate
	BasicParameters
	CollectSolutions
	Dictionary
	Frame
	RandomCallback
	SimpleCallback
)

TARGET_LINK_LIBRARIES(generate
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
	uuid
)

INSTALL(TARGETS generate
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

INSTALL(FILES
	Aggregate.h
	BasicParameters.h
	CollectSolutions.h
	Dictionary.h
	Frame.h
	GenerateCallback.h
	RandomCallback.h
	RandomParameters.h
	SimpleCallback.h
	DESTINATION "include/opencog/generate"
)
