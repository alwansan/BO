// Real UDP IPv4 loopback validation of the standalone discovery adapter.
// NO game, no Android phone runtime, no broadcast, no production auth.
#include "lan_socket.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <iostream>
using namespace bo::lan;
Source Loopback(uint16_t port){
    return {htonl(INADDR_LOOPBACK),port};
}
int main(){
    Socket host,client,dup;
    assert(host.Open());assert(client.Open());
    assert(host.Port()!=client.Port());
    assert(!dup.Open(host.Port())); // No reuse sockets: collision prevented.
    const auto hostAddress=Loopback(host.Port());
    const auto clientAddress=Loopback(client.Port());
    assert(!host.SendTo(hostAddress,{}));
    assert(client.SendTo(hostAddress,Query(0xabcde010)));
    Message query{};
    Source from{};
    assert(host.Receive(query,from));
    assert(query.type==Type::Query && query.challenge==0xabcde010);
    assert(from==clientAddress);
    Responder responder;
    Bytes response;
    // Guard against two UDP services binding a shared logical port.
    assert(!responder.Respond(query,from,0,host.Port(),host.Port(),1,4,"ROOM",response));
    assert(responder.Respond(query,from,0,1508,host.Port(),1,4,"COOP Room",response));
    assert(!responder.Respond(query,from,1,1508,host.Port(),1,4,"COOP Room",response));
    assert(host.SendTo(from,response));
    Message answer{};Source replyFrom{};
    assert(client.Receive(answer,replyFrom));
    assert(replyFrom==hostAddress);
    assert(IsReplyFor(answer,0xabcde010,host.Port()));
    assert(answer.game_port==1508 && answer.name=="COOP Room");
    assert(!IsReplyFor(answer,0xabcde011,host.Port()));
    // Drop invalid version message; never reply to unauthenticated payload.
    Bytes bad=Query(0xabcde010);
    bad[4]=2;
    assert(client.SendTo(hostAddress,bad));
    assert(!host.Receive(query,from,100));
    // No incoming data after query: bounded timeout rather than a hung app.
    assert(!client.Receive(answer,replyFrom,100));
    PeerRateGate peers;
    Source a=Loopback(20000);
    assert(peers.Allow(a,0));assert(!peers.Allow(a,499));
    assert(peers.Allow(a,500));
    for(uint16_t p=20001;p<20016;++p)assert(peers.Allow(Loopback(p),600));
    assert(!peers.Allow(Loopback(30000),700)); // memory bound
    assert(peers.Allow(Loopback(30000),6500)); // expired slot reuse
    // Reopening must release port and not leave duplicate sockets.
    auto held=host.Port();host.Close();
    assert(dup.Open(held));
    std::cout<<"PASS real UDP loopback LAN discovery query/reply, source+nonce, timeouts, rate limits, distinct bind\n";
    std::cout<<"No broadcast, internet, phone integration, auth, or BLACK actor writes\n";
}
