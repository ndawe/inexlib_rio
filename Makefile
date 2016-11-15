.PHONY: examples

clean:
	rm -rf examples/bin

style:
	find inlib -type f | xargs astyle --options=dot.astylerc

examples:
	mkdir -p examples/bin
	g++ examples/wroot.cpp -o examples/bin/wroot -I. -lz
	g++ examples/rroot.cpp -o examples/bin/rroot -I. -lz
