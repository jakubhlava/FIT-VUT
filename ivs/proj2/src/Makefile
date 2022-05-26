.PHONY : profile pack clean test all doc guide run prerequisites
names = xhlava52_xvuthi00_xkraus13
version = "1.0"
build = ../src/build
deb_depends = python3 python3-pip devscripts debhelper doxygen texlive-latex-base texlive-latex-extra texlive-land-czechslovak
pip_depends = pipenv requests pyside2

all: clean alphacalc/__main__.py
	mkdir $(build)
	mkdir $(build)/clean
	python3 setup.py sdist
	mv dist/AlphaCalc-$(version).tar.gz $(build)/clean
	rm -rf src/dist
	cd $(build)/clean && tar -xf AlphaCalc-$(version).tar.gz
	mv $(build)/clean/AlphaCalc-$(version)/* $(build)/clean
	rm -rf $(build)/clean/AlphaCalc-$(version)
	cp -r buildfiles/* $(build)/clean/
	cd $(build)/clean/ && debuild
	mkdir ../build
	mv $(build)/*.deb ../build/
	mv $(build)/clean/*.tar.gz ../build/
	rm -rf $(build)
	rm -rf *.egg*

clean:
	rm -rf ../build
	rm -rf dist
	rm -rf $(build)
	rm -rf *.egg*
	rm -rf documentation

profile:
	python3 alphacalc/alphalib/utils/random_data_gen.py
	echo "Profiling s 10 vstupy" > profiler_output
	python3 alphamath_profiler.py < profiledata_10.txt >> ../profiling/profiler_output
	echo "Profiling se 100 vstupy" >> profiler_output
	python3 alphamath_profiler.py < profiledata_100.txt >> ../profiling/profiler_output
	echo "Profiling s 1000 vstupy" >> profiler_output
	python3 alphamath_profiler.py < profiledata_1000.txt >> ../profiling/profiler_output
	rm profiledata*

test:
	python3 alphamath_tests.py

doc:
	cd alphacalc && doxygen doxy

guide: userguide_source/main.tex
	cd userguide_source && pdflatex main.tex
	cd userguide_source && pdflatex main.tex
	rm -f *.out
	rm -f *.aux
	mv userguide_source/main.pdf ../dokumentace.pdf

run: alphacalc/__main__.py
	pipenv run python3 alphacalc/__main__.py

pack:
	make clean	
	rm -rf ../../$(names)
	rm -f ../../$(names).zip
	mkdir ../../$(names)
	mkdir ../../$(names)/repo
	mkdir ../../$(names)/doc
	mkdir ../../$(names)/install
	make guide
	cd .. && cp -r * ../$(names)/repo/ && cp -r .[!.]* ../$(names)/repo/
	make all
	mv ../build/*.deb ../../$(names)/install/
	make doc
	mv alphacalc/documentation/html/* ../../$(names)/doc/
	cd ../.. && zip $(names).zip -r $(names) 
	rm -rf ../../$(names)
	make clean

prerequisites:
	sudo add-apt-repository ppa:thopiekar/pyside-git
	sudo apt update
	sudo apt install $(deb_depends)
	pip3 install $(pip_depends)
	pipenv install
		
