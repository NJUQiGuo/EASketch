objs = main.o hash.o hopping.o utils.o env.o countsk.o  cusketch.o  PCMSketch.o BOBHash.o DynamicCMSketch.o DynamicSketchWithCompress.o
CXXFLAGS = -O2 -std=c++11

main: $(objs)
	g++ -O2 -o main $(objs) -lpcap

.PHONY : clean
clean:
	rm main $(objs)