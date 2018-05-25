# Polyglot in make and nmake
# ref. https://stackoverflow.com/a/30906085
#\
!if 1 #\
# [[[nmake]]] \
CFLAGS=/TC #\
CXXFLAGS=/EHsc #\
LIBNAME=odstream.lib #\
LIBSNAME=odstream_s.lib #\
LIBARG=$(LIBNAME) #\
LIBSARG=$(LIBSNAME) #\
RM=del #\
RMARGS=*.obj *.lib *.exe *.ilk *.pdb #\
LINKARG=-MD -Fe#\
LINKSARG=-MT -Fe#\
!else
# [[[make]]]
LIBNAME=libodstream.a
LIBSNAME=libodstream.a
LIBARG=-L. -lodstream
LIBSARG=-L. -lodstream
RM=-rm -rf
RMARGS=*.o *.a *.exe
LINKARG= -o 
LINKSARG=-static -o 
#\
!endif

########################################################################
#
# Common

all: $(LIBNAME) $(LIBSNAME)

clean:
	$(RM) $(RMARGS)

buildptest:
	$(CXX) $(CXXFLAGS) -D DEBUG $(LINKARG)testp_wd_cpp test_odprintf.c
	$(CXX) $(CXXFLAGS)          $(LINKARG)testp_nd_cpp test_odprintf.c
	$(CC)  $(CFLAGS)   -D DEBUG $(LINKARG)testp_wd_c   test_odprintf.c
	$(CC)  $(CFLAGS)            $(LINKARG)testp_nd_c   test_odprintf.c

buildtest: $(LIBNAME)
	$(CXX) $(CXXFLAGS) $(LINKARG)test_blank blank.cpp
	$(CXX) $(CXXFLAGS) $(LINKARG)test_nd_who test_odstream.cpp
	$(CXX) $(CXXFLAGS) $(LINKARG)test_nd_nho -D YAK_DEBUG_NO_HEADER_ONLY test_odstream.cpp $(LIBARG)
	$(CXX) $(CXXFLAGS) $(LINKARG)test_wd_who -D DEBUG test_odstream.cpp
	$(CXX) $(CXXFLAGS) $(LINKARG)test_wd_nho -D DEBUG -D YAK_DEBUG_NO_HEADER_ONLY test_odstream.cpp $(LIBARG)

buildstatictest: $(LIBSNAME)
	$(CXX) $(CXXFLAGS) $(LINKSARG)test_blank_s blank.cpp
	$(CXX) $(CXXFLAGS) $(LINKSARG)test_nd_who_s test_odstream.cpp
	$(CXX) $(CXXFLAGS) $(LINKSARG)test_nd_nho_s -D YAK_DEBUG_NO_HEADER_ONLY test_odstream.cpp $(LIBSARG)
	$(CXX) $(CXXFLAGS) $(LINKSARG)test_wd_who_s -D DEBUG test_odstream.cpp
	$(CXX) $(CXXFLAGS) $(LINKSARG)test_wd_nho_s -D DEBUG -D YAK_DEBUG_NO_HEADER_ONLY test_odstream.cpp $(LIBSARG)

########################################################################
#
# UNIX-ish

# Use conditional guard because nmake complains $^
#\
!if 0
libodstream.a: odstream.o
	$(AR) r $@ $^
#\
!endif

########################################################################
#
# Windows

odstream.lib: odstream_md.obj
	lib /out:$@ $**

odstream_s.lib: odstream_mt.obj
	lib /out:$@ $**

odstream_md.obj: odstream.cpp
	$(CXX) $(CXXFLAGS) -MD -c -Fo$@ $**

odstream_mt.obj: odstream.cpp
	$(CXX) $(CXXFLAGS) -MT -c -Fo$@ $**

