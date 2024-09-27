#include "client.h"
#include "server.h"
#include "crypto.h"

#include <ctime>
#include <random>

Client::Client(std::string id, const Server& server) : id(id), server(&server) {
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const{
    return id;
}

std::string Client::get_publickey() const{
    return public_key;
}

double Client::get_wallet() const{
    return server->get_wallet(get_id());
}

std::string Client::sign(std::string txt) const{
    return crypto::signMessage(private_key, txt);
}

bool Client::transfer_money(std::string receiver, double value){
    
    if(server->get_client(receiver) == nullptr){
        return false;
    }
    
    std::string trx = id + "-" + receiver + "-" + std::to_string(value);
    std::string signMessage = sign(trx);

    return server->add_pending_trx(trx, signMessage);
}

size_t Client::generate_nonce(){
    // 使用std::random_device来获取一个非确定性的随机数作为种子
    static std::random_device rd;
    static std::mt19937 generator(rd());
    // 定义一个均匀分布的对象，范围从10到99
    std::uniform_int_distribution<int> distribution(1, 1000);
    // 生成并返回一个介于10到99之间的随机整数
    return distribution(generator);
}