CXX := g++
CXXFLAGS := -std=c++14 -lpthread
LDFLAGS := -lpthread

.PHONY: all clean

producer_consumer: producer_consumer.cpp
	$(CXX) $(CXXFLAGS) producer_consumer.cpp $(LDFLAGS) -o build/producer_consumer

clean:
	rm -rf build/*