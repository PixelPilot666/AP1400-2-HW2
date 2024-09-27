
#include <iostream>
#include <vector>
#include <gtest/gtest.h>
#include "client.h"
#include "server.h"

void  show_pending_transactions()
{
	std::cout  <<  std::string(20, '*') <<  std::endl;
	for(const  auto& trx : pending_trxs)
		std::cout << trx <<  std::endl;
	std::cout  <<  std::string(20, '*') <<  std::endl;
}

int main(int argc, char **argv)
{
    if (false) // make false to run unit-tests
    {
        // debug section
        Server s {};
        pending_trxs.clear();
        std::shared_ptr<Client> bryan = s.add_client("bryan");
        std::shared_ptr<Client> clint = s.add_client("client");
        std::shared_ptr<Client> sarah = s.add_client("sarah");
        
        bryan->transfer_money("client", 0.2);
        
        std::string mempool{};
		for (const auto& trx : pending_trxs){
			mempool += trx;
        }

		show_wallets(s);
		size_t nonce{s.mine()};
		show_wallets(s);
    }
    else
    {
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