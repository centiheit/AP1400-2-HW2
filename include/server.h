#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <memory>
#include <random>
#include <regex> // 使用正则表达式
#include "client.h"
#include "crypto.h"

// 声明一个全局变量 pending_trxs，用于存储待处理的交易，其定义在 server.cpp 中
extern std::vector<std::string> pending_trxs;

// 前向声明 Client 类
class Client;

class Server {
public:
	Server();
	std::shared_ptr<Client> add_client(std::string id);
	std::shared_ptr<Client> get_client(std::string id) const;
	double get_wallet(std::string id) const;
	// 静态成员函数: 静态成员函数可以在不创建类的实例的情况下调用。这对于需要在类级别上执行的操作非常有用。
	// 静态成员函数不能用 const 修饰的原因在于 const 修饰符用于指示成员函数不会修改类的实例成员变量。
	// 然而，静态成员函数是属于类本身的，而不是某个具体的类实例。因此，静态成员函数没有 this 指针，也就没有实例成员变量可以修改。
	static bool parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value);
	bool add_pending_trx(std::string trx, std::string signature) const;
	size_t mine();
	// 友元函数
	friend void show_wallets(const Server &server);
private:
	std::map<std::shared_ptr<Client>, double> clients;
};

#endif //SERVER_H