#pragma once
//LinkAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace syncia
{

class LinkAction : 
		public boost::enable_shared_from_this<LinkAction> {
public:
	using Pointer = boost::shared_ptr<LinkAction>;
	using OnFailedCreateLinkFunc = neuria::network::Client::OnFailedConnectFunc;

	static auto Create(const command::CommandId& command_id, 
			neuria::network::SessionPool::Pointer linked_session_pool, 
			const neuria::network::NodeId& node_id,
			std::ostream& os) -> Pointer {
		return Pointer(new LinkAction(
			command_id, linked_session_pool, node_id, os));
	}

	auto SetOnFailedCreateLinkFunc(
			OnFailedCreateLinkFunc on_failed_create_link_func) -> void {
		this->on_failed_create_link_func = on_failed_create_link_func;
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;
	}

	auto CreateLink(const neuria::network::NodeId& target_node_id, 
			const neuria::ByteArray& wrapped_byte_array) -> void {
		this->client->Connect(target_node_id,
			[this, wrapped_byte_array]
					(neuria::network::Session::Pointer session){
				this->pool->Add(session);	
				session->Send(command::DispatchCommand(this->command_id,
						command::LinkCommand(
							this->node_id, wrapped_byte_array
						).Serialize()
					).Serialize(), 
					neuria::network::Session::OnSendFinishedFunc([this](
							neuria::network::Session::Pointer session){
						session->StartReceive([](
								neuria::network::Session::Pointer session, 
								const neuria::ByteArray& byte_array){	
							//nothing
						});
					}),
					neuria::network::Session::OnFailedSendFunc([](
							const neuria::network::ErrorCode&){
						//nothing
					})
				);	
			},
			[this](const neuria::network::ErrorCode& error_code){
				std::cout << "link action error:" << error_code << std::endl;	
				this->on_failed_create_link_func(error_code);
			},
			[this](neuria::network::Session::Pointer session){
				this->pool->Erase(session);	
			}
		);
	}

private:
	LinkAction(const command::CommandId& command_id, 
		neuria::network::SessionPool::Pointer pool, 
		const neuria::network::NodeId& node_id, 
		std::ostream& os) 
		: command_id(command_id), pool(pool), node_id(node_id), os(os){
	
		this->SetOnFailedCreateLinkFunc(
			[this](const neuria::network::ErrorCode& error_code){
				std::cout 
					<< "create link failed. (this is default "
					<< "\"syncia::LinkAction::FailedCreateLincFunc\"):" 
					<< error_code << std::endl;
			}
		);
	}

	neuria::network::Client::Pointer client;
	command::CommandId command_id;
	neuria::network::SessionPool::Pointer pool;
	neuria::network::NodeId node_id;
	OnFailedCreateLinkFunc on_failed_create_link_func;
	std::ostream& os;
	
};

}

