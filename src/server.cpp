#include "server.h"
#include "client.h"

#include <map>
#include <ctime>
#include <random>


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
    unsigned int seed = static_cast<unsigned int>(std::time(0));
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(1000, 9999);
    int random_number = distribution(generator);
    return random_number;
}

std::shared_ptr<Client> Server::add_client(std::string id){
    if(get_client(id) != nullptr){
        id += 
    }
}