include ../Makefile.inc

SCRIPTS = $(wildcard *.py)
PROFILE_FILES := $(patsubst %.py,%.py.profile, ${SCRIPTS})
LOG_FILES := $(patsubst %.py,%.py.log, ${SCRIPTS})

all : test

test : ${LOG_FILES}

%.py.profile : %.py
	@echo "|| Testing and profiling $<"
	${PYC_PROFILER} -o $@ $< 

%.py.log : %.py
	@echo "|| Running scripts and logging results: $<"
	${PYC} $<  > $@ || (echo "Failed to execute $<. Deleting log."; rm -f $@)

clean:
	-rm -f ${LOG_FILES} ${PROFILE_FILES}
