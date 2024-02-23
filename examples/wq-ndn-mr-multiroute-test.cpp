/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-congestion-alt-topo-plugin.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/**
 *
 *   /------\ 0                                                 0 /------\
 *   |  c1  |<-----+                                       +----->|  p1  |
 *   \------/       \                                     /       \------/
 *                   \              /-----\              /
 *   /------\ 0       \         +==>| r12 |<==+         /       0 /------\
 *   |  c2  |<--+      \       /    \-----/    \       /      +-->|  p2  |
 *   \------/    \      \     |                 |     /      /    \------/
 *                \      |    |   1Mbps links   |    |      /
 *                 \  1  v0   v5               1v   2v  3  /
 *                  +->/------\                 /------\<-+
 *                    2|  r1  |<===============>|  r2  |4
 *                  +->\------/4               0\------/<-+
 *                 /    3^                           ^5    \
 *                /      |                           |      \
 *   /------\ 0  /      /                             \      \  0 /------\
 *   |  c3  |<--+      /                               \      +-->|  p3  |
 *   \------/         /                                 \         \------/
 *                   /     "All consumer-router and"     \
 *   /------\ 0     /      "router-producer links are"    \    0 /------\
 *   |  c4  |<-----+       "10Mbps"                        +---->|  p4  |
 *   \------/                                                    \------/
 *
 *   "Numbers near nodes denote face IDs. Face ID is assigned based on the order of link"
 *   "definitions in the topology file"
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-congestion-alt-topo-plugin
 */

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/ndn-mr-multiroute.txt");
  topologyReader.Read();

   // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();
  
  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();
  ndn::StrategyChoiceHelper::InstallAll("prefix", "/localhost/nfd/strategy/bestroute");
  

  // Set strategy
  /*
  ndn::StrategyChoiceHelper::InstallAll("/m1-", "/localhost/nfd/strategy/wqmrmulticast");
  ndn::StrategyChoiceHelper::InstallAll("/r1-", "/localhost/nfd/strategy/wqmrmulticast");
  ndn::StrategyChoiceHelper::InstallAll("/r2-", "/localhost/nfd/strategy/wqmrmulticast");
  ndn::StrategyChoiceHelper::InstallAll("/r3-", "/localhost/nfd/strategy/wqmrmulticast");
  //~ ndn::StrategyChoiceHelper::InstallAll("/localhost", "/localhost/nfd/strategy/wqmrmulticast");
  */

  // Getting containers for the consumer/producer
  Ptr<Node> producer1 = Names::Find<Node>("m1");
  Ptr<Node> producer2 = Names::Find<Node>("m2");
  Ptr<Node> producer3 = Names::Find<Node>("m3");
  Ptr<Node> producer4 = Names::Find<Node>("m4");
  Ptr<Node> producer5 = Names::Find<Node>("m5");
  Ptr<Node> producer6 = Names::Find<Node>("m6");
  Ptr<Node> consumer = Names::Find<Node>("user");
  Ptr<Node> compute1 = Names::Find<Node>("r1");
	Ptr<Node> compute2 = Names::Find<Node>("r2");
  //Ptr<Node> compute3 = Names::Find<Node>("r3");
  Ptr<Node> combiner1 = Names::Find<Node>("c1");
	Ptr<Node> combiner2 = Names::Find<Node>("c2");
  Ptr<Node> combiner3 = Names::Find<Node>("c3");
  Ptr<Node> combiner4 = Names::Find<Node>("c4");
  Ptr<Node> combiner5 = Names::Find<Node>("c5");
  
  ndn::AppHelper producerHelper1("ns3::ndn::MrMapperTest");
  producerHelper1.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper1.SetPrefix("/m1-");
  producerHelper1.Install(producer1);
  
  ndn::AppHelper producerHelper2("ns3::ndn::MrMapperTest");
  producerHelper2.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper2.SetPrefix("/m2-");
  producerHelper2.Install(producer2);
  
  ndn::AppHelper producerHelper3("ns3::ndn::MrMapperTest");
  producerHelper3.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper3.SetPrefix("/m3-");
  producerHelper3.Install(producer3);
  
 
  ndn::AppHelper producerHelper4("ns3::ndn::MrMapperTest");
  producerHelper4.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper4.SetPrefix("/m4-");
  producerHelper4.Install(producer4);
  
  ndn::AppHelper producerHelper5("ns3::ndn::MrMapperTest");
  producerHelper5.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper5.SetPrefix("/m5-");
  producerHelper5.Install(producer5);
  
  ndn::AppHelper producerHelper6("ns3::ndn::MrMapperTest");
  producerHelper6.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper6.SetPrefix("/m6-");
  producerHelper6.Install(producer6);
  
  ndn::AppHelper computeNodeHelper("ns3::ndn::MrReducerTest");
	computeNodeHelper.SetAttribute("PayloadSize", StringValue("1024"));
	computeNodeHelper.SetPrefix("/r1-");
  //computeNodeHelper.SetPrefix("/b-");
	computeNodeHelper.Install(compute1);
  
  
	ndn::AppHelper computeNodeHelper2("ns3::ndn::MrReducerTest");
	computeNodeHelper2.SetAttribute("PayloadSize", StringValue("1024"));
	computeNodeHelper2.SetPrefix("/r2-");
	computeNodeHelper2.Install(compute2);
  
  /*
  ndn::AppHelper computeNodeHelper3("ns3::ndn::MrComputeTest");
	computeNodeHelper3.SetAttribute("PayloadSize", StringValue("1024"));
	computeNodeHelper3.SetPrefix("/r3-");
	computeNodeHelper3.Install(compute3);
	*/
  
	ndn::AppHelper consumerHelper("ns3::ndn::MrUser");
	//consumerHelper.SetAttribute("Frequency", StringValue("100")); // 100 interests a second
	consumerHelper.SetPrefix("/user-");
	consumerHelper.Install(consumer);
  
  ndn::AppHelper combinerHelper1("ns3::ndn::MrReducerTest");
	combinerHelper1.SetAttribute("PayloadSize", StringValue("1024"));
	combinerHelper1.SetPrefix("/c1-");
	combinerHelper1.Install(combiner1);
  
  ndn::AppHelper combinerHelper2("ns3::ndn::MrReducerTest");
	combinerHelper2.SetAttribute("PayloadSize", StringValue("1024"));
	combinerHelper2.SetPrefix("/c2-");
	combinerHelper2.Install(combiner2);
  
  ndn::AppHelper combinerHelper3("ns3::ndn::MrReducerTest");
	combinerHelper3.SetAttribute("PayloadSize", StringValue("1024"));
	combinerHelper3.SetPrefix("/c3-");
	combinerHelper3.Install(combiner3);
  
 
  ndn::AppHelper combinerHelper4("ns3::ndn::MrReducerTest");
	combinerHelper4.SetAttribute("PayloadSize", StringValue("1024"));
	combinerHelper4.SetPrefix("/c4-");
	combinerHelper4.Install(combiner4);
   
  ndn::AppHelper combinerHelper5("ns3::ndn::MrReducerTest");
	combinerHelper5.SetAttribute("PayloadSize", StringValue("1024"));
	combinerHelper5.SetPrefix("/c5-");
	combinerHelper5.Install(combiner5);
  
  
