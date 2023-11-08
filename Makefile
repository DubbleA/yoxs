include common.mk

API_DIR = server
DB_DIR = data
REQ_DIR = .

.PHONY: prod github all_tests dev_env docs clean

prod: all_tests github

github:
	git commit -a
	git push origin master

#build all components
build: 
	$(MAKE) -C src/monkey build

# Run all tests
tests:
	$(MAKE) -C src/monkey tests

# Clean all components
clean:
	$(MAKE) -C src/monkey clean

dev_env:
	pip install -r $(REQ_DIR)/requirements-dev.txt

docs:
	cd $(API_DIR); make docs
