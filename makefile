EXEDIR = binary/
INCDIR = include/
LIBDIR = lib/
OBJDIR = object/
SRCDIR = source/
PROTO_DIR = /usr/local/
CC = gcc
CFLAGSPROT = -Wall -g -O2 -I${PROTO_DIR}include/
LDFLAGS = ${PROTO_DIR}lib/libprotobuf-c.a -pthread
TARGET = $(EXEDIR)table-client $(EXEDIR)table-server $(LIBDIR)client-lib.o
OBJS = $(OBJDIR)sdmessage.pb-c.o $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)list.o $(OBJDIR)table.o $(OBJDIR)serialization.o $(OBJDIR)table_skel.o $(OBJDIR)network_server.o $(OBJDIR)message-private.o $(OBJDIR)table_stub.o $(OBJDIR)table_client.o $(OBJDIR)table_server.o $(OBJDIR)client_stub.o $(OBJDIR)network_client.o
COMPILE = $(CC) $(CFLAGSPROT) -I$(INCDIR) -MD -c -o $@
LINK = $(CC) $(OBJS) $(LDFLAGS) -o $@



ALL: $(TARGET)



$(EXEDIR)table-client: $(OBJDIR)sdmessage.pb-c.o $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)list.o $(OBJDIR)table.o $(OBJDIR)serialization.o $(OBJDIR)table_skel.o $(OBJDIR)message-private.o $(OBJDIR)table_client.o $(OBJDIR)client_stub.o $(OBJDIR)network_client.o
	$(CC) $(OBJDIR)sdmessage.pb-c.o $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)list.o $(OBJDIR)table.o $(OBJDIR)serialization.o $(OBJDIR)table_skel.o $(OBJDIR)message-private.o $(OBJDIR)table_client.o $(OBJDIR)client_stub.o $(OBJDIR)network_client.o $(LDFLAGS) -o $(EXEDIR)table-client

$(EXEDIR)table-server: $(OBJDIR)sdmessage.pb-c.o $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)list.o $(OBJDIR)table.o $(OBJDIR)serialization.o $(OBJDIR)table_skel.o $(OBJDIR)network_server.o $(OBJDIR)message-private.o $(OBJDIR)table_stub.o $(OBJDIR)table_server.o
	$(CC)  $(OBJDIR)sdmessage.pb-c.o $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)list.o $(OBJDIR)table.o $(OBJDIR)serialization.o $(OBJDIR)table_skel.o $(OBJDIR)network_server.o $(OBJDIR)message-private.o $(OBJDIR)table_stub.o $(OBJDIR)table_server.o $(LDFLAGS) -o $(EXEDIR)table-server

$(LIBDIR)client-lib.o: $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)network_client.o $(OBJDIR)client_stub.o
	ld -r $(OBJDIR)data.o $(OBJDIR)entry.o $(OBJDIR)network_client.o $(OBJDIR)client_stub.o -o $(LIBDIR)client-lib.o




$(OBJDIR)data.o: $(INCDIR)data.h
	$(COMPILE) $(SRCDIR)data.c

$(OBJDIR)entry.o: $(INCDIR)data.h $(INCDIR)entry.h
	$(COMPILE) $(SRCDIR)entry.c

$(OBJDIR)list.o: $(INCDIR)data.h $(INCDIR)entry.h $(INCDIR)list.h
	$(COMPILE) $(SRCDIR)list.c

$(OBJDIR)table.o: $(INCDIR)data.h $(INCDIR)entry.h $(INCDIR)table.h
	$(COMPILE) $(SRCDIR)table.c

$(OBJDIR)serialization.o: $(INCDIR)data.h $(INCDIR)entry.h $(INCDIR)serialization.h
	$(COMPILE) $(SRCDIR)serialization.c

$(OBJDIR)table_skel.o: $(INCDIR)table_skel.h $(INCDIR)message-private.h
	$(COMPILE) $(SRCDIR)table_skel.c

$(OBJDIR)network_client.o: $(INCDIR)client_stub-private.h $(INCDIR)client_stub.h $(INCDIR)sdmessage.pb-c.h $(INCDIR)message-private.h
	$(COMPILE) $(SRCDIR)network_client.c

$(OBJDIR)network_server.o: $(INCDIR)table_skel.h $(INCDIR)network_server.h
	$(COMPILE) $(SRCDIR)network_server.c

$(OBJDIR)message-private.o: $(INCDIR)sdmessage.pb-c.h $(INCDIR)message-private.h
	$(COMPILE) $(SRCDIR)message-private.c

$(OBJDIR)table_stub.o: $(INCDIR)data.h $(INCDIR)entry.h
	$(COMPILE) $(SRCDIR)table_stub.c

$(OBJDIR)table_client.o: $(INCDIR)data.h $(INCDIR)entry.h $(INCDIR)client_stub.h
	$(COMPILE) $(SRCDIR)table_client.c

$(OBJDIR)table_server.o: $(INCDIR)network_server.h
	$(COMPILE) $(SRCDIR)table_server.c

$(OBJDIR)client_stub.o: $(INCDIR)data.h $(INCDIR)entry.h $(INCDIR)client_stub.h $(INCDIR)serialization.h $(INCDIR)network_client.h
	$(COMPILE) $(SRCDIR)client_stub.c

$(OBJDIR)sdmessage.pb-c.o: $(INCDIR)sdmessage.pb-c.h
	$(COMPILE) $(SRCDIR)sdmessage.pb-c.c



$(SRCDIR)%.pb-c.c:$(SRCDIR)%.proto
	 ${PROTO_DIR}bin/protoc-c $(SRCDIR)sdmessage.proto --c_out=./$(SRCDIR)


-include $(OBJS:.o=.d)

clean:
	rm -f $(OBJDIR)* %~ %.d %.o $(TARGET) *pb-c.[ch]
