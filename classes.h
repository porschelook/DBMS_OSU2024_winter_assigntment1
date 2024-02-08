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
    // // // cout << "\tID: " << id << "\n";
    // // // cout << "\tNAME: " << name << "\n";
    // // // cout << "\tBIO: " << bio << "\n";
    // // // cout << "\tMANAGER_ID: " << manager_id << "\n";
  }
};

class StorageBufferManager {
 private:
  const int BLOCK_SIZE = 4096;  // initialize the  block size allowed in main
                                // memory according to the question
  string fileName;
  int sumRage = 0;
  int sumRage2 = 0;
  const int MAX_PAGE = 3;
  // You may declare variables based on your need
  int numRecords = 0;

  // create 3 pages of memory with having 4096 bytes each
  // memory that store the variable-length records as unsigned byte array
  unsigned char* memory;
  int* lastRecord;
  int* recordCount;

  int saveBuffer = 0;
  int loadBuffer = 0;

  // static_cast<unsigned char*>(std::malloc(BLOCK_SIZE * 3));
  // Create a file to store the records
  std::ofstream file;
  std::ifstream fileReader;

  // create number of Position in each pages
  // create a current page number to show that what is the current page
  int currentPage = 1;  // page 0, 1, 2 due to 3 pages possible
  int currentLength = 0;

  // Insert new record
  void setNextPage() {
    currentPage++;
    currentLength = 0;
    memory = memory + BLOCK_SIZE;
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
    memory = static_cast<unsigned char*>(std::malloc(BLOCK_SIZE * 3));
    memset(memory, 0, BLOCK_SIZE * MAX_PAGE);
    setLastRecord();
  }

  void insertToMemory(std::vector<char> record) {
    // // // // cout << record.data() << endl;
    // count size of record
    size_t recordLen = record.size();
    // // // // cout << recordLen << endl;

    // save record
    memcpy(memory + currentLength, record.data(), recordLen);
    currentLength += recordLen;

    *recordCount += 1;
    // save the free space
    memcpy(memory + BLOCK_SIZE - intSize, &currentLength, intSize);
    // save the record count
    // update the last slot in page which is the free position
    memcpy(memory + BLOCK_SIZE - intSize, &currentLength, intSize);

    sumRage2 += recordLen;

    // // cout << "Current Page: " << currentPage
    //  << " Current Length: " << currentLength << " SumRange: " << sumRage2
    //  << " Record Count: " << *recordCount << endl;
  }

  void insertRecord(Record record) {
    if (numRecords == 0) {
      initializeMemory();
    }
    numRecords++;

    // Check if the current page is full

    // Serialize the record and insert record to memory
    std::vector<char> r = record.serializeToString();
    if (r.size() + currentLength >
        BLOCK_SIZE - sizeof(int) * (3 + (*recordCount))) {
      setNextPage();
    }

    if (currentPage > MAX_PAGE) {
      // write all memory to disk then reset memory and keep going
      writePageBufferToFile();
      clearPages();
    }

    insertToMemory(r);

    // Take neccessary steps if capacity is reached (you've utilized all the
    // blocks in main memory)
  }

  void clearPages() {
    // Clear the page
    // Set the page to 0
    memory = memory - BLOCK_SIZE * currentPage;
    currentPage = 1;
    initializeMemory();
  }

 public:
  StorageBufferManager(string NewFileName) {
    // initialize your variables
    fileName = NewFileName;
    // Create your EmployeeRelation file
    file = std::ofstream(NewFileName);
  }

  void writePageBufferToFile() {
    // cast memory to char* and write to file
    char* charMemory = reinterpret_cast<char*>(memory);

    file.write(charMemory, BLOCK_SIZE * 3);
  }

  // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
  void createFromFile(string csvFName) {
    // Add records to the EmployeeRelation
    std::ifstream csvFile(csvFName);
    // Read the file and add records to the EmployeeRelation
    if (!csvFile.is_open()) {
      return;
    }
    string line;
    while (std::getline(csvFile, line, '\n')) {
      // Split the line into fields
      vector<string> fields;
      stringstream ss(line);
      string field;
      while (std::getline(ss, field, ',')) {
        fields.push_back(field);
      }
      // Create a record from the fields
      Record record(fields);
      // Insert the record into the EmployeeRelation
      insertRecord(record);
    }

    // Write the page buffer to the file
    memory = memory - BLOCK_SIZE * currentPage;
    writePageBufferToFile();
    // Close the file
    csvFile.close();
    clearPages();
    // // // cout << "ALL Records: " << numRecords << "DATA = " << sumRage <<
    // endl;

    // test read
    // // // cout << endl;
    // // // cout << endl;
    // // // cout << endl;
    // // // cout << "TEST READ FILE" << endl;
    readFromFile();
    // // // cout << memory << endl;
  }

  void loopThoughMemory() {
    for (int i = 0; i < 3; i++) {
      cout << "Page: " << i + 1 << endl;
      // for (int j = 0; j < BLOCK_SIZE; j++) {
      // cout << memory[j] << "";
      // }
      int lastValueOfBlock = 0;
      memcpy(&lastValueOfBlock, memory + BLOCK_SIZE - intSize, intSize);
			cout << "Last Value: " << lastValueOfBlock << endl;
			memory = memory + BLOCK_SIZE;
    }
    cout << "====================" << endl;
  }

  void readFromFile() {
    const std::size_t ChunkSize =
        BLOCK_SIZE * 3;              // Define the chunk size. 4KB * 3 pages
    std::ifstream inFile(fileName);  // Open the file for reading.
    // // // cout << "File Name: " << fileName << " " << ChunkSize << endl;

    if (!inFile) {
      std::cerr << "Cannot open file for reading: " << fileName << std::endl;
      return;
    }

    std::vector<unsigned char> buffer(ChunkSize);
    int count = 1;

    while (inFile.read(reinterpret_cast<char*>(buffer.data()), ChunkSize) ||
           inFile.gcount() != 0) {
      // read file to memory
      memcpy(memory, buffer.data(), ChunkSize);
      // // // cout << "PRINT Memory:: " << memory << endl;
      loopThoughMemory();

      // // // cout << "Page: " << count++ << endl;

      // If you need to reset the buffer size because the last read was smaller
      // than CHUNK_SIZE and you plan to reuse the buffer, adjust its size
      // accordingly For the sake of demonstration, this step is not necessary
      // unless you have specific reasons for adjusting the buffer
    }
    // Close the file
    inFile.close();  // Close the file.
  }

  // Given an ID, find the relevant record and print it
  Record findRecordById(int id) {}
};
