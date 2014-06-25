PYC := python
## Color support in Makefile
export

SRCS = allChannelsCell \
       CA1PyramidalCell \
#    GranuleCell \
#    LIF \
#    lobster_pyloric \
#    OlfactoryBulbPassive \
#    PurkinjeCellPassive
#
all : ${SRCS} 


.PHONY: subdirs ${SRCS}

subdirs := ${SRCS}

${SRCS}:
	$(MAKE) -C $@

