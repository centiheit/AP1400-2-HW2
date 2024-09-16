#include "server.h"

const double initial_coins = 5.0, mining_reward = 6.25;

std::vector<std::string> pending_trxs;

Server::Server() {}

std::shared_ptr<Client> Server::add_client(std::string id) {
    // 标准伪随机数生成器
    std::default_random_engine e;
    // 生成[0-9]之间的均匀分布整数
    std::uniform_int_distribution<int> u(0, 9);
    // 生成4位随机数并转换为字符串
    std::string tail;
    for (size_t i = 0; i < 4; i++) {
        tail += std::to_string(u(e));
    }
    // 在clients中查找id，如果存在已有id与当前输入的id一致，则令当前id等于id+tail
    for (auto &client : clients) {
        if (client.first->get_id() == id) {
            id += tail;
        }
    }
    // 创建一个新的客户端对象
    std::shared_ptr<Client> new_client = std::make_shared<Client>(id, *this);
    // 可以直接通过索引运算符[]访问map中的元素，如果key不存在则会自动创建一个新的key
    clients[new_client] = initial_coins;
    return new_client;
}

std::shared_ptr<Client> Server::get_client(std::string id) const {
    // 遍历clients中的所有客户端对象
    for (auto &client : clients) {
        // 如果当前客户端对象的id与输入的id一致，则返回当前客户端对象
        if (client.first->get_id() == id) {
            return client.first;
        }
    }
    // 如果clients中没有找到对应的客户端对象，则返回空指针
    return nullptr;
}

double Server::get_wallet(std::string id) const {
    // 遍历clients中的所有客户端对象
    for (auto &client : clients) {
        // 如果当前客户端对象的id与输入的id一致，则返回当前客户端对象的余额
        if (client.first->get_id() == id) {
            return client.second;
        }
    }
    // 如果clients中没有找到对应的客户端对象，则返回0
    return 0.0;
}

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value) {
    // 正则表达式匹配模式
    // \\w+ 匹配任意字母数字下划线（多次），\\d+ 匹配任意数字（多次）
    std::string pattern = "(\\w+)-(\\w+)-(\\d+\\.\\d+)";
    // 创建正则表达式对象
    std::regex r(pattern);
    // 存储匹配结果
    std::smatch results;
    // 如果匹配成功
    if (std::regex_match(trx, results, r)) {
        // 获取sender
        sender = results.str(1);
        // 获取receiver
        receiver = results.str(2);
        // 获取value
        value = std::stod(results.str(3));
        // std::cout << trx << std::endl;
        // std::cout << results.str(1) << " " << results.str(2) << " " << results.str(3) << std::endl;
        // std::cout << sender << " " << receiver << " " << value << std::endl;
        return true;
    }
    // 如果匹配失败，则抛出异常
    else {
        throw std::runtime_error("trx format error");
    }
}

bool Server::add_pending_trx(std::string trx, std::string signature) const {
    // 定义sender, receiver, value
    std::string sender, receiver;
    double value;
    // 解析交易
    parse_trx(trx, sender, receiver, value);
    std::shared_ptr<Client> sender_client = get_client(sender);
    if (sender_client == nullptr) {
        return false;
    }
    // 获取发送方的公钥
    std::string public_key = sender_client->get_publickey();
    // 验证签名
    bool authentic = crypto::verifySignature(public_key, trx, signature);
    if (!authentic) {
        return false;
    }
    // 如果发送方的余额小于交易金额，则返回false
    if (sender_client->get_wallet() < value) {
        return false;
    }
    // 将交易添加到pending_trxs中
    pending_trxs.push_back(trx);
    return true;
}

// 判断字符串str的前10位中是否有连续的0
static inline bool has_consecutive_zeros(const std::string &str, int consecutive_count = 3) {
    const std::string zeros = std::string(consecutive_count, '0');
    // substr用于提取子串，表示从字符串 str 的第 0 个位置开始，提取长度为 10 的子字符串。，find用于查找子串
    // npos 是 std::string 类的一个静态常量，表示查找操作未找到子字符串时的返回值。
    return str.substr(0, 10).find(zeros) != str.npos;
} 

size_t Server::mine() {
    std::string mempool;
    size_t nonce;
    bool mined_flag = false;
    // 遍历pending_trxs中的所有交易，生成内存池
    for (auto &trx : pending_trxs) {
        mempool += trx;
    }
    // 如果没有挖矿成功，就重复遍历所有客户端对象，直到挖矿成功
    while (!mined_flag) {
        // 遍历clients中的所有客户端对象
        for (auto &client : clients) {
            // 当前客户端对象的nonce
            nonce = client.first->generate_nonce();
            // 计算内存池的sha256值
            std::string hash = crypto::sha256(mempool + std::to_string(nonce));
            // 如果hash的前10位中是否有连续的0，则表示挖矿成功
            if (has_consecutive_zeros(hash)) {
                mined_flag = true;
                client.second += mining_reward;
                std::cout << "Mining success! The miner's id is: " << client.first->get_id() << std::endl;
                // 执行pending_trxs中的所有交易
                std::string sender, receiver;
                double value;
                for (auto &trx : pending_trxs) {
                    parse_trx(trx, sender, receiver, value);
                    // 执行交易
                    clients[get_client(sender)] -= value;
                    clients[get_client(receiver)] += value;
                }
                // 清空pending_trxs
                pending_trxs.clear();
                return nonce;
            }
        }
    }
    // 如果挖矿失败，则返回0
    return 0;
}

// 通过友元函数访问私有成员变量
void show_wallets(const Server& server) {
	std::cout << std::string(20, '*') << std::endl;
	for(const auto& client : server.clients)
		std::cout << client.first->get_id() <<  " : "  << client.second << std::endl;
	std::cout << std::string(20, '*') << std::endl;
}