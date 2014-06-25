SCRIPTS = $(wildcard *.py)
PROFILE_FILES := $(patsubst %.py,%.py.profile, ${SCRIPTS})

all : test

test : ${PROFILE_FILES}

%.py.profile : %.py
	@echo "|| Testing and profiling $<"
	${PYC_PROFILER} -o $@ $< 
