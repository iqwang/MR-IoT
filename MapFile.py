def mapper(a, b):
  import random
  
  print "mapper run";
  replyData= "";
  tempA = "";
  tempB = "";
  
  n1 = random.randrange(1,10,1);
  n1Str = str(n1);
  tempA += n1Str;
  a1 = a[1:];
  tempA += a1;
  #print "a is",tempA;
  n2 = random.randrange(10,100,10);
  n2Str = str(n2);
  tempB += n2Str;
  b1 = b[1:];
  tempB += b1;
  #print "b is",tempB;
  r1 = eval(tempA);
  #print "111",r1;
  r1Str = str(r1);
  replyData += r1Str;
  replyData += "-";
  #print "222",replyData;
  r2 = eval(tempB);
  r2Str = str(r2);
  replyData += r2Str;
  #print "replyData is",replyData;
  
  return replyData;
