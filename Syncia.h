#pragma once
//Syncia:20120926
#include <iostream>
#include "neuria/Neuria.h"
#include "neuria/utility/Shell.h"
#include "command/Command.h"
#include "BehaviorDispatcher.h"
#include "UploadAction.h"
#include "LinkAction.h"
#include "LinkBehavior.h"
#include "SearchKeyHashAction.h"
#include "SearchKeyHashBehavior.h"
#include "SpreadKeyHashAction.h"
#include "SpreadKeyHashBehavior.h"

namespace sy
{

class Syncia{
public:
	using Pointer = boost::shared_ptr<Syncia>;

	static auto Create(
			unsigned int max_key_hash_count, 
			unsigned int spread_key_hash_max_count, 
			unsigned int max_hop_count, 
			nr::ntw::SessionPool::Pointer upper_session_pool,
			nr::ntw::SessionPool::Pointer lower_session_pool,
			nr::db::FileKeyHashDb::Pointer file_db, 
			const nr::NodeId& node_id, std::ostream& os) -> Pointer {
		auto upload_action = UploadAction::Create(file_db, node_id, os);
		
		auto search_link_action = LinkAction::Create(
			cmd::GetCommandId<cmd::SearchKeyHashLinkCommand>(), upper_session_pool, 
			node_id, os);
		
		auto search_link_behavior = LinkBehavior::Create(lower_session_pool, 
			cmd::GetCommandId<cmd::SearchKeyHashLinkCommand>(), os);
		search_link_behavior->SetOnReceiveLinkQueryFunc(
			[](nr::ntw::Session::Pointer, const nr::ByteArray&){});

		auto search_key_hash_action = 
			SearchKeyHashAction::Create(upper_session_pool, node_id, os);
		auto search_key_hash_behavior = SearchKeyHashBehavior::Create(
			node_id, max_key_hash_count, max_hop_count, 
			upper_session_pool, file_db, os);
	
		auto spread_key_hash_action = 
			SpreadKeyHashAction::Create(lower_session_pool, node_id, os);
		auto spread_key_hash_behavior = SpreadKeyHashBehavior::Create(
			node_id, spread_key_hash_max_count, max_hop_count, 
			lower_session_pool, file_db, os);

		return Pointer(new Syncia(upload_action, 
			search_link_action, search_link_behavior, 
			search_key_hash_action, search_key_hash_behavior,
			spread_key_hash_action, spread_key_hash_behavior,
			node_id, os));
	}

	auto UploadDirectory(const boost::filesystem::path& upload_directory_path) -> void {
		this->upload_action->UploadDirectory(upload_directory_path);	
	}

	auto CreateSearchLink(const nr::NodeId& target_node_id) -> void {
		this->search_link_action->CreateLink(target_node_id, 
			cmd::SearchKeyHashLinkCommand().Serialize());	
	}

	auto SearchKeyHash(const nr::db::KeywardList& keyward_list) -> void {
		this->search_key_hash_action->SearchKeyHash(keyward_list);
	} 
	
	auto RequestSpreadKeyHash() -> void {
		this->spread_key_hash_action->RequestSpreadKeyHash();
	} 

	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->search_link_action->Bind(client);
		this->search_link_behavior->Bind(client);
	}
	
	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		this->search_link_behavior->Bind(dispatcher);
		this->search_key_hash_behavior->Bind(dispatcher);
		this->spread_key_hash_behavior->Bind(dispatcher);
	}

private:
    Syncia(
		UploadAction::Pointer upload_action, 
		LinkAction::Pointer search_link_action, 
		LinkBehavior::Pointer search_link_behavior,
		SearchKeyHashAction::Pointer search_key_hash_action, 
		SearchKeyHashBehavior::Pointer search_key_hash_behavior, 
		SpreadKeyHashAction::Pointer spread_key_hash_action, 
		SpreadKeyHashBehavior::Pointer spread_key_hash_behavior, 
		const nr::NodeId& node_id, std::ostream& os) 
			:upload_action(upload_action), 
			search_link_action(search_link_action), 
			search_link_behavior(search_link_behavior), 
			search_key_hash_action(search_key_hash_action),
			search_key_hash_behavior(search_key_hash_behavior),
			spread_key_hash_action(spread_key_hash_action),
			spread_key_hash_behavior(spread_key_hash_behavior),
			node_id(node_id), os(os){}
	
	UploadAction::Pointer upload_action;
	
	LinkAction::Pointer search_link_action;
	LinkBehavior::Pointer search_link_behavior;

	SearchKeyHashAction::Pointer search_key_hash_action;
	SearchKeyHashBehavior::Pointer search_key_hash_behavior;

	SpreadKeyHashAction::Pointer spread_key_hash_action;
	SpreadKeyHashBehavior::Pointer spread_key_hash_behavior;
	
	nr::NodeId node_id;
	std::ostream& os;
};

}

