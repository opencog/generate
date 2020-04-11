

ADD_LIBRARY(generate SHARED
	Aggregate
	Dictionary
	RandomCallback
	SimpleCallback
)

TARGET_LINK_LIBRARIES(generate
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)

INSTALL(TARGETS generate
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")

INSTALL(FILES
	Aggregate.h
	Dictionary.h
	Frame.h
	GenerateCallback.h
	RandomCallback.h
	SimpleCallback.h
	DESTINATION "include/opencog/generate"
)
