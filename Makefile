all: libodstream.a

clean:
	-rm -rf *.o *.a

libodstream.a: odstream.o
	$(AR) r $@ $^

buildtest:
	$(CXX) -o test_nd_who test.cpp
	$(CXX) -o test_nd_nho -D YAK_DEBUG_NO_HEADER_ONLY test.cpp -L. -lodstream
	$(CXX) -o test_wd_who -D DEBUG test.cpp
	$(CXX) -o test_wd_nho -D DEBUG -D YAK_DEBUG_NO_HEADER_ONLY test.cpp -L. -lodstream
