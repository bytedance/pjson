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
#include "pjson.h"
using namespace ByteDance;

//-----------------------------------------------------------------
pjson::pjson()
        : _eType(jsonType::jsonNull)
        , _pValueRaw(nullptr)
{

}
//-----------------------------------------------------------------
pjson::~pjson() {
    reset();
}
//-----------------------------------------------------------------
pjson::pjson(const pjson& aFrom)
        : _eType(jsonType::jsonNull)
        , _pValueRaw(nullptr)
{
    copyFrom(aFrom);
}
//-----------------------------------------------------------------
// Move constructor
pjson::pjson(pjson&& aFrom) {
    _eType = aFrom._eType;
    _pValueRaw = aFrom._pValueRaw;
    aFrom._pValueRaw = nullptr;
    aFrom._eType = jsonType::jsonNull;
}
//-----------------------------------------------------------------
// Move assignment
pjson& pjson::operator=(pjson&& aFrom) {
    if (&aFrom == this)
        return *this;

    reset();
    _eType = aFrom._eType;
    _pValueRaw = aFrom._pValueRaw;

    aFrom._eType = jsonType::jsonNull;
    aFrom._pValueRaw = nullptr;

    return *this;
}
//-----------------------------------------------------------------
// Copy assignment
pjson& pjson::operator=(const pjson& aFrom) {
    if (&aFrom == this)
        return *this;

    //reset(); // CopyFrom will do a reset anyways
    copyFrom(aFrom);
    return *this;
}
//-----------------------------------------------------------------
pjson::jsonType pjson::getType() const {
    return _eType;
};
//-----------------------------------------------------------------
pjson::PJSONARRAY* pjson::getArray() {
    if(_eType == jsonType::jsonArray) {
        return _pValueArray;
    }
    return nullptr;
}
//-----------------------------------------------------------------
pjson::PJSONMAP* pjson::getMap() {
    if(_eType == jsonType::jsonMap) {
        return _pValueMap;
    }
    return nullptr;
}
//-----------------------------------------------------------------
float pjson::getFloat() {
    if(_eType == jsonType::jsonNumberInt) {
        return float(*_pValueInt);
    } else if(_eType == jsonType::jsonNumberFloat) {
        return *_pValueFloat;
    }
    return 0.0f;
}
//-----------------------------------------------------------------
int pjson::getInt() {
    if(_eType == jsonType::jsonNumberInt) {
        return *_pValueInt;
    } else if(_eType == jsonType::jsonNumberFloat) {
        return int(*_pValueFloat);
    }
    return 0;
}
//-----------------------------------------------------------------
bool pjson::getBool() {
    switch (_eType) {
        case jsonType::jsonNull: {
            return false;
        }
        case jsonType::jsonString: {
            return (_pValueString->length() > 0);
        }
        case jsonType::jsonNumberInt: {
            return bool(*_pValueInt);
        }
        case jsonType::jsonNumberFloat: {
            return bool(*_pValueFloat);
        }
        case jsonType::jsonBoolean: {
            return (*_pValueBool);
        }
            break;
        case jsonType::jsonArray:
        case jsonType::jsonMap:
        default:
            return false;
            break;
    }
    return false;
}
//-----------------------------------------------------------------
std::string pjson::getString() {
    return (_eType == jsonType::jsonString)? (*_pValueString): "";
}
//-----------------------------------------------------------------
void pjson::reset() {
    resetTo(jsonType::jsonNull);
}
//-----------------------------------------------------------------
void pjson::_resetIfneeded(jsonType aeType) {
    if(_eType != aeType) {
        resetTo(aeType);
    }
}
//-----------------------------------------------------------------
void pjson::resetTo(pjson::jsonType aeType) {
    switch(_eType) {
        case jsonType::jsonNull:         { _pValueRaw = nullptr; break; }
        case jsonType::jsonString:       { delete _pValueString;  break; }
        case jsonType::jsonNumberInt:    { delete _pValueInt;     break; }
        case jsonType::jsonNumberFloat:  { delete _pValueFloat;   break; }
        case jsonType::jsonBoolean:      { delete _pValueBool;    break; }
        case jsonType::jsonArray:  {
            for(pjson* pj : *_pValueArray) {
                delete pj;
            }
            delete _pValueArray;
            break;
        }
        case jsonType::jsonMap: {
            for (const auto& kv : *_pValueMap) {
                delete kv.second;
            }
            delete _pValueMap;
            break;
        }
    }//end switch
    _pValueRaw = nullptr;

    switch(aeType) {
        case jsonType::jsonNull:         { /* _pValueRaw = nullptr; */ break; }
        case jsonType::jsonString:       { _pValueString = new std::string; break; }
        case jsonType::jsonNumberInt:    { _pValueInt = new int; break; }
        case jsonType::jsonNumberFloat:  { _pValueFloat = new float; break; }
        case jsonType::jsonBoolean:      { _pValueBool = new bool; break; }
        case jsonType::jsonArray:        { _pValueArray = new PJSONARRAY; break; }
        case jsonType::jsonMap:       { _pValueMap = new PJSONMAP; break; }
    } //end switch
    _eType = aeType;
}
//-----------------------------------------------------------------
void pjson::copyFrom(const pjson& aFrom) {
    resetTo(aFrom.getType());

    switch(_eType) {
        case jsonType::jsonNull:         { /* _pValueRaw = nullptr; */ break; }
        case jsonType::jsonString:       { *_pValueString = *(aFrom._pValueString); break; }
        case jsonType::jsonNumberInt:    { *_pValueInt = *(aFrom._pValueInt); break; }
        case jsonType::jsonNumberFloat:  { *_pValueFloat = *(aFrom._pValueFloat); break; }
        case jsonType::jsonBoolean:      { *_pValueBool = *(aFrom._pValueBool); break; }
        case jsonType::jsonArray:        {
            for (auto it : *(aFrom._pValueArray)) {
                pjson* pObj = new pjson();
                pObj->copyFrom(*it);
                _pValueArray->push_back(pObj);
            }
            break;
        }
        case jsonType::jsonMap:       {
            for (auto const& it : *(aFrom._pValueMap)) {
                pjson* pObj = new pjson();
                pObj->copyFrom(*(it.second));
                (*_pValueMap)[it.first] = pObj;
            }
            break;
        }
    } //end switch
}
//-----------------------------------------------------------------
std::string pjson::toString(bool bPretty /*=false*/) const {
    int iIndent = bPretty?0:-1;
    return _toString(iIndent);
}
//-----------------------------------------------------------------
std::string pjson::_toString(int a_iIndent) const {
    std::string sOut;

    switch(_eType) {
        case jsonType::jsonNull:         { sOut += "null"; break; }
        case jsonType::jsonString:       { sOut += "\"" + (*_pValueString) + "\""; break; }
        case jsonType::jsonNumberInt:    { sOut += std::to_string(*_pValueInt); break; }
        case jsonType::jsonNumberFloat:  { sOut += std::to_string(*_pValueFloat); break; }
        case jsonType::jsonBoolean:      { sOut += (*_pValueBool)?"true":"false"; break; }
        case jsonType::jsonArray:  {
            sOut += "[";
            std::string spaces;
            if(a_iIndent >=0) {
                spaces = std::string(a_iIndent, ' ');
            }

            bool bFirstElement = true;
            for (auto it = _pValueArray->begin(); it != _pValueArray->end(); it++) {
                if(!bFirstElement) {
                    sOut += ",";
                }

                int iIndent = a_iIndent;
                if(a_iIndent >=0) {
                    sOut += "\n" + spaces;
                    iIndent += 1;
                }

                sOut += " ";
                sOut += (*it)->_toString(iIndent);
                sOut += " ";
                bFirstElement = false;
            }

            if(bFirstElement) {
                sOut += " ]"; // no elements
            } else {
                if(a_iIndent >=0) {
                    sOut += "\n" + spaces;
                }
                sOut += "]";
            }
            break;
        }
        case jsonType::jsonMap: {
            sOut += "{";
            std::string spaces;
            if(a_iIndent >=0) {
                spaces = std::string(a_iIndent, ' ');
            }
            bool bFirstElement = true;
            for (auto it = _pValueMap->begin(); it != _pValueMap->end(); it++) {
                if(!bFirstElement) {
                    sOut += ",";
                }

                if(a_iIndent >=0) {
                    sOut += "\n" + spaces;
                }
                sOut += " \"";
                int iLen = it->first.length();
                sOut += it->first;
                sOut += "\" : ";

                int iIndent = a_iIndent;
                if(a_iIndent >=0) {
                    iIndent += iLen + 6;
                }
                sOut += it->second->_toString(iIndent);
                sOut += " ";
                bFirstElement = false;
            } // end for

            if(bFirstElement) {
                sOut += " }"; // no elements
            } else {
                if(a_iIndent >=0) {
                    sOut += "\n" + spaces;
                }
                sOut += "}";
            }
            break;
        }
    }//end switch

    return sOut;
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::string& aString) {
    _resetIfneeded(jsonType::jsonString);
    *_pValueString = aString;
    return *this;
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const char* aCString) {
    _resetIfneeded(jsonType::jsonString);
    *_pValueString = aCString;
    return *this;
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const int aInt) {
    _resetIfneeded(jsonType::jsonNumberInt);
    *_pValueInt = aInt;
    return *this;
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const float aFloat) {
    _resetIfneeded(jsonType::jsonNumberFloat);
    *_pValueFloat = aFloat;
    return *this;
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const bool aBool) {
    _resetIfneeded(jsonType::jsonBoolean);
    *_pValueBool = aBool;
    return *this;
}
//-----------------------------------------------------------------
#define PJSON_VALUE_ARRAY_SET_ITERATOR      \
          resetTo(jsonType::jsonArray);     \
          for(auto i : aValueArray){        \
            pjson* pTemp = new pjson();     \
            *pTemp = i;                     \
            _pValueArray->push_back(pTemp); \
          }                                 \
          return *this;
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::vector<char*>& aValueArray) {
    PJSON_VALUE_ARRAY_SET_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::vector<const char*>& aValueArray) {
    PJSON_VALUE_ARRAY_SET_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::vector<int>& aValueArray) {
    PJSON_VALUE_ARRAY_SET_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::vector<float>& aValueArray) {
    PJSON_VALUE_ARRAY_SET_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::vector<bool>& aValueArray) {
    PJSON_VALUE_ARRAY_SET_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator=(const std::vector<std::string>& aValueArray) {
    PJSON_VALUE_ARRAY_SET_ITERATOR
}
//-----------------------------------------------------------------
#define PJSON_VALUE_ARRAY_APPEND_ITERATOR       \
          _resetIfneeded(jsonType::jsonArray);  \
          pjson* pTemp = new pjson();           \
          *pTemp = aValue;                      \
          _pValueArray->push_back(pTemp);       \
          return *this;
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::string& aValue) {
    PJSON_VALUE_ARRAY_APPEND_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const char* aValue) {
    PJSON_VALUE_ARRAY_APPEND_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const int aValue) {
    PJSON_VALUE_ARRAY_APPEND_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const float aValue) {
    PJSON_VALUE_ARRAY_APPEND_ITERATOR
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const bool aValue) {
    PJSON_VALUE_ARRAY_APPEND_ITERATOR
}
//-----------------------------------------------------------------
#define PJSON_VALUE_ARRAY_APPEND_ARRAY            \
          _resetIfneeded(jsonType::jsonArray);    \
          for(auto i : aValueArray){              \
            pjson* pTemp = new pjson();           \
            *pTemp = i;                           \
            _pValueArray->push_back(pTemp);       \
          }                                       \
          return *this;
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::vector<int>& aValueArray) {
    PJSON_VALUE_ARRAY_APPEND_ARRAY
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::vector<char*>& aValueArray) {
    PJSON_VALUE_ARRAY_APPEND_ARRAY
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::vector<const char*>& aValueArray) {
    PJSON_VALUE_ARRAY_APPEND_ARRAY
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::vector<float>& aValueArray) {
    PJSON_VALUE_ARRAY_APPEND_ARRAY
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::vector<bool>& aValueArray) {
    PJSON_VALUE_ARRAY_APPEND_ARRAY
}
//-----------------------------------------------------------------
pjson& pjson::operator+=(const std::vector<std::string>& aValueArray) {
    PJSON_VALUE_ARRAY_APPEND_ARRAY
}
//-----------------------------------------------------------------
#define PJSON_VALUE_ARRAY_EXTRACT(pjsonfuncname)              \
  if(_eType != jsonType::jsonArray                            \
    || aTo>=_pValueArray->size()                              \
    || aFrom >=_pValueArray->size()                           \
    || aFrom>aTo) {                                           \
      return false;                                           \
    }                                                         \
  for(size_t i = aFrom; i<=aTo; ++i) {                        \
    aDest.push_back((*_pValueArray)[i]->pjsonfuncname()); \
  }                                                           \
  return true;
