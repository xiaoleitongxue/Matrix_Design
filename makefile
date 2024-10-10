
cc = clang
cpp = clang++
build_dir := ./build

$(build_dir)/Matrix_Design.o : Matrix_Design.cpp Matrix_Design.h
	$(cpp) -c $< -IC:\Users\lilei\Documents\GitHub\Matrix_Design -o $@


clean:
	rm -f build/Matrix_Design.o