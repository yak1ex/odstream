all: libodstream.a

clean:
	-rm -rf *.o *.a *.exe

libodstream.a: odstream.o
	$(AR) r $@ $^

buildtest: libodstream.a
	$(CXX) -o test_nd_who test.cpp
	$(CXX) -o test_nd_nho -D YAK_DEBUG_NO_HEADER_ONLY test.cpp -L. -lodstream
	$(CXX) -o test_wd_who -D DEBUG test.cpp
	$(CXX) -o test_wd_nho -D DEBUG -D YAK_DEBUG_NO_HEADER_ONLY test.cpp -L. -lodstream

buildstatictest: libodstream.a
	$(CXX) -static -o test_nd_who_s test.cpp
	$(CXX) -static -o test_nd_nho_s -D YAK_DEBUG_NO_HEADER_ONLY test.cpp -L. -lodstream
	$(CXX) -static -o test_wd_who_s -D DEBUG test.cpp
	$(CXX) -static -o test_wd_nho_s -D DEBUG -D YAK_DEBUG_NO_HEADER_ONLY test.cpp -L. -lodstream
