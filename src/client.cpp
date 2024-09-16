#include "client.h"

Client::Client(std::string id, const Server& server): id(id), server(&server) {
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const {
    // 使用类中的成员变量时也可以不需要this指针
    // return this->id;
    return id;
}

std::string Client::get_publickey() const {
    return public_key;
}

double Client::get_wallet() {
    // 这里直接调用get_wallet函数不行，因为在当前类里server指向的是const对象，
    // 而get_wallet函数不是const函数，所以需要把server类中的get_wallet函数改为const函数
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const {
    return crypto::signMessage(private_key, txt);
}

bool Client::transfer_money(std::string receiver, double value) {
    // 如果余额不足，则返回false
    if (this->get_wallet() < value) {
        return false;
    }
    // 如果receiver不在server的clients中，则返回false
    if (server->get_client(receiver) == nullptr) {
        return false;
    }
    std::string trx = this->id + "-" + receiver + "-" + std::to_string(value);
    std::string signature = this->sign(trx);
    return this->server->add_pending_trx(trx, signature);
}

size_t Client::generate_nonce() {
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> u(0, INT_MAX);
    return u(e);
}