#pragma once
//SynciaCore:20120926
#include <iostream>
#include "neuria/Neuria.h"
#include "neuria/test/CuiShell.h"
#include "command/Command.h"
#include "FileSystemPath.h"
#include "BehaviorDispatcher.h"
#include "UploadAction.h"
#include "LinkAction.h"
#include "LinkBehavior.h"
#include "SearchKeyHashAction.h"
#include "SearchKeyHashBehavior.h"
#include "SpreadKeyHashAction.h"
#include "SpreadKeyHashBehavior.h"
#include "RequestFileAction.h"
#include "RequestFileBehavior.h"
#include "filesystem/FileSystem.h"


namespace syncia
{

class SynciaCore : public boost::enable_shared_from_this<SynciaCore>{
public:
	using Pointer = boost::shared_ptr<SynciaCore>;

	static auto Create(
			unsigned int max_key_hash_count, 
			unsigned int spread_key_hash_max_count, 
			unsigned int max_hop_count, 
			unsigned int buffer_size,
			neuria::network::SessionPool::Pointer upper_session_pool,
			neuria::network::SessionPool::Pointer lower_session_pool,
			database::FileKeyHashDb::Pointer file_db, 
			database::FileKeyHashDb::Pointer searched_file_db, 
			const neuria::network::NodeId& node_id, 
			LinkAction::OnFailedCreateLinkFunc on_failed_create_link_func,
			RequestFileAction::OnRepliedFileFunc on_replied_file_func,
			std::ostream& os) -> Pointer {
		auto upload_action = UploadAction::Create(file_db, node_id, os);
		
		auto fetch_link_action = LinkAction::Create(
			command::GetCommandId<command::LinkForFetchKeyHashCommand>(), 
			upper_session_pool, node_id, on_failed_create_link_func, os);
		
		auto fetch_link_behavior = LinkBehavior::Create(lower_session_pool, 
			command::GetCommandId<command::LinkForFetchKeyHashCommand>(), os);
		fetch_link_behavior->SetOnReceiveLinkQueryFunc(
			[](neuria::network::Session::Pointer, const neuria::ByteArray&){
				std::cout << "hello" << std::endl;		
			});

		auto search_key_hash_action = 
			SearchKeyHashAction::Create(upper_session_pool, node_id, os);
		auto search_key_hash_behavior = SearchKeyHashBehavior::Create(
			node_id, max_key_hash_count, max_hop_count, 
			upper_session_pool, file_db, searched_file_db, os);
	
		auto spread_key_hash_action = 
			SpreadKeyHashAction::Create(lower_session_pool, node_id, os);
		auto spread_key_hash_behavior = SpreadKeyHashBehavior::Create(
			node_id, spread_key_hash_max_count, max_hop_count, 
			lower_session_pool, file_db, os);

		auto request_file_action = RequestFileAction::Create(os);
		auto request_file_behavior = RequestFileBehavior::Create(buffer_size, 
			[file_db](const database::HashId& hash_id) -> FileSystemPath {
				return file_db->Get(hash_id).GetFilePath();
			}, os);

		return Pointer(new SynciaCore(upload_action, 
			fetch_link_action, fetch_link_behavior, 
			search_key_hash_action, search_key_hash_behavior,
			spread_key_hash_action, spread_key_hash_behavior,
			request_file_action, request_file_behavior,
			file_db, node_id, 
			on_replied_file_func,
			os));
	}

	auto AddUploadDirectory(const FileSystemPath& upload_directory_path) -> void {
		auto watcher = filesystem::AddRemoveFileInDirectoryWatcher(upload_directory_path);
		filesystem::SetOnAddedFileFunc(watcher, 
			[this](const FileSystemPath& file_path){
				this->shared_from_this()->upload_action->UploadFile(file_path);
			}
		);
		filesystem::SetOnRemovedFileFunc(watcher,
			[this](const FileSystemPath& file_path){
				this->shared_from_this()->file_db->Erase(file_path);
			}
		);
		this->add_remove_watcher_list.push_back(watcher);
		this->multiple_timer->AddCallbackFuncAndStartTimer(
			1,
			[this](){
				this->add_remove_watcher_list.back().Check();
				this->add_remove_watcher_list.back().Call();
				this->add_remove_watcher_list.back().Update();
				return neuria::timer::IsContinue(true);
			}
		);
	}

