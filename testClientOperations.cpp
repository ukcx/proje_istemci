#include "gtest/gtest.h"
#include "client_operations.h"

static ClientOperations clientTester;

TEST(ClientOperationsTest, InitializeSocketTest)
{
	//ClientOperations clientTester;

	ASSERT_TRUE(clientTester.initializeSocket("127.0.0.1", 194)) << "\nThe socket can not be initialized, probably winspock2 library is not working\n";
	//we should be able to initialize our socket
}

TEST(ClientOperationsTest, InitializeSocket_withInvalidIpAddressTest)
{
	//ClientOperations clientTester;

	ASSERT_TRUE(clientTester.initializeSocket("random string", 194));	//this won't give error since socket is technically still created, but we can not do anything with this socket
}

TEST(ClientOperationsTest, ConnectToServerTest_withoutSocketInitialization)
{
	ClientOperations client_in_this_scope;

	ASSERT_FALSE(client_in_this_scope.connectToServer());	//this should give error since socket is not initialized yet
}

TEST(ClientOperationsTest, ConnectToServerTest_withSocketInitialization)
{
	//ClientOperations clientTester;
	clientTester.initializeSocket("127.0.0.1", 194);

	ASSERT_TRUE(clientTester.connectToServer()) << "\nCan not connect to the server, probably because server is not running\n";
}

TEST(ClientOperationsTest, sendMesageTest)
{
	//ClientOperations clientTester;
	//clientTester.initializeSocket("127.0.0.1", 194);
	//clientTester.connectToServer();
	std::string msg = "Message to server";
	ASSERT_TRUE(clientTester.sendMessage(msg)) << "\nMessage can not be sent to the server. sendMessage did not work\n";
}

TEST(ClientOperationsTest, receiveMesageTest)
{
	//ClientOperations clientTester;
	//clientTester.initializeSocket("127.0.0.1", 194);
	//clientTester.connectToServer();
	std::string msg = "";
	ASSERT_TRUE(clientTester.receiveMessage(msg)) << "\nThe server could not give any feedback. receiveMessage did not work\n";
}
