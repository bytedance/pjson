# Praveen's JSON

- An Ultra Simple JSON parser for C++.
- PJSON's intent is to allow a very simple way to create and access JSON data in C++.
- Apache license with credit given to the author :) "Praveen Babu J D"
---
# Building / Installation
## Option 1 : Direct inclusion
- The header file is in "pjsonlib/include/pjson.h"
- The Source file(s) is in "pjsonlib/src/*"
- You should be able to directly include the above files into you code.
  
## Option 2 : Make a library
- Make a library see: "pjsonlib/CMakeLists.txt"
- Include the "pjson.h" into your project
- Link "libpjson" lib into your executable or lib

# Usage
## Include the header file
```C++
#include "pjson.h"
using namespace ByteDance;
```

## Creating JSON
```C++
  pjson oJson;
  // To create a JSON map of key Value
  oJson["myKey1"] = "Value1";
  oJson["myKey2"] = "Value2";

  // To create a Nested JSON map of key Value
  oJson["myKey3"]["myInteger"] = 1;
  oJson["myKey3"]["myFloat"] = 1.0f;

  // Create an Array as a Value
  oJson["myKey4"] = std::vector<int>({0,1,2,3,4,5,6});

  // Direct array refernce
  oJson["myKey4"][7] = 7;
  oJson["myKey4"][8] = "Eight";

  // Deep Nesting Map->Array->Map->Value
  oJson["myKey4"][9]["ninth"] = 9.0f;

  //Simplified access
  pjson& rFloats = oJson["myKey3"]["myFloatArray"];
  rFloats = std::vector<float>({0,1.1});

  //Auto fills null values for rFloats[2] and rFloats[3] 
  rFloats[4] = 4.4f;

  // Get unformatted string
  std::string sResult = oJson.toString();
  std::cout<<"\n** Un-Formatted :\n"<<sResult;

  // Print formatted string
  std::cout<<"\n\n** Formatted :\n"<<oJson.toString(true);
```
Output:
- ** Un-Formatted :
```json
{ "myKey1" : "Value1" , "myKey2" : "Value2" , "myKey3" : { "myFloat" : 1.000000 , "myInteger" : 1 } , "myKey4" : [ 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , "Eight" ] }
```
- ** Formatted :
```json
{
 "myKey1" : "Value1" ,
 "myKey2" : "Value2" ,
 "myKey3" : {
             "myFloat" : 1.000000 ,
             "myInteger" : 1 
            } ,
 "myKey4" : [
             0 ,
             1 ,
             2 ,
             3 ,
             4 ,
             5 ,
             6 ,
             7 ,
             "Eight" 
            ] 
}
```

## Editing JSON
```C++
//Load JSON from another JSON string, returns null if it fails
std::string sJsonString = oJson.toString();
pjson* pResult = pjson::CreateFromString(sJsonString);

//Edit an exisiting Array
pjson& rAnotherFloats = (*pResult)["myKey3"]["myFloatArray"];
rAnotherFloats[0] = 33.3f;
rAnotherFloats[2] = "two";

//Change Array to Key Value
(*pResult)["myKey4"] = "four";

std::cout<<"\n\n** Edited Output:\n"<<pResult->toString(true);
delete pResult;
```
- ** Edited Output:
```json
{
 "myKey1" : "Value1" ,
 "myKey2" : "Value2" ,
 "myKey3" : {
             "myFloat" : 1.000000 ,
             "myFloatArray" : [
                               33.299999 ,
                               1.100000 ,
                               "two" ,
                               null ,
                               4.400000 
                              ] ,
             "myInteger" : 1 
            } ,
 "myKey4" : "four" 
}
```

## Raw Access 
```C++
//Load JSON from another JSON string, returns null if it fails
pjson* pResult = pjson::CreateFromString(oJson.toString());
//Edit an exisiting Array
pjson& rAnotherFloats = (*pResult)["myKey4"];
pjson::PJSONARRAY* pArray = rAnotherFloats.getArray();
std::cout<<"\n\n** Print Only the Integers :\n";
for(auto itr : *pArray) {
  if(itr->getType() == pjson::jsonNumberInt){
    std::cout<<" "<<itr->getInt();
  }
}
//Print only the child section
std::cout<<"\n\n** Print just the Sub Section :\n"<<rAnotherFloats.toString();

delete pResult;
```
```
** Print Only the Integers :
 0 1 2 3 4 5 6 7

** Print just the Sub Section :
[ 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , "Eight" , { "ninth" : 9.000000 } ]
```

## More
- See pjsontest/main.cpp for more ways to use this helpful code

---
# Running Test Application
- Test application is under the folder "pjsontest"
- Run CMake , Build on the above folder
- Execute pjsontest