TEXT_DIR = textgame
REQ_DIR = .

FORCE:

prod: tests github

tests: FORCE
	export PYTHONPATH="${PYTHONPATH}:/Users/nlmb/Documents/TextGame"; \
	cd $(TEXT_DIR); make tests

github: FORCE
	- git commit -a
	git push origin master

dev_env: FORCE
	pip3 install -r $(REQ_DIR)/requirements-dev.txt

docs: FORCE
	cd $(TEXT_DIR); make docs
