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

// ndn-tree-cs-tracers.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/**
 * This scenario simulates a tree topology (using topology reader module)
 *
 *    /------\      /------\      /------\      /------\
 *    |m-4   |      |m3    |      |m-2   |      |m-1   |
 *    \------/      \------/      \------/      \------/
 *         ^          ^                ^           ^
 *         |          |                |           |
 *          \        /                  \         /
 *           \      /                    \       /    10Mbps / 1ms
 *            \    /                      \     /
 *             |  |                        |   |
 *             v  v                        v   v
 *          /-------\                    /-------\
 *          | r-1 |                      | r-2 |
 *          \-------/                    \-------/
 *                ^                        ^
 *                |                        |
 *                 \                      /  10 Mpbs / 1ms
 *                  +--------+  +--------+
 *                           |  |
 *                           v  v
 *                        /--------\
 *                        | user   |
 *                        \--------/
 *
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     ./waf --run=ndn-tree-cs-tracers
 */

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/ndn-mr-tree.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

	
  ndn::StrategyChoiceHelper::InstallAll("prefix", "/localhost/nfd/strategy/multicast");

	
  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();




  // Getting containers for the consumer/producer
  Ptr<Node> producer1 = Names::Find<Node>("m1");
	Ptr<Node> producer2 = Names::Find<Node>("m2");
	Ptr<Node> producer3 = Names::Find<Node>("m3");
	Ptr<Node> producer4 = Names::Find<Node>("m4");
	Ptr<Node> producer5 = Names::Find<Node>("m5");
	Ptr<Node> producer6 = Names::Find<Node>("m6");
	Ptr<Node> producer7 = Names::Find<Node>("m7");
	Ptr<Node> producer8 = Names::Find<Node>("m8");
	
	Ptr<Node> consumer = Names::Find<Node>("user");
	
	Ptr<Node> manager = Names::Find<Node>("mr");
	
	Ptr<Node> compute1 = Names::Find<Node>("r1");
	Ptr<Node> compute2 = Names::Find<Node>("r2");
	
	ndn::AppHelper producerHelper("ns3::ndn::MrSensor");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.SetPrefix("/m1-");
    producerHelper.Install(producer1);
    producerHelper.SetPrefix("/m2-");
    producerHelper.Install(producer2);
    producerHelper.SetPrefix("/m3-");
    producerHelper.Install(producer3);
    producerHelper.SetPrefix("/m4-");
    producerHelper.Install(producer4);
    producerHelper.SetPrefix("/m5-");
    producerHelper.Install(producer5);
    producerHelper.SetPrefix("/m6-");
    producerHelper.Install(producer6);
    producerHelper.SetPrefix("/m7-");
    producerHelper.Install(producer7);
    producerHelper.SetPrefix("/m8-");
    producerHelper.Install(producer8);
	
	ndn::AppHelper computeNodeHelper("ns3::ndn::MrCompute");
	computeNodeHelper.SetAttribute("PayloadSize", StringValue("1024"));
	computeNodeHelper.SetPrefix("/r1-");
	computeNodeHelper.Install(compute1);
	//computeNodeHelper.SetPrefix("/r");
	ndn::AppHelper computeNodeHelper2("ns3::ndn::MrCompute2");
	computeNodeHelper2.SetAttribute("PayloadSize", StringValue("1024"));
	computeNodeHelper2.SetPrefix("/r2-");
	computeNodeHelper2.Install(compute2);
	
	ndn::AppHelper consumerHelper("ns3::ndn::MrUser");
	//consumerHelper.SetAttribute("Frequency", StringValue("100")); // 100 interests a second
	//consumerHelper.SetPrefix("/m1");
	consumerHelper.Install(consumer);
	
	ndn::AppHelper managerNodeHelper("ns3::ndn::MrManager");
	managerNodeHelper.SetAttribute("PayloadSize", StringValue("1024"));
	managerNodeHelper.SetPrefix("/mr--");
	managerNodeHelper.Install(manager);
	
  // Register /root prefix with global routing controller and
  // install producer that will satisfy Interests in /root namespace 
  ndnGlobalRoutingHelper.AddOrigins("/m1-", producer1);
  ndnGlobalRoutingHelper.AddOrigins("/m2-", producer2);
  ndnGlobalRoutingHelper.AddOrigins("/m3-", producer3);
  ndnGlobalRoutingHelper.AddOrigins("/m4-", producer4);
  ndnGlobalRoutingHelper.AddOrigins("/m5-", producer5);
  ndnGlobalRoutingHelper.AddOrigins("/m6-", producer6);
  ndnGlobalRoutingHelper.AddOrigins("/m7-", producer7);
  ndnGlobalRoutingHelper.AddOrigins("/m8-", producer8);
  
  ndnGlobalRoutingHelper.AddOrigins("/mr-", manager);
  
  ndnGlobalRoutingHelper.AddOrigins("/r1-", compute1);
  ndnGlobalRoutingHelper.AddOrigins("/r2-", compute2);
  //ndnGlobalRoutingHelper.AddOrigins("/r", compute2);
  //ndnGlobalRoutingHelper.AddOrigins("/r1", compute1);
 

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

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