/*
  // Manually configure FIB routes
  ndn::FibHelper::AddRoute("r1", "/m1-", "m1", 1);
  ndn::FibHelper::AddRoute("r2", "/m1-", "m1", 5);
  ndn::FibHelper::AddRoute("r3", "/m1-", "m1", 7);
  
  ndn::FibHelper::AddRoute("user", "/r1-", "r1", 1);
  ndn::FibHelper::AddRoute("user", "/r2-", "r2", 1);
  ndn::FibHelper::AddRoute("user", "/r3-", "r3", 1);
*/

  //ndnGlobalRoutingHelper.AddOrigins("/b-", compute1);
  ndnGlobalRoutingHelper.AddOrigins("/r1-", compute1);
  ndnGlobalRoutingHelper.AddOrigins("/r2-", compute2);
  //ndnGlobalRoutingHelper.AddOrigins("/r3-", compute3);
  
  ndnGlobalRoutingHelper.AddOrigins("/m1-", producer1);
  ndnGlobalRoutingHelper.AddOrigins("/m2-", producer2);
  ndnGlobalRoutingHelper.AddOrigins("/m3-", producer3);
  ndnGlobalRoutingHelper.AddOrigins("/m4-", producer4);
  ndnGlobalRoutingHelper.AddOrigins("/m5-", producer5);
  ndnGlobalRoutingHelper.AddOrigins("/m6-", producer6);
  
  ndnGlobalRoutingHelper.AddOrigins("/c1-", combiner1);
  ndnGlobalRoutingHelper.AddOrigins("/c2-", combiner2);
  ndnGlobalRoutingHelper.AddOrigins("/c3-", combiner3);
  ndnGlobalRoutingHelper.AddOrigins("/c4-", combiner4);
  ndnGlobalRoutingHelper.AddOrigins("/c5-", combiner5);
  
  
  ndn::L3RateTracer::Install(Names::Find<Node>("user"), "user-trace.txt", Seconds(0.1));

  
  ndnGlobalRoutingHelper.AddOrigins("/user-", consumer);
  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();
  //ndn::GlobalRoutingHelper::CalculateAllPossibleRoutes();

  // Schedule simulation time and run the simulation
  Simulator::Stop(Seconds(20.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}

