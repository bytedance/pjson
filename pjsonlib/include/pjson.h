//
// Copyright 2025 ByteDance Ltd. and/or its affiliates. All rights reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//===----------------------------------------------------------------------===//
// Author: Praveen Babu J D
// License: Apache 2.0
//
#ifndef PRAVEENJSON_H
#define PRAVEENJSON_H

#include <vector>
//#include <unordered_map>
#include <map>
#include <string>

namespace ByteDance {
//==[Interface]============================================================
class pjson {
  public:
    enum jsonType : int64_t {
      jsonNull,
      jsonString,
      jsonNumberInt,
      jsonNumberFloat,
      jsonBoolean,
      jsonArray, //[ ] array
      jsonMap, // { ... } map
    };
    typedef std::vector<pjson*> PJSONARRAY;
    //typedef std::unordered_map<std::string, pjson*> PJSONMAP;
    typedef std::map<std::string, pjson*> PJSONMAP;

    pjson(); // Default Constructor
    ~pjson(); // Destructor
    pjson(const pjson& aFrom); // Copy Constructor
    pjson(pjson&& aFrom); // Move constructor
    pjson& operator=(const pjson& aFrom); // Copy assignment
    pjson& operator=(pjson&& a); // Move assignment

    static pjson* CreateFromString(const std::string& aStr);
    static pjson* CreateFromString(const char* aSrc, size_t a_iSize);

    jsonType getType() const;
    std::string toString(bool bPretty = false) const;
    void copyFrom(const pjson& aFrom);

    PJSONARRAY* getArray();
    PJSONMAP* getMap();
    float getFloat();
    int getInt();
    bool getBool();
    std::string getString();

    void reset();
    void resetTo(jsonType aeType);

    pjson& at(const std::string& aString);
    pjson& at(const char* aSkey);
    pjson& at(int index);

    //Assignment overload
    pjson& operator[] (const std::string& aString);
    pjson& operator[] (const char* aSkey);
    pjson& operator[] (int index);

    pjson& operator=(const std::string& aString);
    pjson& operator=(const char* aCString);
    pjson& operator=(const int aInt);
    pjson& operator=(const float aFloat);
    pjson& operator=(const bool aBool);

    pjson& operator=(const std::vector<std::string>& aValueArray);
    pjson& operator=(const std::vector<char*>& aValueArray);
    pjson& operator=(const std::vector<const char*>& aValueArray);
    pjson& operator=(const std::vector<int>& aValueArray);
    pjson& operator=(const std::vector<float>& aValueArray);
    pjson& operator=(const std::vector<bool>& aValueArray);

    pjson& operator+=(const std::string& aValue);
    pjson& operator+=(const char* aValue);
    pjson& operator+=(const int aValue);
    pjson& operator+=(const float aValue);
    pjson& operator+=(const bool aValue);

    pjson& operator+=(const std::vector<std::string>& aValueArray);
    pjson& operator+=(const std::vector<char*>& aValueArray);
    pjson& operator+=(const std::vector<const char*>& aValueArray);
    pjson& operator+=(const std::vector<int>& aValueArray);
    pjson& operator+=(const std::vector<float>& aValueArray);
    pjson& operator+=(const std::vector<bool>& aValueArray);

    bool getArrayValues(size_t aFrom, size_t aTo, std::vector<std::string>& aDest);
    bool getArrayValues(size_t aFrom, size_t aTo, std::vector<int>& aDest);
    bool getArrayValues(size_t aFrom, size_t aTo, std::vector<float>& aDest);
    bool getArrayValues(size_t aFrom, size_t aTo, std::vector<bool>& aDest);

  private:
    std::string _toString(int a_iIndent) const;
    void _resetIfneeded(jsonType aeType);
    static bool _CreateFromString(const char* aSrc, size_t& a_iStart, size_t a_iEnd, pjson*& a_rResult);

    static bool _ScanPastColon(const char* aSrc, size_t& a_iStart,const size_t a_iEnd);
    static bool _ExtractString(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, std::string& aStrResult);
    static bool _ScanString(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, pjson*& a_rStrResult);
    static bool _ScanBool(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, pjson*& a_rBoolResult);
    static bool _ScanToNext(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, char& a_rResult);
    static bool _ScanNull(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, pjson*& a_rNUllResult);
    static bool _ScanNumber(const char* aSrc, size_t& a_iStart, const size_t a_iEnd, pjson*& a_rNumResult);
    static bool _ScanArray(const char* aSrc, size_t& a_iStart, const size_t a_iEnd, pjson*& a_rAResult);
    static bool _ScanObject(const char* aSrc, size_t& a_iStart, const size_t a_iEnd, pjson*& a_rAResult);

  private:

    jsonType _eType = jsonType::jsonNull;
    union {
      void* _pValueRaw = nullptr;
      PJSONMAP* _pValueMap;
      PJSONARRAY* _pValueArray;
      int* _pValueInt;
      float* _pValueFloat;
      bool* _pValueBool;
      std::string* _pValueString;
      /* data */
    };
};
//========================================================================
};// end namespace ByteDance
#endif /* !PRAVEENJSON_H */