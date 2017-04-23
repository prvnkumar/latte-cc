AM_CPPFLAGS = $(CXX11_FLAGS) -I$(srcdir)/../src
AM_CXXFLAGS = $(PICKY_CXXFLAGS)
LDADD = ../src/libsourdough.a -lpthread

common_source = contest_message.hh contest_message.cc \
	controller.hh controller.cc aimdcontroller.cc aimdcontroller.hh \
	rttcontroller.hh rttcontroller.cc rttaimdcontroller.hh rttaimdcontroller.cc \
	metacontroller.hh metacontroller.cc

bin_PROGRAMS = sender receiver

sender_SOURCES = $(common_source) sender.cc

receiver_SOURCES = $(common_source) receiver.cc
