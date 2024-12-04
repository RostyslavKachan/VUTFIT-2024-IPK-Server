# Compiler to be used is g++
CPP = g++
# Compiler flags
CXXFLAGS = -Wall -g
# Target executable name
TARGET = ipk24chat-server
# Object files that make up the project
OBJS = main.o MSG_validate.o Server.o Server_cout.o Server_reply_construc.o TCP.o 
#making executable file
$(TARGET): $(OBJS)
	$(CPP) $(CXXFLAGS) -o $(TARGET) $(OBJS)

#making object files
%.o: %.cpp
	$(CPP) $(CXXFLAGS) -c $<

#delete object files and target
clean:
	rm -f $(OBJS) $(TARGET)
