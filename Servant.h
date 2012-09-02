#pragma once
//Servant:20120827
#include <iostream>
#include "neuria/FuncController.h"
#include "neuria/P2pCore.h"
#include "JsonParser.h"

namespace sy
{
template<class Parser>
class Servant : public boost::enable_shared_from_this<Servant<Parser>> {
public:
	using Pointer = boost::shared_ptr<Servant>;

	static auto Create(boost::asio::io_service& service, const std::string& local_hostname, 
			int port, int buffer_size, const Parser& parser, std::ostream& os) -> Pointer {
		auto servant_ptr = Pointer(new Servant(service, local_hostname, port, parser, os));
		auto controller_ptr = Controller::Create(service,
			boost::bind(&Servant::PickupKey, servant_ptr, _1));

		controller_ptr->Register("search_key_hash",
			boost::bind(&Servant::OnReceiveSearchKeyHash, servant_ptr, _1, _2));
	
		auto core_ptr = nr::P2pCore::Create(service, port, buffer_size, 
			boost::bind(&Servant::OnAccept, servant_ptr, _1),
			boost::bind(&Controller::CallMatchFunc, controller_ptr, _1, _2),
			boost::bind(&Servant::OnCloseAccepted, servant_ptr, _1), os);

		servant_ptr->SetControllerPtr(controller_ptr);
		servant_ptr->SetCorePtr(core_ptr);
		return servant_ptr;
	}

	auto SearchKeyHash(const std::vector<std::string>& keyward_list) -> void {
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
		this->core_ptr->Connect(hostname, port, 
			boost::bind(&Servant::OnConnectToUpperNode, this->shared_from_this(), _1),
			boost::bind(&Servant::OnReceiveFromUpperNode, this->shared_from_this(), _1, _2),
			boost::bind(&Servant::OnCloseUpperNodeSession, this->shared_from_this(), _1));
	}

	auto GetCorePtr() -> nr::P2pCore::Pointer {
		return this->core_ptr;	
	}

private:
	using Controller = nr::FuncController<typename Parser::Key>;

	Servant(boost::asio::io_service& service, const std::string& local_hostname, int port, 
		const Parser& parser, std::ostream& os) 
		: service(service), local_hostname(local_hostname), port(port), 
		connected_pool(nr::SessionPool::Create()), 
		accepted_pool(nr::SessionPool::Create()), 
		parser(parser), os(os), max_hop_count(3){}

	auto OnAccept(nr::Session::Pointer session) -> void {
		this->accepted_pool->Add(session);
	}

	auto OnCloseAccepted(nr::Session::Pointer session) -> void {
		this->accepted_pool->Erase(session);
	}

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

	auto PickupKey(const utl::ByteArray& byte_array) -> typename Parser::Key {
		return this->parser.PickupKey(byte_array);
	}

	auto SetControllerPtr(typename Controller::Pointer controller_ptr) -> void {
		this->controller_ptr = controller_ptr;
	}

	auto SetCorePtr(nr::P2pCore::Pointer core_ptr) -> void {
		this->core_ptr = core_ptr;
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
	typename Controller::Pointer controller_ptr;
	nr::P2pCore::Pointer core_ptr;
	Parser parser;
	std::ostream& os;
	int max_hop_count;
};
}

