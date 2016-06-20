.PHONY: clean
clean:
	rm -f results.json winner.json temp.txt datascores.json distsims.json

.PHONY: dtest
dtest:
	python3 -m $@ hash

.PHONY: deploy
deploy:
	make -C paper deploy
