include Makefile.inc
CLEAN_RECORD := $(shell rm -f ${SUCCESS_FILE} ${FAILURE_FILE})

SRCS = allChannelsCell \
       CA1PyramidalCell \
       GranuleCell \
       LIF \
       lobster_pyloric \
       OlfactoryBulbPassive \
       PurkinjeCellPassive

all : ${SRCS} 
	@echo "\n\n============================================================"
	@echo "Successful scripts: "
	@echo `cat ${SUCCESS_FILE}`
	@echo "\n\n============================================================"
	@echo "Failed scripts: "
	@echo `cat ${FAILURE_FILE}`

subdirs := ${SRCS}

${SRCS}:
	$(MAKE) -C $@

clean:
	for dir in ${SRCS}; do \
	    $(MAKE) -C $$dir clean; \
	done 
	-rm -f $$SUCCESS_FILE
	-rm -f $$FAILURE_FILE

.PHONY: subdirs ${SRCS}
.PHONY: clean all subdirs 
