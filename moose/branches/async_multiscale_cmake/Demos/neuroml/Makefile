PYC := python
## Color support in Makefile
export

SRCS = allChannelsCell \
#    CA1PyramidalCell \
#    GranuleCell \
#    LIF \
#    lobster_pyloric \
#    OlfactoryBulbPassive \
#    PurkinjeCellPassive
#
all : test 

test: $(SRCS)
	for curr_dir in ${SRCS}; \
	    do \
		echo "++ Testing $${curr_dir}"; \
		$(MAKE) -C $${curr_dir}; \
	    done

