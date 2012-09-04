#pragma once
//Servant:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include "neuria/Neuria.h"
#include "JsonParser.h"
#include <jsoncpp/value.h>
#include <boost/algorithm/string.hpp>

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

		controller_ptr->Register("search_key_hash_query",
			boost::bind(&Servant::OnReceiveSearchKeyHashQuery, servant_ptr, _1));
	
		controller_ptr->Register("search_key_hash_answer",
			boost::bind(&Servant::OnReceiveSearchKeyHashAnswer, servant_ptr, _1));
		
		auto core_ptr = nr::ntw::P2pCore::Create(service, port, buffer_size, 
			[servant_ptr](nr::ntw::Session::Pointer session){ servant_ptr->accepted_pool->Add(session); },
			boost::bind(&Controller::CallMatchFunc, controller_ptr, _1),
			[servant_ptr](nr::ntw::Session::Pointer session){ servant_ptr->accepted_pool->Erase(session); }, os);

		servant_ptr->SetControllerPtr(controller_ptr);
		servant_ptr->SetCorePtr(core_ptr);
		return servant_ptr;
	}

	auto GetCorePtr() -> nr::ntw::P2pCore::Pointer {
		return this->core_ptr;	
	}
	
	auto ConnectToUpperNode(const std::string& hostname, int port) -> void
	{
		this->core_ptr->Connect(hostname, port, 
			boost::bind(&Servant::OnConnectToUpperNode, this->shared_from_this(), _1),
			boost::bind(&Servant::OnReceiveFromUpperNode, this->shared_from_this(), _1),
			boost::bind(&Servant::OnCloseUpperNodeSession, this->shared_from_this(), _1));
	}

	auto QuerySearchKeyHash(const std::vector<std::string>& keyward_list) -> void {	
		typename Parser::Dict command{};
		command["command"] =  "search_key_hash_query";
		command["search_keyward_list"] = typename Parser::Dict(Json::arrayValue);
		for(const auto keyward : keyward_list){
			command["search_keyward_list"].append(keyward);
		}
			typename Parser::Dict route_dict;
				typename Parser::Dict local_servant_info;
				local_servant_info["address"] = this->local_hostname;
				local_servant_info["port"] = this->port;
			route_dict["route_servant"] = local_servant_info; 
			
			route_dict["searched_key_hash"] = 
						typename Parser::Dict(Json::arrayValue); 
		command["route"] = typename Parser::Dict(Json::arrayValue);
		command["route"].append(route_dict);
		this->at_random_selector(*connected_pool)->Send(this->parser.Combinate(command));
	}

