
#include <iostream>
#include <gtest/gtest.h>
#include "client.h"
#include "server.h"

void show_pending_transactions() {
	std::cout << std::string(20, '*') << std::endl;
	for(const  auto& trx : pending_trxs)
		std::cout << trx << std::endl;
	std::cout << std::string(20, '*') << std::endl;
}

int main(int argc, char **argv) {
    if (false) { // make false to run unit-tests
    // if (true) { // make false to run unit-tests
        // debug section
        Server server{};
        auto bryan{server.add_client("bryan")};
        Client const* p{bryan.get()};
        std::string signature{p->sign("mydata")};
        std::cout << crypto::verifySignature(p->get_publickey(), "mydata", signature) << std::endl;

        std::string sender{}, receiver{};
        double value;
        Server::parse_trx("sarah-clay-0.5", sender, receiver, value);
        std::cout << sender << " " << receiver << " " << value << std::endl;
    }
    else {
        ::testing::InitGoogleTest(&argc, argv);
        std::cout << "RUNNING TESTS ..." << std::endl;
        int ret{RUN_ALL_TESTS()};
        if (!ret)
            std::cout << "<<<SUCCESS>>>" << std::endl;
        else
            std::cout << "FAILED" << std::endl;
    }
    return 0;   
}