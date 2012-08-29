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

	static auto Create(boost::asio::io_service& service, int port, int buffer_size, 
			const Parser& parser, std::ostream& os) -> Pointer {
		auto servant_ptr = Pointer(new Servant(parser, os));
		
		Manager::FuncDict for_upper_func_dict{};
		
		Manager::FuncDict for_lower_func_dict{};
		for_lower_func_dict["search_key_hash"] = 
			boost::bind(&Servant::DebugOutput, servant_ptr, _1, _2, _3);
		
		auto manager_ptr = Manager::Create(
			service, port, buffer_size, boost::bind(&Servant::PickupKey, servant_ptr, _1),
			for_upper_func_dict, for_lower_func_dict, os);
		servant_ptr->SetManagerPtr(manager_ptr);
		return servant_ptr;
	}

	auto SearchKeyHash(const std::vector<std::string>& keyward_list) -> void{
		typename Parser::Command command{};
		command["command"] =  "search_key_hash";
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

	Servant(const Parser& parser, std::ostream& os) 
		: parser(parser), os(os){}

	Manager::Pointer manager_ptr;
	Parser parser;
	std::ostream& os;
};
}

