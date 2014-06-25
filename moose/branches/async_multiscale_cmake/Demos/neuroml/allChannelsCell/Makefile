SCRIPTS = allChannelsCell.py

all : test

test : ${SCRIPTS}
	for script in ${SCRIPTS}; \
	    do \
		echo "|| Testing $${script}"; \
		$${PYC} $${script}; \
	    done 
