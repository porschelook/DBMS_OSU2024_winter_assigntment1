/*
Skeleton code for storage and buffer management
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;

#include <vector>

int main(int argc, char *const argv[])
{
  // Create the EmployeeRelation file from Employee.csv
  StorageBufferManager manager("EmployeeRelation");
  manager.createFromFile("Employee.csv");
  // Loop to lookup IDs until user is ready to quit

  // manager.findRecordById(11432140);
  // convert arguments to integers and loop though them to find the records
  while (true)
  {

    cout << "Enter Employee ID to look up (or enter -1 to quit): ";
    int employeeId;
    cin >> employeeId;

    if (employeeId == -1)
    {
      // User wants to quit
      break;
    }
    manager.findRecordById(employeeId);
    // Search for the given Employee ID
    // manager.findRecordById(employeeId);
  }

  // for (int i = 1; i < argc; i++) {
  //   cout << "--------------------------------" << endl;
  //   int id = atoi(argv[i]);

  // }

  return 0;
}
