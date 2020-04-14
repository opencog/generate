

ADD_LIBRARY(generate SHARED
	Aggregate
	BasicParameters
	CollectStyle
	Dictionary
	Frame
	LinkStyle
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
	CollectStyle.h
	Dictionary.h
	Frame.h
	GenerateCallback.h
	LinkStyle.h
	RandomCallback.h
	RandomParameters.h
	SimpleCallback.h
	DESTINATION "include/opencog/generate"
)
