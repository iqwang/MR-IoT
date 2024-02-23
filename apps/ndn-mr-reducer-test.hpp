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

#ifndef NDN_MRREDUCERTEST_H
#define NDN_MRREDUCERTEST_H

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/random-variable-stream.h"
#include "ndn-app.hpp"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include <deque>


namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief A simple Interest-sink applia simple Interest-sink application
 *
 * A simple Interest-sink applia simple Interest-sink application,
 * which replying every incoming Interest with Data packet with a specified
 * size and name same as in Interest.cation, which replying every incoming Interest
 * with Data packet with a specified size and name same as in Interest.
 */
class MrReducerTest : public App {
public:
  static TypeId
  GetTypeId(void);

  MrReducerTest();

  // inherited from NdnApp
  virtual void
  OnInterest(shared_ptr<const Interest> interest);
  // From App
  virtual void
  OnData(shared_ptr<const Data> contentObject);

public:
  void FindNeighbours();
  void DiscoverDownstreams();
  void QueryPit();


protected:
  // inherited from Application base class.
  virtual void
  StartApplication(); // Called at time specified by Start
  virtual void
  StopApplication(); // Called at time specified by Stop


private:
  Ptr<UniformRandomVariable> m_rand;
  Name m_prefix;
  Name m_postfix;
  uint32_t m_virtualPayloadSize;
  Time m_freshness;
  uint32_t m_signature;
  Name m_keyLocator;

  
  
  std::string m_taskInterestName;
  Name m_pendingInterestName;
  std::map<std::string, std::string> m_buildTreeInterestMap;
  std::string m_askPitPrefix;
  std::string m_askFibPrefix;
  int m_askPitNum=0;
  int m_gotPitNum=0;
  std::string m_selectUpstream;
  std::vector<std::string> m_oneHopNeighbours;
  int m_sendDisNeiNum=0;
  int m_gotDisNeiNum=0;
  std::map<std::string, std::string> m_checkNeibMap;
  std::string m_disNeiPrefix;
  std::string m_disDownStream1;
  std::string m_disDownStream2;
  int m_sendDisDownNeiNum =0;
  int m_gotDisDownNeiNum =0;
  std::vector<std::string> m_pendingTreeTagList;
  std::map<std::string, std::string> m_pendTreeJobMap;
  std::string m_treeTag = "0";
  std::string m_currentTreeFlag = "0";
  std::map<std::string, std::string> m_jobRefMap;
  std::string m_jobRefNei = "0";
  std::map<std::string, std::string> m_disDownNodeMap;
  std::vector<std::string> m_nodeList4Task;
  std::string m_assignTask;
  std::string m_reduceFunc;
  int m_sendTaskNum =0;
  int m_gotTaskNum =0;
  std::string m_tContent = "0";
  std::vector<std::string> m_upDisNodes;
  std::map<std::string, std::string> m_fibResult;
  std::string m_selectNodeName = "0";
  
};


} // namespace ndn
} // namespace ns3

#endif // NDN_MRCOMPUTETEST_H
