#pragma once
//Syncia:20120913
#include <iostream>
#include "SearchKeyHashQueryBehavior.h"
#include "SearchKeyHashAnswerBehavior.h"
#include "SearchKeyHashQueryAction.h"

namespace sy
{
class Syncia : public boost::enable_shared_from_this<Syncia> {
public:
	using Pointer = boost::shared_ptr<Syncia>;

	static auto Create(boost::asio::io_service& service, const nr::NodeId& node_id, 
			int buffer_size, int max_hop_count, std::ostream& os) -> Pointer {
		auto search_link_pool = nr::ntw::SessionPool::Create();
		return Pointer(new Syncia(
			service, node_id, buffer_size, max_hop_count, search_link_pool, os));
	}

	auto Bind(nr::ntw::BehaviorDispatcher::Pointer dispatcher) -> void {
		this->skhqb->Bind(dispatcher);
		this->skhab->Bind(dispatcher);
	}

	auto Bind(nr::ntw::Client::Pointer client) -> void { 
		this->skhab->Bind(client);
		this->skhqa->Bind(client);
	}

	auto ConnectSearchLink(const nr::NodeId& node_id) -> void {
		this->skhqa->ConnectSearchLink(node_id);	
	}

	auto QuerySearchKeyHash(const std::vector<std::string>& keyward_list) -> void {
		this->skhqa->QuerySearchKeyHash(keyward_list);	
	}

private:
    Syncia(boost::asio::io_service& service, const nr::NodeId& node_id, 
		int buffer_size, int max_hop_count, 
		nr::ntw::SessionPool::Pointer search_link_pool, std::ostream& os)
			: skhqb(SearchKeyHashQueryBehavior::Create(
				service, node_id, max_hop_count, search_link_pool, os)), 
			skhab(SearchKeyHashAnswerBehavior::Create(
				service, node_id, buffer_size, os)), 
			skhqa(SearchKeyHashQueryAction::Create(
				service, node_id, buffer_size, search_link_pool, os)), os(os){
		skhqb->SetSearchKeyHashAnswerer(
			[this](const cmd::SearchKeyHashQueryCommand& command){
				this->os << "return back!" << std::endl;
				this->skhab->AnswerSearchKeyHash(
					cmd::SearchKeyHashQueryCommand2SearchKeyHashAnswerCommand(
						command));
			}
		);
	}
	
	nr::ntw::Client::Pointer client;
	SearchKeyHashQueryBehavior::Pointer skhqb;
	SearchKeyHashAnswerBehavior::Pointer skhab;
	SearchKeyHashQueryAction::Pointer skhqa;
	std::ostream& os;
};
}

