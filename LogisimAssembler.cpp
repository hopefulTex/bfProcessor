// Bra*nFuck CPU Assembler
// Written in 2022 bc I keep forgetting that this project exists
// Rose Thatcher copyright yadayada

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

string openFile(string fileName, string &words, ifstream &inFile);

string parseFile(ifstream &inFile, int mode = 0);

bool writeToFile(string fileName, string &words, ofstream &outFile);

bool cleanUp(string &inFileName, string &outFileName,
  string &words, ifstream &inFile, ofstream &outFile);

//int helpPage();

int main(int argc, char *argv[]) {

  string inFileName = "";
  string outFileName = "";
  ifstream inFile;
  ofstream outFile;
  string words;

  if (argc < 2)
  {
    cout << endl << "Error: Missing argument(s)." << endl;
    //helpPage();
    return 1;
  }
  if (argc > 4) {
    cout << endl << "Error: Too many argument(s)." << endl;
    //helpPage();
    return 1;
  }
  if (argc == 2) {
    inFileName = argv[1];
    outFileName = "a.txt";
  } else if(argc == 3) {
    inFileName = argv[1];
    outFileName = argv[2];
  }
  
  words = openFile(inFileName, words, inFile);

  if(writeToFile(outFileName, words, outFile) != 0) {
    cout << endl << "Error: Could not write to file. Make sure you have access"
    << " to this folder" << endl;
    return 1;
  }

  /*if(cleanUp(inFileName, outFileName, words, inFile, outFile) != 0) {
    cout << endl << "Error: Cleanup unsuccessful, you should probably clear "
    << "your memory." << endl;
    return 1;
  }
  */

  return 0;
}

string parseFile(ifstream &inFile, int mode) {
  int openCount = 0;
  int closeCount = 0;
  int j = 0;
  const string INCLUDES_STRING = "&include ";
  string includes;
  ifstream included;
  string errVec;
  string line;
  string truncatedLine = "";
  string finalLine = "";

  errVec.push_back(15);

  // extract tokens
  while(std::getline(inFile,line)) {

    if(line.substr(0,9) != INCLUDES_STRING) {
      
      for(int i = 0; i < line.length(); i++) {
        switch (line[i])
        {
          case 0x2B:
            // +
            truncatedLine.push_back(line[i]);
            break;

          case 0x2C:
          // ,
            truncatedLine.push_back(line[i]);
            break;

          case 0x2D:
            // -
            truncatedLine.push_back(line[i]);
            break;

          case 0x2E:
            // .
            truncatedLine.push_back(line[i]);
            break;

          case 0x3C:
            // <
            truncatedLine.push_back(line[i]);
            break;

          case 0x3E:
            // >
            truncatedLine.push_back(line[i]);
            break;

          case 0x5B:
              // [
            truncatedLine.push_back(line[i]);
            break;

          case 0x5D:
            // ]
            truncatedLine.push_back(line[i]);
            break;

          default:
            break;
        }
      }
    } else {
      line.erase(0,9);
      truncatedLine += openFile(line, includes, included);
      return truncatedLine;
    }
  }

  // tokens -> code
  for(int i = 0; i < truncatedLine.length(); i++) {
      if(!(j % 8)) finalLine.push_back('\n');
      switch (truncatedLine[i])
      {
      case 0x2B:
        // +
        finalLine += "3 "; //0011 Increment Value
        if ((truncatedLine[i+1] != '+') && truncatedLine[i+1] != '-') {

          j++;
          if(!(j % 8)) finalLine.push_back('\n');
          finalLine += "0 "; //0000 Load Register -> Memory
          
        }
        break;

      case 0x2C:
        // ,
        finalLine += "8 "; //1000 Load ASCII Input -> Register
        if ((truncatedLine[i+1] != '+') && truncatedLine[i+1] != '-') {

          j++;
          if(!(j % 8)) finalLine.push_back('\n');
          finalLine += "0 "; //0000 Load Register -> Memory
          
        }
        break;
      
      case 0x2D:
        // -
        finalLine += "4 "; //0100 Deincrement Value
        if ((truncatedLine[i+1] != '+') && truncatedLine[i+1] != '-') {

          j++;
          if(!(j % 8)) finalLine.push_back('\n');
          finalLine += "0 "; //0000 Load Register -> Memory
          
        }
        break;

      case 0x2E:
        // .
        finalLine += "7 "; //0111 Load Memory -> Terminal
        break;

      case 0x3C:
        // <
        finalLine += "2 "; //0010 Deincrement Memory Address
        if ((truncatedLine[i+1] != '>') && truncatedLine[i+1] != '<') {

          j++;
          if(!(j % 8)) finalLine.push_back('\n');
          finalLine += "9 "; //1001 Load Memory -> Register
          
        }
        break;

      case 0x3E:
        // >
        finalLine += "1 "; //0001 Increment Memory Address
        if ((truncatedLine[i+1] != '>') && truncatedLine[i+1] != '<') {

          j++;
          if(!(j % 8)) finalLine.push_back('\n');
          finalLine += "9 "; //1001 Load Memory -> Register
          
        }
        break;

      case 0x5B:
        // [
        openCount++;
        finalLine += "5 "; //0101 Load PC -> Stack, Increment Stack Address
        break;

      case 0x5D:
        // ]
        closeCount++;
        finalLine += "6 "; //0110 (if Register !0) Deincrement Stack Address
        j++;
        if(!(j % 8)) finalLine.push_back('\n');
        finalLine += "a "; //1010 (if Register !0) Load Stack -> PC
        
        break;

      default:
        break;
      }
      j++;
    }
  if(!(j % 8)) finalLine += '\n'; 
  finalLine += "b "; //1011 HALT

  if(openCount != closeCount) return errVec;

  errVec.clear();
  line.clear();
  truncatedLine.clear();
  return finalLine;
}

string openFile(string fileName, string &words, ifstream &inFile) {
  string errVec;
  errVec.push_back(15);
  words.clear();

  inFile.open(fileName, ios::in);
  if(!inFile.is_open()) {
    cout << endl << "Error: Could not open file " << '"' << fileName << '"'
     << ". Please make sure you have permission to this folder." << endl; 
    return errVec;
  }
  
  words = parseFile(inFile);
  
  if(words == errVec) {
    cout << endl << "Error:(" << fileName << ") Uneven brackets. Check your "
    "loops." << endl;
  }
  errVec.clear();
  inFile.close();
  return words;
}

bool writeToFile(string fileName, string &words,
 ofstream &outFile) {
  string header = "v2.0 raw";

  words.insert(0,header);

  outFile.open(fileName, ios::out);
  if(!outFile.is_open()) return 1;

  outFile << words << '\n';
  
  return 0;
}

bool cleanUp(string &inFileName, string &outFileName, 
  string &words, ifstream &inFile, ofstream &outFile) {
  for(int i = 0; i < 2; i++) {
    
    if(!inFileName.empty()) {
      if(i == 1) {
        return 1;
      }
      inFileName.clear();
    }

    if(!words.empty()) {
      if(i == 1) {
        return 1;
      }
      words.clear();
    }

    if(inFile.is_open()) {
      if(i == 1) {
        return 1;
      }
      inFile.close();
    }

    if(outFile.is_open()) {
      if(i == 1) {
        return 1;
      }
      outFile.close();
    }
  
  }
  return 0;
}

