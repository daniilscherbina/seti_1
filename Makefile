all:
	g++ ./server/main.cpp -o ./bin/server
	g++ ./client/main.cpp -o ./bin/client
	g++ ./checker/main.cpp -o ./bin/checker