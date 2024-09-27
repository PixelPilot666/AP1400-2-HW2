#include "server.h"
#include "client.h"
#include "crypto.h"

#include <map>
#include <regex>
#include <ctime>
#include <random>

std::vector<std::string> pending_trxs;
Server::Server()
{
    clients = std::map<std::shared_ptr<Client>, double>{};
}

std::shared_ptr<Client> Server::get_client(std::string id) const{
    for(std::pair<std::shared_ptr<Client>, double> client : clients){
        if((*(client.first)).get_id() == id){
            return client.first;
        }
    }
    return nullptr;
}

size_t random_num(){
    // 使用std::random_device来获取一个非确定性的随机数作为种子
    static std::random_device rd;
    static std::mt19937 generator(rd());
    // 定义一个均匀分布的对象，范围从10到99
    std::uniform_int_distribution<int> distribution(1000, 9999);
    // 生成并返回一个介于10到99之间的随机整数
    return distribution(generator);
}

std::shared_ptr<Client> Server::add_client(std::string id){
    
    if(get_client(id) != nullptr){
        size_t random = random_num();
        id += std::to_string(random);
    }

    std::shared_ptr<Client> cli = std::make_shared<Client>(id, *this);
    clients[cli] = 5.0;
    return cli;
}

double Server::get_wallet(std::string id) const{
    for (std::pair<std::shared_ptr<Client>, double> item : clients){
        if(item.first->get_id() == id){
            return item.second;
        }
    }
    return 0.0;
}

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value){
    std::regex pattern ("(\\w+)-(\\w+)-(\\d+(\\.\\d+)?)");
    std::smatch match;
    if(std::regex_match(trx, match, pattern)){
        sender = match[1].str();
        receiver = match[2].str();
        value = std::stod(match[3].str());
        return true;
    }
    else{
        throw std::runtime_error("Invalid transation format.");
    }
    return false;
}

bool Server::add_pending_trx(std::string trx, std::string signature) const{
    std::string sender, receiver;
    double value;

    try{
        parse_trx(trx, sender, receiver, value);
    }
    catch(const std::runtime_error& e){
        std::cerr << "parse_trx runtime_error." << std::endl;
        return false;
    }

    std::shared_ptr<Client> sClient = get_client(sender);
    std::shared_ptr<Client> rClient = get_client(receiver);
    if(sClient == nullptr || rClient == nullptr){
        return false;
    }

    std::string publicKey = sClient->get_publickey();
    bool vaild = crypto::verifySignature(publicKey, trx, signature);
    if(vaild && clients.at(sClient) >= value){
        pending_trxs.push_back(trx);
        return true;
    }

    return false;
}

size_t Server::mine(){
    std::string mempool = {};
    for(std::string item : pending_trxs){
        mempool += item;
    }
    size_t nonce = 0;
    std::string sha256 = {};
    bool flag = false;
    std::string sender, receiver;
    double value = 0.;
    while(!flag){
        for(auto& c_w : clients){
            nonce = c_w.first->generate_nonce();
            sha256 = crypto::sha256(mempool + std::to_string(nonce));
            if(sha256.substr(0,10).find("000") != std::string::npos){
                flag = true;
                c_w.second += 6.25;
                for(std::string trx : pending_trxs){
                    parse_trx(trx, sender, receiver, value);
                    std::shared_ptr<Client> sClient = get_client(sender);
                    std::shared_ptr<Client> rClient = get_client(receiver);
                    if(sClient != nullptr && rClient != nullptr){
                        clients.at(sClient) -= value;
                        clients.at(rClient) += value;
                    }
                }
                pending_trxs.clear();
                return nonce;
            }
        }
    }
    return 0;
}

void show_wallets(const  Server& server){
	std::cout << std::string(20, '*') << std::endl;
	for(const auto& client: server.clients)
		std::cout << client.first->get_id() <<  " : "  << client.second << std::endl;
	std::cout << std::string(20, '*') << std::endl;
}