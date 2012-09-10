#pragma once
//SearchKeyHashAbility:20120827
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
class SearchKeyHashAbility : public nr::ntw::Action, public nr::ntw::Behavior, 
		public boost::enable_shared_from_this<SearchKeyHashAbility<Parser>> {
public:
	using Pointer = boost::shared_ptr<SearchKeyHashAbility>;

	static auto Create(boost::asio::io_service& service, const nr::NodeId& node_id, 
			int buffer_size, const Parser& parser, std::ostream& os) -> Pointer {
		return Pointer(new SearchKeyHashAbility(service, node_id, parser, os));
	}

	auto ConnectSearchLink(const nr::NodeId& node_id) -> void {
		nr::ntw::Connect(this->client, node_id, this->connected_pool,
			boost::bind(&SearchKeyHashAbility::OnReceiveFromUpperNode, 
				this->shared_from_this(), _1, _2));
	}
	
private:
	auto OnReceiveFromUpperNode(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		OnReceiveSearchKeyHashQuery(session, byte_array);
	}

	auto OnReceiveSearchKeyHashAnswer(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		auto command = this->parser.Parse(byte_array);	
		
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;
		assert(!this->connected_pool->IsEmpty());

		AnswerSearchKeyHash(command);
	}

public:
	auto QuerySearchKeyHash(const std::vector<std::string>& keyward_list) -> void {	
		typename Parser::Dict command{};
		command["command"] =  "search_key_hash_query";
		command["search_keyward_list"] = typename Parser::Dict(Json::arrayValue);
		for(const auto keyward : keyward_list){
			command["search_keyward_list"].append(keyward);
		}
			typename Parser::Dict route_dict;
				typename Parser::Dict local_peer_info;
				local_peer_info["node_id"] = this->node_id;
			route_dict["route_peer"] = local_peer_info; 
			
			route_dict["searched_key_hash"] = 
						typename Parser::Dict(Json::arrayValue); 
		command["route"] = typename Parser::Dict(Json::arrayValue);
		command["route"].append(route_dict);
		this->at_random_selector(*connected_pool)->Send(this->parser.Combinate(command));
	}

private:
	SearchKeyHashAbility(boost::asio::io_service& service, 
		const nr::NodeId& node_id, const Parser& parser, std::ostream& os) 
		: service(service), node_id(node_id), 
		connected_pool(nr::ntw::SessionPool::Create()), 
		accepted_pool(nr::ntw::SessionPool::Create()), 
		parser(parser), os(os), max_hop_count(3){}
	
	auto PickupKey(const nr::ByteArray& byte_array) -> typename Parser::Key {
		return this->parser.PickupKey(byte_array);
	}

	auto DoBind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;
	}
	
	auto DoBind(nr::ntw::Server::Pointer server) -> void {
		nr::ntw::SetCallbacks(server, this->accepted_pool, 
			boost::bind(&SearchKeyHashAbility::OnReceiveSearchKeyHashQuery, 
				this->shared_from_this(), _1, _2));
	}
	
	auto OnReceiveSearchKeyHashQuery(
			nr::ntw::Session::Pointer session, const nr::ByteArray& byte_array) -> void {
		auto command = this->parser.Parse(byte_array);
		
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;

		assert(!this->connected_pool->IsEmpty());
		
		typename Parser::Dict route_dict;
			typename Parser::Dict route_peer;
			route_peer["node_id"] = this->node_id;
		route_dict["route_peer"] = route_peer; 
		
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
			if(this->node_id == command["route"][i]["route_peer"]["node_id"].asString()){
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
			nr::ntw::Send(this->client,
				command["route"][self_index-1]["route_peer"]["node_id"].asString(),
				this->parser.Combinate(command));
		}	
	}


	auto DebugOutput(const nr::ByteArray& byte_array) -> void {
		this->os << "DEBUG(parsed byte_array):" << 
			this->parser.Parse(byte_array) << std::endl;
	}

	boost::asio::io_service& service;
	nr::NodeId node_id;
	nr::ntw::SessionPool::Pointer connected_pool;
	nr::ntw::SessionPool::Pointer accepted_pool;
	Parser parser;
	nr::ntw::Client::Pointer client;
	std::ostream& os;
	unsigned int max_hop_count;
	nr::utl::RandomElementSelector at_random_selector;
};

template<class Parser>
auto CreateSearchKeyHashAbility(boost::asio::io_service& service, int port, const nr::NodeId& node_id, int buffer_size, const Parser& parser, std::ostream& os) -> typename SearchKeyHashAbility<Parser>::Pointer {
	auto client = nr::ntw::SocketClient::Create(service, buffer_size, os);
	auto server = nr::ntw::SocketServer::Create(
		service, port, buffer_size, os);

	auto peer = SearchKeyHashAbility<JsonParser>::Create(service, node_id,
		buffer_size, parser, os);
	peer->nr::ntw::Action::Bind(client);
	peer->nr::ntw::Behavior::Bind(server);

	server->StartAccept();
	return peer;	
}

template<class JsonParser>
auto TestSearchKeyHashAbilityCuiApp(
		boost::asio::io_service& service, typename SearchKeyHashAbility<JsonParser>::Pointer peer) -> void {
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));
	std::cout << "\"help\" shows commands" << std::endl;
	while(true){ //main loop
		try{
			const auto command = nr::utl::GetInput<std::string>("command?:");
			
			if(command == "search"){
				const auto keywards = nr::utl::GetInput<std::string>("keywards?:");
				std::vector<std::string> keyward_list{};
				boost::split(keyward_list, keywards, boost::is_space());
				peer->QuerySearchKeyHash(keyward_list);
			}
			else if(command == "connect"){
				const auto hostname = nr::utl::GetInput<std::string>("hostname?:");
				const auto port = nr::utl::GetInput<int>("port?:");	

				peer->ConnectSearchLink(nr::utl::CreateSocketNodeId(hostname, port));
			}	
		}
		catch(std::exception& e){
			std::cout << "error!!!:" << e.what() << std::endl;	
		}
	}
	t.join();	
	
}

auto ManySearchKeyHashAbility(int num) -> void {
	auto peer_list = std::vector<typename SearchKeyHashAbility<JsonParser>::Pointer>{};
	int base_port = 54321;
	auto stream_list = std::vector<boost::shared_ptr<nr::utl::LabeledSink::Stream>>{};
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	for(int i = 0; i < num; i++)
	{
		stream_list.push_back(
			boost::shared_ptr<nr::utl::LabeledSink::Stream>(
				new nr::utl::LabeledSink::Stream(
					nr::utl::LabeledSink("peer"+boost::lexical_cast<std::string>(i)))));
		peer_list.push_back(CreateSearchKeyHashAbility(
			service, base_port+i, nr::utl::CreateSocketNodeId("127.0.0.1", base_port+i), 128, 
			JsonParser("command"), *stream_list.back()));
	}
	boost::thread t1(boost::bind(&boost::asio::io_service::run, &service));
	boost::thread t2(boost::bind(&boost::asio::io_service::run, &service));

	for(int i = 0; i < 3; i++)
	{
		for(const auto& peer : peer_list){
			peer->ConnectSearchLink(nr::utl::CreateSocketNodeId("localhost", base_port+nr::utl::Random(0, num-1)));
		}
	}
	TestSearchKeyHashAbilityCuiApp<JsonParser>(service, peer_list.front());

	while(true){}

	t1.join();	
	t2.join();	


}

}

