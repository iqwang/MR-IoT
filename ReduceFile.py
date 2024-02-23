def reducer(x, b):
  
  print "reduce function run";
  #print "b is: ", b;
  '''
  a =10;
  b = 100;
  r1 = a+b;
  return r1;
  '''
  commaPos = [];
  linePos = [];
  for n in range(len(b)):
    if b[n] == "-":
      #print "n1: ",n
      linePos.append(n)
      #print "line: ",len(linePos)
    if b[n] == ",":
      #print "n2: ",n
      commaPos.append(n)
      #print "comma: ",len(commaPos)
      
  kList = [];
  vList = [];
  k1 = b[:linePos[0]];
  #print "k1: ",k1;
  kList.append(k1);
  for i in range(len(linePos)-1):
    n1 = linePos[i]+1
    n2 = commaPos[i]
    n3 = commaPos[i]+1
    n4 = linePos[i+1]
    v1 = b[n1:n2]
    #print "v1: ",v1;
    vList.append(v1);
    v2 = b[n3:n4]
    #print "k2: ",v2;
    kList.append(v2)
    #print tempAllValue," is value"
    
  m1 = linePos[-1];
  m2 = commaPos[-1];
  k2 = b[m1+1:m2];
  vList.append(k2);
  #print "v2: ",k2;
  #print "v length: ",len(vList)," and k len: ",len(kList);
  
  tempoK = [];
  posList = [];
  replyData = "";
  
  while len(kList)!=0:
    valueList =[];
    tempKey = kList[0]
    replyData +=tempKey;
    replyData += "-";
    #print "here: ",replyData;
    
    for i1 in range(0,len(kList)):
      if tempKey == kList[i1]:
        posList.append(i1)
        tempoK.append(kList[i1])
        vInt = int(vList[i1]);
        valueList.append(vInt)
        #print "valueList len: ",len(valueList);
  
    #print "reduce"
    c = reduce(lambda a,b: eval(x), valueList);
    #print "c is:",c; 
    c1 = str(c);
    replyData +=c1;
    replyData +=",";
    #print len(kList)," and ",len(vList)
    
    #print "length: ",len(posList);
    for i2 in range(len(posList)-1,-1,-1):
      t1=posList[i2]
      del kList[t1]
      del vList[t1]
    
    del tempoK[:];
    del posList[:];
  
  #print "Python reply Data: ",replyData;
  return replyData;
  
         
  '''
  while len(kList)!=0:
    valueList =[];
    tempKey = kList[0]
    replyData +=tempKey;
    replyData += "-";
    print "here: ",replyData;
    #print "new temp key: ",tempKey
    for i1 in range(0,len(kList)):
      if tempKey == kList[i1]:
        posList.append(i1)
        tempoK.append(kList[i1])
        tempoV += vList[i1]
        print "temp v: ",tempoV
    
    slashPos = [];
    for j in range(len(tempoV)):
      if tempoV[j] == "/":
        #print j
        slashPos.append(j)
        
    k3 = tempoV[:slashPos[0]];
    k33 = int(k3);
    valueList.append(k33);
    #print "k3: ",k3
    for m in range(len(slashPos)-1):
      n5 = slashPos[m]+1
      n6 = slashPos[m+1]
      v3 = tempoV[n5:n6]
      v33 = int(v3)
      valueList.append(v33);
      #print "v3",v3
 '''
  