//-----------------------------------------------------------------
bool pjson::getArrayValues(size_t aFrom, size_t aTo, std::vector<std::string>& aDest) {
    PJSON_VALUE_ARRAY_EXTRACT(getString)
}
//-----------------------------------------------------------------
bool pjson::getArrayValues(size_t aFrom, size_t aTo, std::vector<int>& aDest) {
    PJSON_VALUE_ARRAY_EXTRACT(getInt)
}
//-----------------------------------------------------------------
bool pjson::getArrayValues(size_t aFrom, size_t aTo, std::vector<float>& aDest){
    PJSON_VALUE_ARRAY_EXTRACT(getFloat)
}
//-----------------------------------------------------------------
bool pjson::getArrayValues(size_t aFrom, size_t aTo, std::vector<bool>& aDest){
    PJSON_VALUE_ARRAY_EXTRACT(getBool)
}
//-----------------------------------------------------------------
pjson& pjson::at(const std::string& aString) {
    const char* cStr = aString.c_str();
    _resetIfneeded(jsonType::jsonMap);
    PJSONMAP::iterator it = _pValueMap->find(cStr);
    if(it != _pValueMap->end()) {
        return *((*_pValueMap)[cStr]);
    }
    pjson* pNew = new pjson();
    (*_pValueMap)[cStr] = pNew;
    return *pNew;
}
//-----------------------------------------------------------------
pjson& pjson::at(const char* aSkey) {
    _resetIfneeded(jsonType::jsonMap);
    PJSONMAP::iterator it = _pValueMap->find(aSkey);
    if(it != _pValueMap->end()) {
        return *((*_pValueMap)[aSkey]);
    }
    pjson* pNew = new pjson();
    (*_pValueMap)[aSkey] = pNew;
    return *pNew;
}
//-----------------------------------------------------------------
pjson& pjson::at(int index) {
    _resetIfneeded(jsonType::jsonArray);
    int iSize = static_cast<int>(_pValueArray->size());
    if(index < 0) {
        if(iSize > 0) {
            index = (iSize - (index % iSize)) % iSize;
        } else {
            index = 0;
        }
    }

    if(index >= iSize) {
        int iAdd = 1 + index - iSize;
        for(int i=iAdd;i--;) {
            _pValueArray->push_back(new pjson());
        }
    }

    return *(*_pValueArray)[static_cast<size_t>(index)];
}
//-----------------------------------------------------------------
pjson& pjson::operator[] (int index) {
    return at(index);
}
//-----------------------------------------------------------------
pjson& pjson::operator[] (const std::string& aString) {
    return at(aString);
}
//-----------------------------------------------------------------
pjson& pjson::operator[] (const char* aSkey) {
    return at(aSkey);
}
//-----------------------------------------------------------------
/*static*/
pjson* pjson::CreateFromString(const std::string& aStr) {
    return CreateFromString(aStr.c_str(), aStr.length());
}
//-----------------------------------------------------------------
/*static*/
pjson* pjson::CreateFromString(const char* aSrc, size_t a_iSize) {
    size_t iStart =0;
    size_t iEnd =a_iSize;
    pjson* pResult = nullptr;
    /*bool bSuccess = */
    _CreateFromString(aSrc, iStart, iEnd, pResult);
    return pResult;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_CreateFromString(const char* aSrc, size_t& a_iStart,size_t a_iEnd, pjson*& a_rResult) {
    //1. Scan for fundametal type
    char aChar;
    while (_ScanToNext(aSrc, a_iStart, a_iEnd, aChar)) {
        aChar = tolower(aChar);
        if('\"' == aChar) {
            return _ScanString(aSrc, a_iStart, a_iEnd, a_rResult);
        }
        else if('n' == aChar) {
            return _ScanNull(aSrc, a_iStart, a_iEnd, a_rResult);
        }
        else if('t' == aChar || 'f' == aChar) {
            return _ScanBool(aSrc, a_iStart, a_iEnd, a_rResult);
        }
        else if('+' == aChar || '-' == aChar || '.' == aChar || ('0' <= aChar && '9' >= aChar)) {
            return _ScanNumber(aSrc, a_iStart, a_iEnd, a_rResult);
        } else if('{' == aChar) {
            return _ScanObject(aSrc, a_iStart, a_iEnd, a_rResult);
        } else if('[' == aChar) {
            return _ScanArray(aSrc, a_iStart, a_iEnd, a_rResult);
        } else {
            //unknown
            break;
        }
    } // end while

    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanBool(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, pjson*& a_rBoolResult) {
    if((a_iEnd - a_iStart) >= 4
       && 't' == tolower(aSrc[a_iStart])
       && 'r' == tolower(aSrc[a_iStart+1])
       && 'u' == tolower(aSrc[a_iStart+2])
       && 'e' == tolower(aSrc[a_iStart+3])) {
        a_rBoolResult = new pjson();
        *a_rBoolResult = bool(true);
        a_iStart+=4;
        return true;
    }
    if((a_iEnd - a_iStart) >= 5
       && 'f' == tolower(aSrc[a_iStart])
       && 'a' == tolower(aSrc[a_iStart+1])
       && 'l' == tolower(aSrc[a_iStart+2])
       && 's' == tolower(aSrc[a_iStart+3])
       && 'e' == tolower(aSrc[a_iStart+4])) {
        a_rBoolResult = new pjson();
        *a_rBoolResult = bool(false);
        a_iStart+=5;
        return true;
    }
    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ExtractString(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, std::string& aStrResult) {
    if('\"' != aSrc[a_iStart]){
        return false;
    }
    ++a_iStart;
    int iEnd = a_iStart;
    int iStart = a_iStart;
    while(a_iStart<a_iEnd) {
        char c = aSrc[a_iStart++];
        if('\"' == c) {
            iEnd = a_iStart - 1;
            break;
        } else if('\\' == c) {
            ++a_iStart;
        }
    }
    if(iStart<iEnd && iStart >=0 && iEnd >=0) {
        aStrResult = std::string(aSrc+iStart, iEnd-iStart);
        return true;
    }
    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanString(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, pjson*& a_rStrResult) {
    std::string str;
    if(_ExtractString(aSrc, a_iStart, a_iEnd, str)) {
        a_rStrResult = new pjson();
        *a_rStrResult = str;
        return true;
    }
    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanPastColon(const char* aSrc, size_t& a_iStart,const size_t a_iEnd) {
    while(a_iStart<a_iEnd) {
        char c = aSrc[a_iStart++];
        if(':' == c) {
            return true;
        } else if(' ' == c || '\t' == c || '\0' == c || '\n' == c) {
            //ignore
        } else {
            break;
        }
    }

    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanToNext(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, char& a_rResult) {
    while(a_iStart<a_iEnd) {
        a_rResult = aSrc[a_iStart];
        if(' ' == a_rResult || '\t' == a_rResult || '\0' == a_rResult || '\n' == a_rResult) {
            ++a_iStart;
        } else {
            return true;
        }
    }

    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanNull(const char* aSrc, size_t& a_iStart,const size_t a_iEnd, pjson*& a_rNUllResult) {
    if((a_iEnd - a_iStart) >= 4
       && 'n' == tolower(aSrc[a_iStart])
       && 'u' == tolower(aSrc[a_iStart+1])
       && 'l' == tolower(aSrc[a_iStart+2])
       && 'l' == tolower(aSrc[a_iStart+3])
            ) {
        a_rNUllResult = new pjson();
        a_iStart+=4;
        return true;
    }
    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanNumber(const char* aSrc, size_t& a_iStart, const size_t a_iEnd, pjson*& a_rNumResult) {
    bool bFloat = false;
    int iEnd = a_iStart;
    enum NumberSection : int {
        NumberSectionSign = 0,
        NumberSectionDigit,
        NumberSectionDot,
        NumberSectionDecimal,
        NumberSectionExponential,
        NumberSectionExponentialSign,
        NumberSectionExponentialDigit,
        NumberEnd
    };
    int eSec = NumberSectionSign;

    for(int i = a_iStart; i<a_iEnd && eSec!=NumberEnd; ) {
        switch ((NumberSection)eSec) {
            case NumberSectionSign: {
                if('+' == aSrc[i] || '-' == aSrc[i]) {
                    iEnd = ++i;
                }
                ++eSec;
                break;
            }
            case NumberSectionDigit: {
                if('0' <= aSrc[i] && '9' >= aSrc[i]) {
                    iEnd = ++i;
                } else {
                    ++eSec;
                }
                break;
            }
            case NumberSectionDot: {
                if('.' == aSrc[i]) {
                    iEnd = ++i;
                    bFloat = true;
                }
                ++eSec;
                break;
            }
            case NumberSectionDecimal: {
                if('0' <= aSrc[i] && '9' >= aSrc[i]) {
                    iEnd = ++i;
                    bFloat = true;
                } else {
                    ++eSec;
                }
                break;
            }
            case NumberSectionExponential: {
                if('e' == tolower(aSrc[i])) {
                    iEnd = ++i;
                    bFloat = true;
                }
                ++eSec;
                break;
            }
            case NumberSectionExponentialSign: {
                if('+' == aSrc[i] || '-' == aSrc[i]) {
                    iEnd = ++i;
                    bFloat = true;
                }
                ++eSec;
                break;
            }
            case NumberSectionExponentialDigit: {
                if('0' <= aSrc[i] && '9' >= aSrc[i]) {
                    iEnd = ++i;
                    bFloat = true;
                } else {
                    ++eSec;
                }
                break;
            }
            case NumberEnd: {
                break;
            }
        } // end switch
    } //end for

    if(iEnd > a_iStart) {
        std::string sTemp = std::string(aSrc+a_iStart, iEnd-a_iStart);
        a_rNumResult = new pjson();
        if(bFloat) {
            *a_rNumResult = std::stof(sTemp);
        } else {
            *a_rNumResult = std::stoi(sTemp);
        }
        a_iStart = iEnd;
        return true;
    }
    return false;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanArray(const char* aSrc, size_t& a_iStart, const size_t a_iEnd, pjson*& a_rAResult) {
    a_rAResult = new pjson();
    a_rAResult->resetTo(jsonType::jsonArray);
    bool bValid = false;
    ++a_iStart; // ignore first char "["
    char aChar;
    while(_ScanToNext(aSrc, a_iStart, a_iEnd, aChar)) {
        if(']' == aChar) {
            ++a_iStart;
            bValid = true;
            break;
        } else if(',' == aChar) {
            ++a_iStart; //ignore commas
        } else {
            pjson* pTemp = nullptr;
            if(_CreateFromString(aSrc, a_iStart,a_iEnd, pTemp)) {
                a_rAResult->_pValueArray->push_back(pTemp);
            } else {
                break;
            }
        }
    }
    if(!bValid) {
        delete a_rAResult;
        a_rAResult = nullptr;
    }

    return bValid;
}
//-----------------------------------------------------------------
/*static*/
bool pjson::_ScanObject(const char* aSrc, size_t& a_iStart, const size_t a_iEnd, pjson*& a_rAResult) {
    a_rAResult = new pjson();
    a_rAResult->resetTo(jsonType::jsonMap);
    bool bValid = false;
    ++a_iStart; // ignore first char "{"
    char aChar;
    while(_ScanToNext(aSrc, a_iStart, a_iEnd, aChar)) {
        if('}' == aChar) {
            ++a_iStart;
            bValid = true;
            break;
        } else if(',' == aChar) {
            ++a_iStart; // ignore commas
        } else {
            pjson* pVal = nullptr;
            std::string mkey;
            if(_ExtractString(aSrc, a_iStart, a_iEnd, mkey)
               && _ScanPastColon(aSrc, a_iStart, a_iEnd)
               && _CreateFromString(aSrc, a_iStart, a_iEnd, pVal)) {
                //success
                (*(a_rAResult->_pValueMap))[mkey.c_str()] = pVal;
            } else {
                delete a_rAResult;
                a_rAResult = nullptr;
                bValid = false;
                break;
            }
        }
    }
    return bValid;
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, float& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, int& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, bool& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, std::string& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, std::vector<std::string>& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, std::vector<int>& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, std::vector<float>& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const std::string& aKey, std::vector<bool>& a_rResult) {
    return getIfExist(aKey.c_str(), a_rResult);
}
//-----------------------------------------------------------------
#define PJSON_VALUE_EXTRACT_IF_EXISTS(pjsontype,getfunc)                         \
if(_eType == jsonType::jsonMap) {                                                \
    auto it = _pValueMap->find(aKey);                                            \
    if (it != _pValueMap->end() && it->second->getType()==jsonType::pjsontype) { \
        a_rResult = it->second->getfunc();                                       \
        return true;                                                             \
    }                                                                            \
}                                                                                \
return false;                                                                    \
//
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, float& a_rResult) {
    if(_eType == jsonType::jsonMap) {
        auto it = _pValueMap->find(aKey);
        if (it != _pValueMap->end()) {
            if(it->second->getType()==jsonType::jsonNumberFloat ||
                it->second->getType()==jsonType::jsonNumberInt) {
                a_rResult = it->second->getFloat();
                return true;
            }
        }
    }
    return false;
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, int& a_rResult) {
    PJSON_VALUE_EXTRACT_IF_EXISTS(jsonNumberInt, getInt)
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, bool& a_rResult) {
    PJSON_VALUE_EXTRACT_IF_EXISTS(jsonBoolean, getBool)
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, std::string& a_rResult) {
    PJSON_VALUE_EXTRACT_IF_EXISTS(jsonString, getString)
}
//-----------------------------------------------------------------
#define PJSON_ARRAY_VALUE_EXTRACT_IF_EXISTS                                     \
if (_eType == jsonType::jsonMap) {                                              \
    auto it = _pValueMap->find(aKey);                                           \
    if (it != _pValueMap->end() && it->second->getType()==jsonType::jsonArray) {\
        size_t arraylen = it->second->getArray()->size();                       \
        a_rResult.clear();                                                      \
        it->second->getArrayValues(0,arraylen-1,a_rResult);                     \
        return true;                                                            \
    }                                                                           \
}                                                                               \
return false;                                                                   \
//
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, std::vector<std::string>& a_rResult) {
    PJSON_ARRAY_VALUE_EXTRACT_IF_EXISTS
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, std::vector<int>& a_rResult) {
    PJSON_ARRAY_VALUE_EXTRACT_IF_EXISTS
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, std::vector<float>& a_rResult) {
    PJSON_ARRAY_VALUE_EXTRACT_IF_EXISTS
}
//-----------------------------------------------------------------
bool pjson::getIfExist(const char* aKey, std::vector<bool>& a_rResult) {
    PJSON_ARRAY_VALUE_EXTRACT_IF_EXISTS
}
//-----------------------------------------------------------------
bool pjson::hasKey(const std::string& aKey) {
    return hasKey(aKey.c_str());
}
//-----------------------------------------------------------------
bool pjson::hasKey(const char* cStr) {
    if(_eType == jsonType::jsonMap) {
        auto it = _pValueMap->find(cStr);
        return (it != _pValueMap->end());
    }
    return false;
}
//-----------------------------------------------------------------
// JSON String Encoding Function
//
// I'll create a C++ function that encodes a data buffer into a 
// string that can be safely embedded in a JSON file. This function
// will handle escaping special characters and converting binary 
// data to a safe representation.
//
// Here's the implementation:
//-----------------------------------------------------------------
/*static*/
std::string pjson::EncodeForJSON(const char* data, size_t length) {
    std::string result;
    result.reserve(length * 2); // Reserve space to avoid frequent reallocations
    
    for (size_t i = 0; i < length; ++i) {
        char c = data[i];
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '/':  result += "\\/"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                // Handle control characters and non-ASCII characters
                if (static_cast<unsigned char>(c) < 32 || static_cast<unsigned char>(c) >= 128) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    result += buf;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

//-----------------------------------------------------------------
/*static*/
std::string pjson::EncodeBase64ForJSON(const char* data, size_t length) {
    static const char base64_chars[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string result;
    result.reserve((length + 2) / 3 * 4); // Reserve space for base64 output
    
    size_t i = 0;
    while (i + 2 < length) {
        // Process 3 bytes at a time
        uint32_t triplet = (static_cast<unsigned char>(data[i]) << 16) |
                          (static_cast<unsigned char>(data[i+1]) << 8) |
                          (static_cast<unsigned char>(data[i+2]));
        
        // Convert to 4 characters
        result += base64_chars[(triplet >> 18) & 0x3F];
        result += base64_chars[(triplet >> 12) & 0x3F];
        result += base64_chars[(triplet >> 6) & 0x3F];
        result += base64_chars[triplet & 0x3F];
        
        i += 3;
    }
    
    // Handle remaining bytes
    if (i + 1 == length) {
        // 1 byte remaining
        uint32_t pair = static_cast<unsigned char>(data[i]) << 8;
        result += base64_chars[(pair >> 10) & 0x3F];
        result += base64_chars[(pair >> 4) & 0x3F];
        result += '='; // Padding
        result += '='; // Padding
    } else if (i + 2 == length) {
        // 2 bytes remaining
        uint32_t triplet = (static_cast<unsigned char>(data[i]) << 16) |
                          (static_cast<unsigned char>(data[i+1]) << 8);
        result += base64_chars[(triplet >> 18) & 0x3F];
        result += base64_chars[(triplet >> 12) & 0x3F];
        result += base64_chars[(triplet >> 6) & 0x3F];
        result += '='; // Padding
    }
    
    return result;
}
//-----------------------------------------------------------------
/*static*/
std::string pjson::DecodeFromJSON(const std::string& jsonStr) {
    std::string result;
    result.reserve(jsonStr.length()); // Reserve space to avoid frequent reallocations
    
    for (size_t i = 0; i < jsonStr.length(); ++i) {
        if (jsonStr[i] == '\\' && i + 1 < jsonStr.length()) {
            char c = jsonStr[++i];
            switch (c) {
                case '\"': result += '\"'; break;
                case '\\': result += '\\'; break;
                case '/':  result += '/';  break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case 'u':  // Handle Unicode escape sequence
                    if (i + 4 < jsonStr.length()) {
                        // Parse the 4 hex digits
                        std::string hexStr = jsonStr.substr(i + 1, 4);
                        try {
                            int hexValue = std::stoi(hexStr, nullptr, 16);
                            // For simplicity, we're only handling single-byte characters
                            result += static_cast<char>(hexValue);
                        } catch (...) {
                            // If parsing fails, just add the original sequence
                            result += "\\u" + hexStr;
                        }
                        i += 4;
                    }
                    break;
                default:
                    // Unknown escape sequence, just add the character
                    result += c;
                    break;
            }
        } else {
            result += jsonStr[i];
        }
    }
    
    return result;
}

//-----------------------------------------------------------------
/*static*/
std::string pjson::DecodeBase64FromJSON(const std::string& base64Str) {
    static const unsigned char base64_index[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
        0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    std::string result;
    result.reserve(base64Str.length() * 3 / 4); // Reserve space for decoded output
    
    size_t i = 0;
    while (i < base64Str.length()) {
        // Skip non-base64 characters
        if (base64Str[i] == '=' || base64_index[static_cast<unsigned char>(base64Str[i])] == 0) {
            i++;
            continue;
        }
        
        // Ensure we have enough characters for a complete group
        if (i + 1 >= base64Str.length()) break;
        
        // Decode a group of 4 characters into 3 bytes
        unsigned char a = base64_index[static_cast<unsigned char>(base64Str[i])];
        unsigned char b = base64_index[static_cast<unsigned char>(base64Str[i+1])];
        
        result += static_cast<char>((a << 2) | (b >> 4));
        
        if (i + 2 < base64Str.length() && base64Str[i+2] != '=') {
            unsigned char c = base64_index[static_cast<unsigned char>(base64Str[i+2])];
            result += static_cast<char>(((b & 0x0F) << 4) | (c >> 2));
            
            if (i + 3 < base64Str.length() && base64Str[i+3] != '=') {
                unsigned char d = base64_index[static_cast<unsigned char>(base64Str[i+3])];
                result += static_cast<char>(((c & 0x03) << 6) | d);
            }
        }
        
        i += 4;
    }
    
    return result;
}
//-----------------------------------------------------------------