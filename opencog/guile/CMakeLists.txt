
# Boilerplate for the goofy config file nonsense. Argh.
DECLARE_GUILE_CONFIG_TARGET(SCM_CONFIG "opencog generate-config" "GENERATE_TEST")

ADD_SUBDIRECTORY (modules)

# More search-path file nonsense. I hate this.
WRITE_GUILE_CONFIG(
	${GUILE_BIN_DIR}/opencog/generate-config.scm
	SCM_CONFIG TRUE)
WRITE_GUILE_CONFIG(
	${GUILE_BIN_DIR}/opencog/generate-config-installable.scm
	SCM_CONFIG FALSE)
INSTALL(FILES
	${GUILE_BIN_DIR}/opencog/generate-config-installable.scm
	DESTINATION ${GUILE_SITE_DIR}/opencog
	RENAME generate-config.scm)
