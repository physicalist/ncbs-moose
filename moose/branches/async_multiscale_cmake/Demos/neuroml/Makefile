include Makefile.inc
SRCS = allChannelsCell \
       CA1PyramidalCell \
       GranuleCell \
       LIF \
       lobster_pyloric \
       OlfactoryBulbPassive \
       PurkinjeCellPassive

all : ${SRCS} 

.PHONY: subdirs ${SRCS}

subdirs := ${SRCS}

${SRCS}:
	$(MAKE) -C $@

clean:
	for dir in ${SRCS}; do \
	    $(MAKE) -C $$dir clean; \
	done

.PHONEY: clean all subdirs 
