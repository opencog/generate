
ADD_LIBRARY (guile-generate
   GenerateSCM.cc
)

TARGET_LINK_LIBRARIES(guile-generate
   smob
   ${COGUTIL_LIBRARY}
)
ADD_GUILE_EXTENSION(SCM_CONFIG generate "opencog-ext-path-generate")

INSTALL (TARGETS guile-generate
	EXPORT AtomSpaceTargets
	DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)
