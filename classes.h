#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <fstream>  // Include the necessary header file for the getline function
using namespace std;

#define intSize sizeof(int)  // int size

class Record {
 public:
  int id, manager_id;
  std::string bio, name;

  Record(vector<std::string> fields) {
    id = stoi(fields[0]);
    name = fields[1];
    bio = fields[2];
    manager_id = stoi(fields[3]);
  }

  // serialize the record by passing memory as an unsigned char argument as
  // unsigned char* memory
  std::vector<char> serializeToString() {
    // cout << "name: " << name << endl;
    string serialStr = to_string(id) + "$" + name + "$" + bio + "$" +
                       to_string(manager_id) + "$";
    vector<char> serial = {};

    for (int i = 0; i < serialStr.size(); i++) {
      serial.push_back(serialStr[i]);
    }
    // Add the record to the memory
    return serial;
  }

  void print() {
    cout << "\tID: " << id << "\n";
    cout << "\tNAME: " << name << "\n";
    cout << "\tBIO: " << bio << "\n";
    cout << "\tMANAGER_ID: " << manager_id << "\n";
  }
};

class StorageBufferManager {
 private:
  const int BLOCK_SIZE = 4096;  // initialize the  block size allowed in main
  const int MAX_PAGE = 3;
  // You may declare variables based on your need
  int numRecords = 0;

  // pointer of page of memory with having 4096 bytes each
  // memory that store the variable-length records as unsigned byte array

  // create array of pointers to the last record in each page

  unsigned char* pageBuffer[4];

  unsigned char* memory;
  int* lastRecord;
  int* recordCount;

  bool isWritten = false;
  int targetId = NULL;
  int isFound = 0;

  // static_cast<unsigned char*>(std::malloc(BLOCK_SIZE * 3));
  // Create a file to store the records
  string fileName;
  std::ofstream file;
  std::ifstream fileReader;

  // create number of Position in each pages
  // create a current page number to show that what is the current page
  int currentPage = 0;  // page 0, 1, 2 due to 3 pages possible
  int currentLength = 0;

  // Insert new record
  void setNextPage() {
    currentPage++;
    currentLength = 0;
    memory = pageBuffer[currentPage];
    setLastRecord();
  }

  void setLastRecord() {
    lastRecord = (int*)(memory + BLOCK_SIZE - sizeof(int));
    recordCount = (int*)(memory + BLOCK_SIZE - sizeof(int) * 2);
    *lastRecord = 0;
    *recordCount = 0;
  }

  void initializeMemory() {
    // Initialize the memory
    for (int i = 0; i < MAX_PAGE + 1; i++) {
      free(pageBuffer[i]);
      pageBuffer[i] = static_cast<unsigned char*>(std::malloc(BLOCK_SIZE));
    }
    memory = pageBuffer[currentPage];
    setLastRecord();
  }

  void insertToMemory(std::vector<char> record) {
    size_t recordLen = record.size();
    for (int i = 0; i < recordLen; i++) {
      memory[currentLength + i] = record[i];
    }

    currentLength += recordLen;

    unsigned char* tmp = (unsigned char*)malloc(recordLen);
    for (int i = 0; i < recordLen; i++) {
      tmp[i] = memory[currentLength - recordLen + i];
    }

    *recordCount += 1;
    // save the free space
    memcpy(memory + BLOCK_SIZE - intSize, &currentLength, intSize);

    // save the record count
    // update the last slot in page which is the free position
    // update the length of the record to slot
    memcpy(memory + BLOCK_SIZE - intSize, &currentLength, intSize);

    // update record count to slot
    memcpy(memory + BLOCK_SIZE - intSize * 2, recordCount, intSize);

    // update the length of the block to slot
    int lengthOfBlock = (int)record.size();
    memcpy(memory + BLOCK_SIZE - intSize * 2 - intSize * (*recordCount),
           &lengthOfBlock, intSize);

    int* val = (int*)(memory + BLOCK_SIZE - intSize);
    int* val2 = (int*)(memory + BLOCK_SIZE - intSize * 2);
    int* val3 =
        (int*)(memory + BLOCK_SIZE - intSize * 2 - intSize * (*recordCount));
    // cout << "VALUE =" << *val << " " << *val2 << " " << *val3 << endl;
  }

  void insertRecord(Record record) {
    if (numRecords == 0) {
      initializeMemory();
    }
    numRecords++;
    isWritten = false;

    // Check if the current page is full

    // Serialize the record and insert record to memory
    std::vector<char> r = record.serializeToString();
    if (r.size() + currentLength >
        BLOCK_SIZE - sizeof(int) * (3 + (*recordCount))) {
      cout << numRecords << " " << r.size() << " " << currentLength << " "
           << BLOCK_SIZE - sizeof(int) * (3 + (*recordCount)) << endl;
      setNextPage();
    }

    if (currentPage == MAX_PAGE) {
      cout << "Write Pages buffer to file" << endl;
      writePageBufferToFile();
      isWritten = true;
      clearPages();
    }
    insertToMemory(r);
  }