private:
	using Controller = nr::utl::MatchFuncCaller<typename Parser::Key>;

	Servant(boost::asio::io_service& service, const std::string& local_hostname, int port, 
		const Parser& parser, std::ostream& os) 
		: service(service), local_hostname(local_hostname), port(port), 
		connected_pool(nr::ntw::SessionPool::Create()), 
		accepted_pool(nr::ntw::SessionPool::Create()), 
		parser(parser), os(os), max_hop_count(6){}

	auto OnConnectToUpperNode(nr::ntw::Session::Pointer session) -> void {
		this->connected_pool->Add(session);
	}

	auto OnReceiveFromUpperNode(const nr::ByteArray& byte_array) -> void {
		OnReceiveSearchKeyHashQuery(byte_array);
	}

	auto OnCloseUpperNodeSession(nr::ntw::Session::Pointer session) -> void {
		this->connected_pool->Erase(session);
	}

	auto PickupKey(const nr::ByteArray& byte_array) -> typename Parser::Key {
		return this->parser.PickupKey(byte_array);
	}

	auto SetControllerPtr(typename Controller::Pointer controller_ptr) -> void {
		this->controller_ptr = controller_ptr;
	}

	auto SetCorePtr(nr::ntw::P2pCore::Pointer core_ptr) -> void {
		this->core_ptr = core_ptr;
	}

	auto DebugOutput(const nr::ByteArray& byte_array) -> void {
		this->os << "DEBUG(parsed byte_array):" << 
			this->parser.Parse(byte_array) << std::endl;
	}

	auto OnReceiveSearchKeyHashQuery(const nr::ByteArray& byte_array) -> void {
		auto command = this->parser.Parse(byte_array);
		
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;

		assert(!this->connected_pool->IsEmpty());
		
		typename Parser::Dict route_dict;
			typename Parser::Dict route_servant;
			route_servant["address"] = this->local_hostname;
			route_servant["port"] = this->port;
		route_dict["route_servant"] = route_servant; 
		
		route_dict["searched_key_hash"] = 
			typename Parser::Dict(Json::arrayValue); 
		command["route"].append(route_dict);
		
		if(command["route"].size() <= this->max_hop_count){
			this->at_random_selector(
				*this->connected_pool)->Send(this->parser.Combinate(command));
		}
		else{
			AnswerSearchKeyHash(command);
		}
	}
	
	auto AnswerSearchKeyHash(typename Parser::Dict command) -> void {
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;
		if(command["command"] != "search_key_hash_answer"){
			this->os << "return back query as answer." << std::endl;
			command["command"] = "search_key_hash_answer";
		}
		
		unsigned int self_index = 0;
		for(unsigned int i = 0; i < command["route"].size(); i++){
			if(this->local_hostname 
					== command["route"][i]["route_servant"]["address"].asString()
					&& this->port 
					== command["route"][i]["route_servant"]["port"].asInt()){
				self_index = i;
				break;
			}
		}
		
		if(self_index == 0){
			this->os << "query was answered!" << std::endl;	
			this->os << command["route"] << std::endl;
		}
		else{
			this->os << "send answer!" << std::endl;
			nr::ntw::Send(this->core_ptr,
				command["route"][self_index-1]["route_servant"]["address"].asString(), 
				command["route"][self_index-1]["route_servant"]["port"].asInt(),
				this->parser.Combinate(command));
		}	
	}

	auto OnReceiveSearchKeyHashAnswer(const nr::ByteArray& byte_array) -> void {
		auto command = this->parser.Parse(byte_array);	
		
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;
		assert(!this->connected_pool->IsEmpty());

		AnswerSearchKeyHash(command);
	}


	boost::asio::io_service& service;
	std::string local_hostname;
	int port;
	nr::ntw::SessionPool::Pointer connected_pool;
	nr::ntw::SessionPool::Pointer accepted_pool;
	typename Controller::Pointer controller_ptr;
	nr::ntw::P2pCore::Pointer core_ptr;
	Parser parser;
	std::ostream& os;
	unsigned int max_hop_count;
	nr::utl::RandomElementSelector at_random_selector;
};

auto TestServantCuiApp(
		boost::asio::io_service& service, Servant<JsonParser>::Pointer servant_ptr) -> void {
	auto core_ptr = servant_ptr->GetCorePtr();
	//boost::thread t(boost::bind(&boost::asio::io_service::run, &service));
	std::cout << "\"help\" shows commands" << std::endl;
	while(true){ //main loop
		try{
			const auto command = nr::utl::GetInput<std::string>("command?:");
			
			if(command == "search"){
				const auto keywards = nr::utl::GetInput<std::string>("keywards?:");
				std::vector<std::string> keyward_list{};
				boost::split(keyward_list, keywards, boost::is_space());
				servant_ptr->QuerySearchKeyHash(keyward_list);
			}
			else if(command == "connect"){
				const auto hostname = nr::utl::GetInput<std::string>("hostname?:");
				const auto port = nr::utl::GetInput<int>("port?:");	

				servant_ptr->ConnectToUpperNode(hostname, port);
			}	
		}
		catch(std::exception& e){
			std::cout << "error!!!:" << e.what() << std::endl;	
		}
	}
	//t.join();	
	
}

auto ManyServant(int num) -> void {
	auto servant_list = std::vector<Servant<JsonParser>::Pointer>{};
	int base_port = 54321;
	auto stream_list = std::vector<boost::shared_ptr<nr::utl::LabeledSink::Stream>>{};
	boost::asio::io_service service;
	for(int i = 0; i < num; i++)
	{
		stream_list.push_back(
			boost::shared_ptr<nr::utl::LabeledSink::Stream>(
				new nr::utl::LabeledSink::Stream(
					nr::utl::LabeledSink("servant"+boost::lexical_cast<std::string>(i)))));
		servant_list.push_back(Servant<JsonParser>::Create(
			service, "127.0.0.1", base_port+i, 128, 
			JsonParser("command"), *stream_list.back()));
	}
	boost::thread t1(boost::bind(&boost::asio::io_service::run, &service));
	boost::thread t2(boost::bind(&boost::asio::io_service::run, &service));

	for(int i = 0; i < 3; i++)
	{
		for(const auto& servant : servant_list){
			servant->ConnectToUpperNode("localhost", base_port+nr::utl::Random(0, num-1));
		}
	}
	TestServantCuiApp(service, servant_list.front());

	while(true){}

	t1.join();	
	t2.join();	


}

}

