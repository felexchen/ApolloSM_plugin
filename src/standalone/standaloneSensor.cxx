#include <stdio.h>
#include <ApolloSM/ApolloSM.hh>
#include <uhal/uhal.hpp>
#include <vector>
#include <string>
#include <boost/tokenizer.hpp>
#include <unistd.h> // usleep
#include <signal.h>

// ====================================================================================================
// Defintions

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

struct temperatures {
  uint8_t MCUTemp;
  uint8_t FIREFLYTemp;
  uint8_t FPGATemp;
  uint8_t REGTemp;
};

// ====================================================================================================
// Kill program if it is in background
bool volatile loop;

void static signal_handler(int const signum) {
  if(SIGINT == signum) {
    printf("\n");
    loop = false;
  }
  return;
}

// ====================================================================================================

temperatures sendAndParse(ApolloSM** SMptr) {
  temperatures temps;
  
  // read and print
  std::string recv((*SMptr)->UART_CMD("CM.CM1", "simple_sensor", '%'));
  //printf("\nReceived:\n\n%s\n\n", recv.c_str());
  
  // Separate by line
  boost::char_separator<char> lineSep("\r\n");
  tokenizer lineTokens{recv, lineSep};

  // One vector for each line 
  std::vector<std::vector<std::string> > allTokens;

  // Separate by spaces
  boost::char_separator<char> space(" ");
  int vecCount = 0;
  // For each line
  for(tokenizer::iterator lineIt = lineTokens.begin(); lineIt != lineTokens.end(); ++lineIt) {
    tokenizer wordTokens{*lineIt, space};
    // We don't yet own any memory in allTokens so we append a blank vector
    std::vector<std::string> blankVec;
    allTokens.push_back(blankVec);
    // One vector per line
    for(tokenizer::iterator wordIt = wordTokens.begin(); wordIt != wordTokens.end(); ++wordIt) {
      allTokens[vecCount].push_back(*wordIt);
    }
    vecCount++;
  }
  /*
  uhal::Node const & nS2_1 = SM->GetNode("SLAVE_I2C.S2.1");
  uhal::Node const & nS3_1 = SM->GetNode("SLAVE_I2C.S3.1");
  uhal::Node const & nS4_1 = SM->GetNode("SLAVE_I2C.S4.1");
  uhal::Node const & nS5_1 = SM->GetNode("SLAVE_I2C.S5.1");
  */

  float ftemp1 = std::atof(allTokens[0][1].c_str());
  if(0 > ftemp1) {
    ftemp1 = 0;
  }
  temps.MCUTemp = (uint8_t)ftemp1;

  float ftemp2 = std::atof(allTokens[1][1].c_str());
  if(0 > ftemp2) {
    ftemp2 = 0;
  }
  temps.FIREFLYTemp = (uint8_t)ftemp2;

  float ftemp3 = std::atof(allTokens[2][1].c_str());
  if(0 > ftemp3) {
    ftemp3 = 0;
  }
  temps.FPGATemp = (uint8_t)ftemp3;

  float ftemp4 = std::atof(allTokens[3][1].c_str());
  if(0 > ftemp4) {
    ftemp4 = 0;
  }
  temps.REGTemp = (uint8_t)ftemp4;  

  return temps;
}

// ====================================================================================================

void sendTemps(ApolloSM** SMptr, temperatures temps) {
  (*SMptr)->RegWriteNode((*SMptr)->GetNode("SLAVE_I2C.S2.1"), temps.MCUTemp);
  (*SMptr)->RegWriteNode((*SMptr)->GetNode("SLAVE_I2C.S3.1"), temps.FIREFLYTemp);
  (*SMptr)->RegWriteNode((*SMptr)->GetNode("SLAVE_I2C.S4.1"), temps.FPGATemp);
  (*SMptr)->RegWriteNode((*SMptr)->GetNode("SLAVE_I2C.S5.1"), temps.REGTemp);
}

// ====================================================================================================

