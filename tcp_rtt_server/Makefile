CXXFLAGS = -g -O2 -Wall -fmessage-length=0

OBJS = tcp_rtt_server.o

LIBS =

TARGET =	tcp_rtt_server

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
