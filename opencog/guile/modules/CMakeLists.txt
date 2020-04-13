
ADD_LIBRARY (guile-generate SHARED
	GenerateSCM.cc
)

TARGET_LINK_LIBRARIES(guile-generate
	generate
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)
ADD_GUILE_EXTENSION(SCM_CONFIG generate "opencog-ext-path-generate")

INSTALL (TARGETS guile-generate
	EXPORT GenerateTargets
	DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)
