#pragma once
//Servant:20120827
#include <iostream>
#include "neuria/FuncManager.h"
#include "JsonParser.h"

namespace sy
{
template<class Parser>
class Servant /*: public boost::enable_shared_from_this<Parser> */{
public:
	using Pointer = boost::shared_ptr<Servant>;

	static auto Create(boost::asio::io_service& service, const std::string& localhost_name, 
			int port, int buffer_size, const Parser& parser, std::ostream& os) -> Pointer {
		auto servant_ptr = Pointer(new Servant(localhost_name, port, parser, os));
		
		Manager::FuncDict for_upper_func_dict{};
		
		Manager::FuncDict for_lower_func_dict{};
		for_lower_func_dict["search_key_hash"] = 
			boost::bind(&Servant::OnReceiveSearchKeyHash, servant_ptr, _1, _2, _3);
		
		auto manager_ptr = Manager::Create(
			service, port, buffer_size, boost::bind(&Servant::PickupKey, servant_ptr, _1),
			for_upper_func_dict, for_lower_func_dict, os);
		servant_ptr->SetManagerPtr(manager_ptr);
		return servant_ptr;
	}

	auto SearchKeyHash(const std::vector<std::string>& keyward_list) -> void{
		typename Parser::Command command{};
		command["command"] =  "search_key_hash";
		command["jump_count"] = 0;
		command["hash_list"] = typename Parser::Command(Json::arrayValue);
		command["src_host"] = this->localhost_name;
		command["src_port"] = this->port;
		for(const auto keyward : keyward_list){
			command["search_text_list"].append(keyward);
		}
		this->manager_ptr->GetCorePtr()->BroadcastToUpper(this->parser.Combinate(command));
		//this->core_ptr->BroadcastToUpper("{\"command\":\"search_key_hash\", }");	
	}

	auto GetCorePtr() -> nr::P2pCore::Pointer {
		return this->manager_ptr->GetCorePtr();	
	}

private:
	using Manager = nr::FuncManager<std::string>;
	auto PickupKey(const utl::ByteArray& byte_array) -> std::string {
		return (this->parser.Parse(byte_array))["command"].asString();
	}

	auto SetManagerPtr(Manager::Pointer manager_ptr) -> void {
		this->manager_ptr = manager_ptr;
	}

	auto DebugOutput(nr::P2pCore::Pointer core, nr::Session::Pointer session, 
			const utl::ByteArray& byte_array) -> void {
		this->os << "DEBUG(parsed byte_array):" << 
			this->parser.Parse(byte_array) << std::endl;
	}

	auto OnReceiveSearchKeyHash(
			nr::P2pCore::Pointer core, nr::Session::Pointer session, 
			const utl::ByteArray& byte_array) -> void {
		auto command = this->parser.Parse(byte_array);
		
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;

		if(this->max_jump_count > command["jump_count"].asInt()){
			command["jump_count"] = command["jump_count"].asInt() + 1;
			core->BroadcastToUpper(this->parser.Combinate(command));
		}
		else{
			//to do.	
		}

	}

	Servant(const std::string& localhost_name, int port, const Parser& parser, std::ostream& os) 
		: localhost_name(localhost_name), port(port), parser(parser), os(os), max_jump_count(3){}

	std::string localhost_name;
	int port;
	Manager::Pointer manager_ptr;
	Parser parser;
	std::ostream& os;
	int max_jump_count;
};
}