int main(int, char**) {

  //  std::string sendline("simple_sensor");

  ApolloSM * SM = NULL;
  SM = new ApolloSM();
  std::vector<std::string> arg;
  arg.push_back("connections.xml");
  SM->Connect(arg);

  // read and print
  //std::string baseNode("CM.CM1"); 
  
//std::string recv(SM->UART_CMD(baseNode, sendline, '%'));
//printf("\nReceived:\n\n%s\n\n", recv.c_str());
//
//typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
//
//// Separate by line
//boost::char_separator<char> lineSep("\r\n");
//tokenizer lineTokens{recv, lineSep};
//
//// One vector for each line 
//std::vector<std::vector<std::string> > allTokens;
//
//// Separate by spaces
//boost::char_separator<char> space(" ");
//int vecCount = 0;
//// For each line
//for(tokenizer::iterator lineIt = lineTokens.begin(); lineIt != lineTokens.end(); ++lineIt) {
//  tokenizer wordTokens{*lineIt, space};
//  // We don't yet own any memory in allTokens so we append a blank vector
//  std::vector<std::string> blankVec;
//  allTokens.push_back(blankVec);
//  // One vector per line
//  for(tokenizer::iterator wordIt = wordTokens.begin(); wordIt != wordTokens.end(); ++wordIt) {
//    allTokens[vecCount].push_back(*wordIt);
//  }
//  vecCount++;
//}
//
//// Print all tokens
//for(int i = 0; i < (int)allTokens.size(); i++) {
//  printf("Line %d:\n", i); 
//  for(int j = 0; j < (int)allTokens[i].size(); j++) {
//    printf("%s   ", allTokens[i][j].c_str());
//    fflush(stdout);
//  }
//  printf("\n");
//}
//
//// Write temperatures to nodes
//std::string si2cNode("SLAVE_I2C.");
//
////std::vector<uhal::Node> si2cNodes;
////for(int i = 0; i < (int)si2cNodes.size(); i++) {
////  si2cNodes.push_back(SM->GetNode(si2cNode+"S"+std::to_string(i+2)+".1"));
////  printf("Created node: %s\n", si2cNodes[i].getPath().c_str());
////}
//
//uhal::Node const & nS2_1 = SM->GetNode(si2cNode+"S2.1");
//uhal::Node const & nS3_1 = SM->GetNode(si2cNode+"S3.1");
//uhal::Node const & nS4_1 = SM->GetNode(si2cNode+"S4.1");
//uhal::Node const & nS5_1 = SM->GetNode(si2cNode+"S5.1");
//
//float ftemp1 = std::atof(allTokens[0][1].c_str());
//if(0 > ftemp1) {
//  ftemp1 = 0;
//}
//uint8_t temp1 = (uint8_t)ftemp1;
//
//float ftemp2 = std::atof(allTokens[1][1].c_str());
//if(0 > ftemp2) {
//  ftemp2 = 0;
//}
//uint8_t temp2 = (uint8_t)ftemp2;
//
//float ftemp3 = std::atof(allTokens[2][1].c_str());
//if(0 > ftemp3) {
//  ftemp3 = 0;
//}
//uint8_t temp3 = (uint8_t)ftemp3;
//
//float ftemp4 = std::atof(allTokens[3][1].c_str());
//if(0 > ftemp4) {
//  ftemp4 = 0;
//}
//uint8_t temp4 = (uint8_t)ftemp4;

//  temperatures temps = sendAndParse(&SM);
  temperatures temps;  

  //  printf("We got back:\n%d\n%d\n%d\n%d\n", temps.MCUTemp, temps.FIREFLYTemp, temps.FPGATemp, temps.REGTemp);
/*
  printf("Writing %s temperature %d to node %s\n", allTokens[0][0].c_str(), temp1, nS2_1.getPath().c_str());
  printf("Writing %s temperature %d to node %s\n", allTokens[1][0].c_str(), temp2, nS3_1.getPath().c_str()); 
  printf("Writing %s temperature %d to node %s\n", allTokens[2][0].c_str(), temp3, nS4_1.getPath().c_str()); 
  printf("Writing %s temperature %d to node %s\n", allTokens[3][0].c_str(), temp4, nS5_1.getPath().c_str()); 
  
//printf("Writing %s temperature %d to node %sS2.1\n", allTokens[0][0].c_str(), temp1, si2cNode.c_str()); 
//printf("Writing %s temperature %d to node %sS3.1\n", allTokens[1][0].c_str(), std::atoi(allTokens[1][1].c_str()), si2cNode.c_str()); 
//printf("Writing %s temperature %d to node %sS4.1\n", allTokens[2][0].c_str(), std::atoi(allTokens[2][1].c_str()), si2cNode.c_str()); 
//printf("Writing %s temperature %d to node %sS5.1\n", allTokens[3][0].c_str(), std::atoi(allTokens[3][1].c_str()), si2cNode.c_str());

  SM->RegWriteNode(nS2_1, temp1);
  SM->RegWriteNode(nS3_1, temp2);
  SM->RegWriteNode(nS4_1, temp3);
  SM->RegWriteNode(nS5_1, temp4);
  */
  
//  sendTemps(&SM, temps);

  // Catch SIGINT to set loop = false
  struct sigaction sa;
  // Restore old SIGINT action
  struct sigaction oldsa;
  // Instantiate sigaction struct member with signal handler function
  sa.sa_handler = signal_handler;
  sigaction(SIGINT, &sa, &oldsa);
  loop = true;

  //  timespec ts;
  //  time_t start;

  while(loop) {
    // start time
    //    clock_gettime(CLOCK_REALTIME, &ts);
    //start = ts.tv_sec;

    temps = sendAndParse(&SM);
    sendTemps(&SM, temps);
    usleep(10000000);
  }

  // Restore old action of receiving SIGINT (which is to kill program) before returning 
  sigaction(SIGINT, &oldsa, NULL);

  printf("Successful kill\n");

  return 0;
}
