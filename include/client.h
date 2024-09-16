#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "server.h"
#include "crypto.h"

// 前向声明 Server 类
class Server;

class Client {
public:
	Client(std::string id, const Server& server);
	std::string get_id() const;
	std::string get_publickey() const;
	double get_wallet();
	std::string sign(std::string txt) const;
	bool transfer_money(std::string receiver, double value);
	size_t generate_nonce();
private:
	// 第一个 const：Server const* 表示指针所指向的对象是常量，即你不能通过这个指针修改 Server 对象的内容。
	// 换句话说，server 指向的 Server 对象是只读的。
	// 第二个 const：const server 表示指针本身是常量，即你不能修改 server 指针的值，使其指向另一个 Server 对象。
	// 换句话说，server 指针是只读的。
	Server const* const server;
	const std::string id;
	std::string public_key;
	std::string private_key;
};

#endif //CLIENT_H