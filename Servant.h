#pragma once
//Servant:20120827
#include <iostream>
#include "neuria/AcceptedManager.h"
#include "JsonParser.h"

namespace sy
{
template<class Parser>
class Servant : public boost::enable_shared_from_this<Servant<Parser>> {
public:
	using Pointer = boost::shared_ptr<Servant>;

	static auto Create(boost::asio::io_service& service, const std::string& local_hostname, 
			int port, int buffer_size, const Parser& parser, std::ostream& os) -> Pointer {
		auto accepted_pool = nr::SessionPool::Create();
		auto servant_ptr = Pointer(new Servant(service, local_hostname, port, 
			accepted_pool, parser, os));
		auto manager_ptr = Manager::Create(service, port, buffer_size, accepted_pool,
			boost::bind(&Servant::PickupKey, servant_ptr, _1), os);

		manager_ptr->Register("search_key_hash",
			boost::bind(&Servant::OnReceiveSearchKeyHash, servant_ptr, _1, _2));
		
		servant_ptr->SetManagerPtr(manager_ptr);
		return servant_ptr;
	}

	auto SearchKeyHash(const std::vector<std::string>& keyward_list) -> void{
		typename Parser::Command command{};
		command["command"] =  "search_key_hash";
		command["hop_count"] = 0;
		command["hash_list"] = typename Parser::Command(Json::arrayValue);
		command["src_host"] = this->local_hostname;
		command["src_port"] = this->port;
		for(const auto keyward : keyward_list){
			command["search_text_list"].append(keyward);
		}
		nr::Broadcast(this->service, this->connected_pool, this->parser.Combinate(command));
	}

	auto ConnectToUpperNode(const std::string& hostname, int port) -> void
	{
		this->manager_ptr->GetCorePtr()->Connect(hostname, port, 
			boost::bind(&Servant::OnConnectToUpperNode, this->shared_from_this(), _1),
			boost::bind(&Servant::OnReceiveFromUpperNode, this->shared_from_this(), _1, _2),
			boost::bind(&Servant::OnCloseUpperNodeSession, this->shared_from_this(), _1));
	}

	auto GetCorePtr() -> nr::P2pCore::Pointer {
		return this->manager_ptr->GetCorePtr();	
	}

private:
	Servant(boost::asio::io_service& service, const std::string& local_hostname, int port, 
		nr::SessionPool::Pointer accepted_pool, const Parser& parser, std::ostream& os) 
		: service(service), local_hostname(local_hostname), port(port), 
		connected_pool(nr::SessionPool::Create()), accepted_pool(accepted_pool), 
		parser(parser), os(os), max_hop_count(3){}

	auto OnConnectToUpperNode(nr::Session::Pointer session) -> void {
		this->connected_pool->Add(session);
	}

	auto OnReceiveFromUpperNode(
			nr::Session::Pointer session, const utl::ByteArray& byte_array) -> void {
		OnReceiveSearchKeyHash(session, byte_array);
	}

	auto OnCloseUpperNodeSession(nr::Session::Pointer session) -> void {
		this->connected_pool->Erase(session);
	}

	using Manager = nr::AcceptedManager<std::string>;
	auto PickupKey(const utl::ByteArray& byte_array) -> std::string {
		return (this->parser.Parse(byte_array))["command"].asString();
	}

	auto SetManagerPtr(Manager::Pointer manager_ptr) -> void {
		this->manager_ptr = manager_ptr;
	}

	auto DebugOutput(
			nr::Session::Pointer session, const utl::ByteArray& byte_array) -> void {
		this->os << "DEBUG(parsed byte_array):" << 
			this->parser.Parse(byte_array) << std::endl;
	}

	auto OnReceiveSearchKeyHash(
			nr::Session::Pointer session, const utl::ByteArray& byte_array) -> void {
		auto command = this->parser.Parse(byte_array);
		
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;

		if(this->max_hop_count > command["hop_count"].asInt()){
			command["hop_count"] = command["hop_count"].asInt() + 1;
			nr::Broadcast(this->service, this->connected_pool, this->parser.Combinate(command));
		}
		else{
			//to do
			//core->Connect(command["src_host"], command["src_port"]);
			
		}

	}

	boost::asio::io_service& service;
	std::string local_hostname;
	int port;
	nr::SessionPool::Pointer connected_pool;
	nr::SessionPool::Pointer accepted_pool;
	Manager::Pointer manager_ptr;
	Parser parser;
	std::ostream& os;
	int max_hop_count;
};
}

