#pragma once
//BehaviorDispatcher:20120913
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "neuria/Neuria.h"
#include "command/DispatchCommand.h"

namespace syncia{

class BehaviorDispatcher : 
		public boost::enable_shared_from_this<BehaviorDispatcher> {
public:
	using Pointer = boost::shared_ptr<BehaviorDispatcher>;
	static auto Create(boost::asio::io_service& service, std::ostream& os) -> Pointer {
		return Pointer(new BehaviorDispatcher(service, os));	
	}
	
	auto RegisterFunc(const command::CommandId& command_id, 
			neuria::network::Session::OnReceivedFunc func) -> void{
		std::cout << "registered " << command_id << std::endl;
		this->func_dict[command_id()] = func;
	}
	
	auto GetOnReceivedFunc() -> neuria::network::Session::OnReceivedFunc {
		return boost::bind(
			&BehaviorDispatcher::Dispatch, this->shared_from_this(), _1, _2);
	}


private:
    BehaviorDispatcher(boost::asio::io_service& service, std::ostream& os)
		: service(service), registered_func_strand(service), os(os){}

	auto Dispatch(neuria::network::Session::Pointer session, 
			const neuria::ByteArray& byte_array) -> void {
		auto command = command::DispatchCommand::Parse(byte_array);
		this->os << "received dispatch command: " << command << std::endl;
		if(this->func_dict.find(command.GetCommandId()()) == this->func_dict.end()){
			this->os << "invalid command id:" << command.GetCommandId() << std::endl;
			session->Close();
			return;
		}
		else{
			this->os << "call command(id:" 
				<< command.GetCommandId() << ")" << std::endl;
			this->service.post(/*this->registered_func_strand.wrap(*/
				[this, session, command](){
					this->func_dict[command.GetCommandId()()](
						session, 
						command.GetWrappedByteArray()
					);
				}
			)/*)*/;
		}
	}
	
	friend auto operator<<(
		std::ostream& os, const BehaviorDispatcher& dispatcher) -> std::ostream&;

	boost::asio::io_service& service;
	boost::asio::strand registered_func_strand;
	std::map<command::CommandId::WrappedType, neuria::network::Session::OnReceivedFunc> func_dict;
	std::ostream& os;
};

inline auto operator<<(std::ostream& os, 
		const BehaviorDispatcher& dispatcher) -> std::ostream& {
	for(auto& pair : dispatcher.func_dict){
		std::cout << pair.first << ", " << std::flush;
	}
	return os;
}

}

