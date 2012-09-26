#pragma once
//FetchBehavior:20120827
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "BehaviorDispatcher.h"

namespace sy
{

class FetchBehavior : 
		public boost::enable_shared_from_this<FetchBehavior> {
public:
	using Pointer = boost::shared_ptr<FetchBehavior>;

	using IsTurningPointDecider = boost::function<
		cmd::IsAnswer (const cmd::FetchCommand::Route&, const nr::ByteArray&)>;
	using FetchQueryRedirector = boost::function<
		nr::ByteArray (const nr::ByteArray&)>;
	using FetchAnswerRedirector = boost::function<
		nr::ByteArray (const nr::ByteArray&)>;	
	using OnReceiveFetchAnswerFunc = boost::function<
		void (nr::ntw::Session::Pointer, const nr::ByteArray&)>;

	static auto Create(
			const cmd::CommandId& command_id, 
			const nr::NodeId& node_id, 
			nr::ntw::SessionPool::Pointer to_session_pool,
			std::ostream& os) -> Pointer {
		return Pointer(
			new FetchBehavior(
				command_id, node_id, to_session_pool, os));
	}
	
	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(this->command_id,
			boost::bind(&FetchBehavior::OnReceiveFetchCommand, 
				this->shared_from_this(), _1, _2));
	}

	auto Bind(nr::ntw::Client::Pointer client) -> void {
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
	
	auto SetOnReceiveFetchAnswerFunc(OnReceiveFetchAnswerFunc func) -> void {
		this->on_receive_fetch_answer_func = func;	
	}

private:
	FetchBehavior(const cmd::CommandId& command_id, const nr::NodeId& node_id, 
		nr::ntw::SessionPool::Pointer to_session_pool, std::ostream& os) 
		: command_id(command_id()), node_id(node_id), 
		to_session_pool(to_session_pool), os(os){}


	auto OnReceiveFetchCommand(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		auto fetch_command = cmd::FetchCommand::Parse(byte_array);
		fetch_command.AddRoute(this->node_id);
		if(fetch_command.IsReturnBackToStart(this->node_id)){ //When Finished
			this->os << "on receive finished answer." << std::endl;
			this->on_receive_fetch_answer_func(session,
				fetch_command.GetWrappedByteArray());
		}
		else{ 
			if(fetch_command.IsAnswer()){ //When Answer
				this->os << "on receive fetch answer." << std::endl;
				std::cout << fetch_command << std::endl;
				nr::ntw::Send(this->client,
					fetch_command.GetOneStepCloserNodeId(this->node_id),	
					cmd::DispatchCommand(
						this->command_id,
						cmd::FetchCommand(
							cmd::IsAnswer(true), fetch_command.GetRoute(), 
							this->fetch_answer_redirector(
								fetch_command.GetWrappedByteArray())
						).Serialize()
					).Serialize()
				);
			}
			else{ //When Query
				this->os << "on receive fetch query." << std::endl;
				auto is_answer = 
					this->is_turning_point_decider(fetch_command.GetRoute(), 
						fetch_command.GetWrappedByteArray());
				auto redirect_byte_array = 
					this->fetch_query_redirector(fetch_command.GetWrappedByteArray());
				this->at_random_selector(*to_session_pool)->Send(
					cmd::DispatchCommand(
						this->command_id,
						cmd::FetchCommand(
							is_answer, fetch_command.GetRoute(), 
							redirect_byte_array
						).Serialize()
					).Serialize(),
					[](nr::ntw::Session::Pointer){}
				);
			}
		}
	}

	IsTurningPointDecider is_turning_point_decider;
	FetchQueryRedirector fetch_query_redirector;
	FetchAnswerRedirector fetch_answer_redirector;
	OnReceiveFetchAnswerFunc on_receive_fetch_answer_func;
	nr::utl::RandomElementSelector at_random_selector;
	nr::ntw::Client::Pointer client;
	
	cmd::CommandId::WrappedType command_id;
	nr::NodeId node_id;
	nr::ntw::SessionPool::Pointer to_session_pool;
	std::ostream& os;
};

}

