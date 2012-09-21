#pragma once
//TypeWrapper:20120922
#include <iostream>

namespace sy
{
class FromSessionType{};
using FromSession = nr::utl::TypeWrapper<nr::ntw::Session::Pointer, FromSessionType>;

class ToSessionType{};
using ToSession = nr::utl::TypeWrapper<nr::ntw::Session::Pointer, ToSessionType>;

class AcceptedPoolType{};
using AcceptedPool = nr::utl::TypeWrapper<nr::ntw::SessionPool::Pointer, 
	AcceptedPoolType>;

class ConnectedPoolType{};
using ConnectedPool = nr::utl::TypeWrapper<nr::ntw::SessionPool::Pointer, 
	ConnectedPoolType>;

}