  void clearPages() {
    // Clear the page
    // Set the page to 0

    currentPage = 0;
    // free(pageBuffer);
    initializeMemory();
  }

 public:
  StorageBufferManager(string NewFileName) {
    // initialize your variables
    fileName = NewFileName;
  }

  // loop print value in pageBuffer[2]

  void writePageBufferToFile() {
    for (int i = 0; i < MAX_PAGE; i++) {
      cout << "Writing to file at Page " << i + 1 << endl;
      file.write((char*)(pageBuffer[i]), BLOCK_SIZE);
    }
  }

  // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
  void createFromFile(string csvFName) {
    // Add records to the EmployeeRelation
    std::ifstream csvFile(csvFName);
    // Read the file and add records to the EmployeeRelation
    file = std::ofstream(fileName, std::ios::out | std::ios::binary);
    if (!csvFile.is_open()) {
      return;
    }
    string line;
    cout << "CREATED FILE" << endl;
    while (std::getline(csvFile, line, '\n')) {
      // Split the line into fields
      vector<string> fields;
      stringstream ss(line);
      string field;
      while (std::getline(ss, field, ',')) {
        fields.push_back(field);
      }
      // cout << fields[0] << endl;
      // Create a record from the fields
      Record record(fields);
      // Insert the record into the EmployeeRelation
      insertRecord(record);
    }

    if (isWritten == false) {
      cout << "Write Pages buffer to file...." << endl;
      writePageBufferToFile();
      isWritten = true;
    }

    file.close();
    // Close the file
    csvFile.close();
    clearPages();
  }

  void findValue(int pgCount, int target, unsigned char* pageBufferTmp) {
    for (int n = 0; n < pgCount; n++) {
      // cout << "PAGE COUNT: " << pgCount << " " << n << endl;
      memory = pageBufferTmp;
      int* val = (int*)(memory + BLOCK_SIZE - intSize);
      int* val2 = (int*)(memory + BLOCK_SIZE - intSize * 2);
      int itemCount = *val2;
      // if (*val == 0) {
      //   break;
      // }
      // cout << "Print Size val: " << *val << " __" << *val2 << endl;

      int sum = 0;
      for (int i = 0; i < itemCount; i++) {
        int* val3 = (int*)(memory + BLOCK_SIZE - intSize * 3 - intSize * i);
        // cout << "Print Size val3 : " << *val3 << " __" << endl;
        char* val4 = (char*)malloc(*val3);
        memcpy(val4, memory + sum, *val3);
        sum += *val3;
        // cout << val4 << endl;

        vector<string> fields;
        string str = string(val4);
        stringstream ss(str);
        string field;
        while (std::getline(ss, field, '$')) {
          fields.push_back(field);
        }
        // cout << "ID: " << fields[0] << endl;
        Record record(fields);
        if (record.id == target) {
          isFound = 1;
          record.print();
          return;
        }
      }
    }
  }

  void b2b() {}

  void readFromFile(int target) {
    for (int i = 0; i < MAX_PAGE; i++) {
      free(pageBuffer[i]);
      pageBuffer[i] = static_cast<unsigned char*>(std::malloc(BLOCK_SIZE));
    }
    isFound = 0;
    // initializeMemory();
    clearPages();
    // cout << "READ FROM FILE" << endl;
    const std::size_t ChunkSize = BLOCK_SIZE;  // Define the chunk size. 4KB
    std::ifstream inFile(fileName,
                         std::ios::binary);  // Open the file for reading.

    if (!inFile) {
      std::cerr << "Cannot open file for reading: " << fileName << std::endl;
      return;
    }

    std::vector<unsigned char> buffer(ChunkSize);
    bool isRead = false;
    int pgCount = 0;

    unsigned char* pageBufferTmp =
        static_cast<unsigned char*>(std::malloc(BLOCK_SIZE));
    while (inFile.read(reinterpret_cast<char*>(buffer.data()), ChunkSize)) {
      // memcpy(pageBuffer[pgCount++], buffer.data(), ChunkSize);
      memcpy(pageBufferTmp, buffer.data(), ChunkSize);
      findValue(1, target, pageBufferTmp);
    }
    inFile.close();  // Close the file.
  }

  // Given an ID, find the relevant record and print it
  void findRecordById(int id) {
    cout << "Finding record with id: " << id << endl;
    readFromFile(id);
  }
};
