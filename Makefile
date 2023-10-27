include common.mk

API_DIR = server
DB_DIR = data
REQ_DIR = .

.PHONY: prod github all_tests dev_env docs clean

prod: all_tests github

github:
   - git commit -a
   git push origin master

build: 
	$(MAKE) -C $(SRV_DIR) build
	$(MAKE) -C $(API_DIR) build

all_tests:
   cd $(API_DIR); make tests
   cd $(DB_DIR); make tests

dev_env:
   pip install -r $(REQ_DIR)/requirements-dev.txt

docs:
   cd $(API_DIR); make docs

clean:
   cd $(API_DIR); make clean
   cd $(DB_DIR); make clean