	auto CreateSearchLink(const neuria::network::NodeId& target_node_id) -> void {
		this->fetch_link_action->CreateLink(target_node_id, 
			command::LinkForFetchKeyHashCommand().Serialize());	
	}

	auto SearchKeyHash(const database::KeywordList& keyword_list) -> void {
		this->search_key_hash_action->SearchKeyHash(keyword_list);
	} 
	
	auto RequestSpreadKeyHash() -> void {
		this->spread_key_hash_action->RequestSpreadKeyHash();
	} 

	auto RequestFile(const database::HashId& hash_id, 
			const neuria::network::NodeId& node_id, 
			const FileSystemPath& download_directory_path) -> void {
		this->request_file_action->RequestFile(hash_id, node_id, 
			download_directory_path, this->on_replied_file_func);
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->fetch_link_action->Bind(client);
		this->fetch_link_behavior->Bind(client);
		this->search_key_hash_behavior->Bind(client);
		this->spread_key_hash_behavior->Bind(client);
		this->request_file_action->Bind(client);
	}
	
	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		this->fetch_link_behavior->Bind(dispatcher);
		this->search_key_hash_behavior->Bind(dispatcher);
		this->spread_key_hash_behavior->Bind(dispatcher);
		this->request_file_behavior->Bind(dispatcher);
	}
	
	auto Bind(neuria::timer::MultipleTimer::Pointer multiple_timer) -> void {
		this->multiple_timer = multiple_timer;
		this->multiple_timer->AddCallbackFuncAndStartTimer(10, 
			[this](){
				//this->file_db->UpdateBirthTime();
				this->file_db->Apply(
					[this](database::FileKeyHash& key_hash){ 
						if(key_hash.GetOwnerId() == this->node_id){
							key_hash.SetBirthTimeNow();
						}
					}
				);
				return neuria::timer::IsContinue(true);
			}
		);
	}

private:
    SynciaCore(
		UploadAction::Pointer upload_action, 
		LinkAction::Pointer fetch_link_action, 
		LinkBehavior::Pointer fetch_link_behavior,
		SearchKeyHashAction::Pointer search_key_hash_action, 
		SearchKeyHashBehavior::Pointer search_key_hash_behavior, 
		SpreadKeyHashAction::Pointer spread_key_hash_action, 
		SpreadKeyHashBehavior::Pointer spread_key_hash_behavior, 
		RequestFileAction::Pointer request_file_action,
		RequestFileBehavior::Pointer request_file_behavior,
		database::FileKeyHashDb::Pointer file_db,
		const neuria::network::NodeId& node_id, 
		RequestFileAction::OnRepliedFileFunc on_replied_file_func,
		std::ostream& os) 
			:upload_action(upload_action), 
			fetch_link_action(fetch_link_action), 
			fetch_link_behavior(fetch_link_behavior), 
			search_key_hash_action(search_key_hash_action),
			search_key_hash_behavior(search_key_hash_behavior),
			spread_key_hash_action(spread_key_hash_action),
			spread_key_hash_behavior(spread_key_hash_behavior),
			request_file_action(request_file_action),
			request_file_behavior(request_file_behavior),
			file_db(file_db), node_id(node_id), 
			on_replied_file_func(on_replied_file_func), os(os){
		
	}

	neuria::timer::MultipleTimer::Pointer multiple_timer;
	std::vector<filesystem::AddRemoveFileInDirectoryWatcher> add_remove_watcher_list;

	UploadAction::Pointer upload_action;
	
	LinkAction::Pointer fetch_link_action;
	LinkBehavior::Pointer fetch_link_behavior;

	SearchKeyHashAction::Pointer search_key_hash_action;
	SearchKeyHashBehavior::Pointer search_key_hash_behavior;

	SpreadKeyHashAction::Pointer spread_key_hash_action;
	SpreadKeyHashBehavior::Pointer spread_key_hash_behavior;

	RequestFileAction::Pointer request_file_action;
	RequestFileBehavior::Pointer request_file_behavior;

	database::FileKeyHashDb::Pointer file_db;
	neuria::network::NodeId node_id;

	RequestFileAction::OnRepliedFileFunc on_replied_file_func;

	std::ostream& os;
};

}

