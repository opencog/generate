

ADD_LIBRARY(generate SHARED
	Aggregate.cc
	BasicParameters.cc
	CollectStyle.cc
	Dictionary.cc
	LinkStyle.cc
	Odometer.cc
	RandomCallback.cc
	SimpleCallback.cc
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
	GenerateCallback.h
	LinkStyle.h
	Odometer.h
	RandomCallback.h
	RandomParameters.h
	SimpleCallback.h
	DESTINATION "include/opencog/generate"
)
