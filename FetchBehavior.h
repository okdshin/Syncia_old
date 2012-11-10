#pragma once
//FetchBehavior:20120827
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "BehaviorDispatcher.h"

namespace syncia
{

class FetchBehavior : 
		public boost::enable_shared_from_this<FetchBehavior> {
public:
	using Pointer = boost::shared_ptr<FetchBehavior>;

	using IsTurningPointDecider = boost::function<
		command::IsAnswer (const command::FetchCommand::Route&, const neuria::ByteArray&)>;
	using FetchQueryRedirector = boost::function<
		neuria::ByteArray (const neuria::ByteArray&)>;
	using FetchAnswerRedirector = boost::function<
		neuria::ByteArray (const neuria::ByteArray&)>;	
	using OnReceivedFetchAnswerFunc = boost::function<
		void (neuria::network::Session::Pointer, const neuria::ByteArray&)>;

	static auto Create(
			const command::CommandId& command_id, 
			const neuria::network::NodeId& node_id, 
			neuria::network::SessionPool::Pointer to_session_pool,
			std::ostream& os) -> Pointer {
		return Pointer(
			new FetchBehavior(
				command_id, node_id, to_session_pool, os));
	}
	
	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(this->command_id,
			boost::bind(&FetchBehavior::OnReceivedFetchCommand, 
				this->shared_from_this(), _1, _2));
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;
	}

	auto SetIsTurningPointDecider(IsTurningPointDecider func) -> void {
		this->is_turning_point_decider = func;	
	}

	auto SetFetchQueryRedirector(FetchQueryRedirector func) -> void {
		this->fetch_query_redirector = func;
	}

	auto SetFetchAnswerRedirector(FetchAnswerRedirector func) -> void {
		this->fetch_answer_redirector = func;
	}
	
	auto SetOnReceivedFetchAnswerFunc(OnReceivedFetchAnswerFunc func) -> void {
		this->on_receive_fetch_answer_func = func;	
	}

private:
	FetchBehavior(const command::CommandId& command_id, const neuria::network::NodeId& node_id, 
		neuria::network::SessionPool::Pointer to_session_pool, std::ostream& os) 
		: command_id(command_id()), node_id(node_id), 
		to_session_pool(to_session_pool), os(os){}

	auto OnReceivedFetchCommand(neuria::network::Session::Pointer session, 
			const neuria::ByteArray& byte_array) -> void {
		auto fetch_command = command::FetchCommand::Parse(byte_array);
		this->os << fetch_command << std::endl;

		if(fetch_command.IsAnswer()){
			if(fetch_command.IsReturnBackToStart(this->node_id)){
				this->Get(session, fetch_command);	
			}
			else {
				this->ReturnBack(fetch_command);
			}
		}
		else {
			fetch_command.AddRoute(this->node_id);
			if(this->to_session_pool->IsEmpty()){
				if(!fetch_command.IsReturnBackToStart(this->node_id)){
					this->os << 
						"...but no link. so immediately return back." << std::endl;
					auto new_fetch_command = command::FetchCommand(
						command::IsAnswer(false), fetch_command.GetRoute(), 
						this->fetch_query_redirector(
							fetch_command.GetWrappedByteArray())
					);
					this->ReturnBack(new_fetch_command);	
				}
				else {
					this->Assert();
				}	
			}
			else {
				if(this->is_turning_point_decider(fetch_command.GetRoute(), 
						fetch_command.GetWrappedByteArray())()){
					this->os << "here is turning point" << std::endl;
					this->ReturnBack(fetch_command);
				}
				else {
					this->Query(fetch_command);	
				}
			}
		}
	}
	
	auto ReturnBack(const command::FetchCommand& fetch_command) -> void {
		this->os << "ReturnBack" << std::endl;
		assert(this->client != nullptr);
		auto new_fetch_command = command::FetchCommand(
			command::IsAnswer(true), fetch_command.GetRoute(), 
			this->fetch_answer_redirector(fetch_command.GetWrappedByteArray()));
		this->os << "new fetch command: " << new_fetch_command << std::endl;
		neuria::network::Send(this->client,
			fetch_command.GetOneStepCloserNodeId(this->node_id),	
			command::DispatchCommand(
				this->command_id, new_fetch_command.Serialize()).Serialize(),
			[this](const neuria::network::ErrorCode& error_code){
				this->os << "failed connect to answer. : " 
					<< error_code << std::endl;	
			}
		);
	}

	auto Get(neuria::network::Session::Pointer session, 
			const command::FetchCommand& fetch_command) -> void {
		this->os << "Get" << std::endl;
		this->on_receive_fetch_answer_func(session,
			fetch_command.GetWrappedByteArray());
	}

	auto Query(const command::FetchCommand& fetch_command) -> void {
		this->os << "Query" << std::endl;
		auto redirect_byte_array = this->fetch_query_redirector(
			fetch_command.GetWrappedByteArray());
		std::cout << "to session pool:" << this->to_session_pool << std::endl;
		assert(!this->to_session_pool->IsEmpty());
		this->at_random_selector(*(this->to_session_pool))->Send(
			command::DispatchCommand(
				this->command_id,
				command::FetchCommand(
					command::IsAnswer(false), fetch_command.GetRoute(), 
					redirect_byte_array
				).Serialize()
			).Serialize(),
			[](neuria::network::Session::Pointer){}
		);
	}

	auto Assert() -> void {
		assert(!"invalid state.");	
	}


	IsTurningPointDecider is_turning_point_decider;
	FetchQueryRedirector fetch_query_redirector;
	FetchAnswerRedirector fetch_answer_redirector;
	OnReceivedFetchAnswerFunc on_receive_fetch_answer_func;
	neuria::utility::RandomElementSelector at_random_selector;
	neuria::network::Client::Pointer client;
	
	command::CommandId::WrappedType command_id;
	neuria::network::NodeId node_id;
	neuria::network::SessionPool::Pointer to_session_pool;
	std::ostream& os;
};

}

