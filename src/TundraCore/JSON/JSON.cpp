// For conditions of distribution and use, see copyright notice in License.txt

#include "StableHeaders.h"
#include "JSON.h"

#include <StringUtils.h>

#include <cstdio>
#include <cstdlib>

using namespace Urho3D;

namespace Tundra
{

const JSONValue JSONValue::EMPTY;
const JSONArray JSONValue::emptyJSONArray;
const JSONObject JSONValue::emptyJSONObject;

JSONValue::JSONValue() :
    type(JSON_NULL)
{
}

JSONValue::JSONValue(const JSONValue& value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(bool value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(int value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(unsigned value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(float value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(double value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(const String& value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(const char* value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(const JSONArray& value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::JSONValue(const JSONObject& value) :
    type(JSON_NULL)
{
    *this = value;
}

JSONValue::~JSONValue()
{
    SetType(JSON_NULL);
}

JSONValue& JSONValue::operator = (const JSONValue& rhs)
{
    SetType(rhs.type);
    
    switch (type)
    {
    case JSON_BOOL:
        data.boolValue = rhs.data.boolValue;
        break;
        
    case JSON_NUMBER:
        data.numberValue = rhs.data.numberValue;
        break;
        
    case JSON_STRING:
        *(reinterpret_cast<String*>(&data)) = *(reinterpret_cast<const String*>(&rhs.data));
        break;
        
    case JSON_ARRAY:
        *(reinterpret_cast<JSONArray*>(&data)) = *(reinterpret_cast<const JSONArray*>(&rhs.data));
        break;
        
    case JSON_OBJECT:
        *(reinterpret_cast<JSONObject*>(&data)) = *(reinterpret_cast<const JSONObject*>(&rhs.data));
        break;
        
    default:
        break;
    }
    
    return *this;
}

JSONValue& JSONValue::operator = (bool rhs)
{
    SetType(JSON_BOOL);
    data.boolValue = rhs;
    return *this;
}

JSONValue& JSONValue::operator = (int rhs)
{
    SetType(JSON_NUMBER);
    data.numberValue = (double)rhs;
    return *this;
}

JSONValue& JSONValue::operator = (unsigned rhs)
{
    SetType(JSON_NUMBER);
    data.numberValue = (double)rhs;
    return *this;
}

JSONValue& JSONValue::operator = (float rhs)
{
    SetType(JSON_NUMBER);
    data.numberValue = (double)rhs;
    return *this;
}

JSONValue& JSONValue::operator = (double rhs)
{
    SetType(JSON_NUMBER);
    data.numberValue = rhs;
    return *this;
}

JSONValue& JSONValue::operator = (const String& value)
{
    SetType(JSON_STRING);
    *(reinterpret_cast<String*>(&data)) = value;
    return *this;
}

JSONValue& JSONValue::operator = (const char* value)
{
    SetType(JSON_STRING);
    *(reinterpret_cast<String*>(&data)) = value;
    return *this;
}

JSONValue& JSONValue::operator = (const JSONArray& value)
{
    SetType(JSON_ARRAY);
    *(reinterpret_cast<JSONArray*>(&data)) = value;
    return *this;
}

JSONValue& JSONValue::operator = (const JSONObject& value)
{
    SetType(JSON_OBJECT);
    *(reinterpret_cast<JSONObject*>(&data)) = value;
    return *this;
}

JSONValue& JSONValue::operator [] (uint index)
{
    if (type != JSON_ARRAY)
        SetType(JSON_ARRAY);
    
    return (*(reinterpret_cast<JSONArray*>(&data)))[index];
}

const JSONValue& JSONValue::operator [] (uint index) const
{
    if (type == JSON_OBJECT)
        return (*(reinterpret_cast<const JSONArray*>(&data)))[index];
    else
        return EMPTY;
}

JSONValue& JSONValue::operator [] (const String& key)
{
    if (type != JSON_OBJECT)
        SetType(JSON_OBJECT);
    
    return (*(reinterpret_cast<JSONObject*>(&data)))[key];
}

const JSONValue& JSONValue::operator [] (const String& key) const
{
    if (type == JSON_OBJECT)
    {
        const JSONObject& object = *(reinterpret_cast<const JSONObject*>(&data));
        JSONObject::ConstIterator it = object.Find(key);
        return it != object.End() ? it->second_ : EMPTY;
    }
    else
        return EMPTY;
}

bool JSONValue::operator == (const JSONValue& rhs) const
{
    if (type != rhs.type)
        return false;
    
    switch (type)
    {
    case JSON_BOOL:
        return data.boolValue == rhs.data.boolValue;
        
    case JSON_NUMBER:
        return data.numberValue == rhs.data.numberValue;
        
    case JSON_STRING:
        return *(reinterpret_cast<const String*>(&data)) == *(reinterpret_cast<const String*>(&rhs.data));
        
    case JSON_ARRAY:
        return *(reinterpret_cast<const JSONArray*>(&data)) == *(reinterpret_cast<const JSONArray*>(&rhs.data));
        
    case JSON_OBJECT:
        return *(reinterpret_cast<const JSONObject*>(&data)) == *(reinterpret_cast<const JSONObject*>(&rhs.data));
        
    default:
        return true;
    }
}

bool JSONValue::FromString(const String& str)
{
    const char* pos = str.CString();
    const char* end = pos + str.Length();
    return Parse(pos, end);
}

bool JSONValue::FromString(const char* str)
{
    const char* pos = str;
    const char* end = pos + String::CStringLength(str);
    return Parse(pos, end);
}

void JSONValue::ToString(String& dest, int spacing, int indent) const
{
    switch (type)
    {
    case JSON_BOOL:
        dest += data.boolValue;
        return;
        
    case JSON_NUMBER:
        dest += String(data.numberValue);
        return;
        
    case JSON_STRING:
        WriteJSONString(dest, *(reinterpret_cast<const String*>(&data)));
        return;
        
    case JSON_ARRAY:
        {
            const JSONArray& array = GetArray();
            dest += '[';
            
            if (array.Size())
            {
                indent += spacing;
                for (JSONArray::ConstIterator it = array.Begin(); it < array.End(); ++it)
                {
                    if (it != array.Begin())
                        dest += ',';
                    dest += '\n';
                    WriteIndent(dest, indent);
                    it->ToString(dest, spacing, indent);
                }
                indent -= spacing;
                dest += '\n';
                WriteIndent(dest, indent);
            }
            
            dest += ']';
        }
        break;
        
    case JSON_OBJECT:
        {
            const JSONObject& object = GetObject();
            dest += '{';
            
            if (object.Size())
            {
                indent += spacing;
                for (JSONObject::ConstIterator it = object.Begin(); it != object.End(); ++it)
                {
                    if (it != object.Begin())
                        dest += ',';
                    dest += '\n';
                    WriteIndent(dest, indent);
                    WriteJSONString(dest, it->first_);
                    dest += ": ";
                    it->second_.ToString(dest, spacing, indent);
                }
                indent -= spacing;
                dest += '\n';
                WriteIndent(dest, indent);
            }
            
            dest += '}';
        }
        break;
        
    default:
        dest += "null";
    }
}

String JSONValue::ToString(int spacing) const
{
    String ret;
    ToString(ret, spacing);
    return ret;
}

void JSONValue::Push(const JSONValue& value)
{
    SetType(JSON_ARRAY);
    (*(reinterpret_cast<JSONArray*>(&data))).Push(value);
}

void JSONValue::Insert(uint index, const JSONValue& value)
{
    SetType(JSON_ARRAY);
    (*(reinterpret_cast<JSONArray*>(&data))).Insert(index, value);
}

void JSONValue::Pop()
{
    if (type == JSON_ARRAY)
        (*(reinterpret_cast<JSONArray*>(&data))).Pop();
}

void JSONValue::Erase(uint pos, uint length)
{
    if (type == JSON_ARRAY)
        (*(reinterpret_cast<JSONArray*>(&data))).Erase(pos, length);
}

void JSONValue::Resize(uint newSize)
{
    SetType(JSON_ARRAY);
    (*(reinterpret_cast<JSONArray*>(&data))).Resize(newSize);
}

void JSONValue::Insert(const Pair<String, JSONValue>& pair)
{
    SetType(JSON_OBJECT);
    (*(reinterpret_cast<JSONObject*>(&data))).Insert(pair);
}

void JSONValue::Erase(const String& key)
{
    if (type == JSON_OBJECT)
        (*(reinterpret_cast<JSONObject*>(&data))).Erase(key);
}

void JSONValue::Clear()
{
    if (type == JSON_ARRAY)
        (*(reinterpret_cast<JSONArray*>(&data))).Clear();
    else if (type == JSON_OBJECT)
        (*(reinterpret_cast<JSONObject*>(&data))).Clear();
}

void JSONValue::SetEmptyArray()
{
    SetType(JSON_ARRAY);
    Clear();
}

void JSONValue::SetEmptyObject()
{
    SetType(JSON_OBJECT);
    Clear();
}

void JSONValue::SetNull()
{
    SetType(JSON_NULL);
}

uint JSONValue::Size() const
{
    if (type == JSON_ARRAY)
        return (*(reinterpret_cast<const JSONArray*>(&data))).Size();
    else if (type == JSON_OBJECT)
        return (*(reinterpret_cast<const JSONObject*>(&data))).Size();
    else
        return 0U;
}

bool JSONValue::IsEmpty() const
{
    if (type == JSON_ARRAY)
        return (*(reinterpret_cast<const JSONArray*>(&data))).Empty();
    else if (type == JSON_OBJECT)
        return (*(reinterpret_cast<const JSONObject*>(&data))).Empty();
    else
        return false;
}

bool JSONValue::Contains(const String& key) const
{
    if (type == JSON_OBJECT)
        return (*(reinterpret_cast<const JSONObject*>(&data))).Contains(key);
    else
        return false;
}

bool JSONValue::Parse(const char*& pos, const char*& end)
{
    char c;
    
    // Handle comments
    for (;;)
    {
        if (!NextChar(c, pos, end, true))
            return false;
    
        if (c == '/')
        {
            if (!NextChar(c, pos, end, false))
                return false;
            if (c == '/')
            {
                // Skip until end of line
                if (!MatchChar('\n', pos, end))
                    return false;
            }
            else if (c == '*')
            {
                // Skip until end of comment
                if (!MatchChar('*', pos, end))
                    return false;
                if (!MatchChar('/', pos, end))
                    return false;
            }
            else
                return false;
        }
        else
            break;
    }

    if (c == '}' || c == ']')
        return false;
    else if (c == 'n')
    {
        SetNull();
        return MatchString("ull", pos, end);
    }
    else if (c == 'f')
    {
        *this = false;
        return MatchString("alse", pos, end);
    }
    else if (c == 't')
    {
        *this = true;
        return MatchString("rue", pos, end);
    }
    else if (IsDigit(c) || c == '-')
    {
        --pos;
        *this = strtod(pos, const_cast<char**>(&pos));
        return true;
    }
    else if (c == '\"')
    {
        SetType(JSON_STRING);
        return ReadJSONString(*(reinterpret_cast<String*>(&data)), pos, end, true);
    }
    else if (c == '[')
    {
        SetEmptyArray();
        // Check for empty first
        if (!NextChar(c, pos, end, true))
            return false;
        if (c == ']')
            return true;
        else
            --pos;
        
        for (;;)
        {
            JSONValue arrayValue;
            if (!arrayValue.Parse(pos, end))
                return false;
            Push(arrayValue);
            if (!NextChar(c, pos, end, true))
                return false;
            if (c == ']')
                break;
            else if (c != ',')
                return false;
        }
        return true;
    }
    else if (c == '{')
    {
        SetEmptyObject();
        if (!NextChar(c, pos, end, true))
            return false;
        if (c == '}')
            return true;
        else
            --pos;
        
        for (;;)
        {
            String key;
            if (!ReadJSONString(key, pos, end, false))
                return false;
            if (!NextChar(c, pos, end, true))
                return false;
            if (c != ':')
                return false;
            
            JSONValue objectValue;
            if (!objectValue.Parse(pos, end))
                return false;
            (*this)[key] = objectValue;
            if (!NextChar(c, pos, end, true))
                return false;
            if (c == '}')
                break;
            else if (c != ',')
                return false;
        }
        return true;
    }
    
    return false;
}

void JSONValue::SetType(JSONType newType)
{
    if (type == newType)
        return;
    
    switch (type)
    {
    case JSON_STRING:
        (reinterpret_cast<String*>(&data))->~String();
        break;
        
    case JSON_ARRAY:
        (reinterpret_cast<JSONArray*>(&data))->~JSONArray();
        break;
        
    case JSON_OBJECT:
        (reinterpret_cast<JSONObject*>(&data))->~JSONObject();
        break;
        
    default:
        break;
    }
    
    type = newType;
    
    switch (type)
    {
    case JSON_STRING:
        new(reinterpret_cast<String*>(&data)) String();
        break;
        
    case JSON_ARRAY:
        new(reinterpret_cast<JSONArray*>(&data)) JSONArray();
        break;
        
    case JSON_OBJECT:
        new(reinterpret_cast<JSONObject*>(&data)) JSONObject();
        break;
        
    default:
        break;
    }
}

void JSONValue::WriteJSONString(String& dest, const String& str)
{
    dest += '\"';
    
    for (String::ConstIterator it = str.Begin(); it != str.End(); ++it)
    {
        char c = *it;
        
        if (c >= 0x20 && c != '\"' && c != '\\')
            dest += c;
        else
        {
            dest += '\\';
            
            switch (c)
            {
            case '\"':
            case '\\':
                dest += c;
                break;
                
            case '\b':
                dest += 'b';
                break;
                
            case '\f':
                dest += 'f';
                break;
                
            case '\n':
                dest += 'n';
                break;
                
            case '\r':
                dest += 'r';
                break;
                
            case '\t':
                dest += 't';
                break;
                
            default:
                {
                    char buffer[6];
                    sprintf(buffer, "u%04x", c);
                    dest += (const char*)&buffer[0];
                }
                break;
            }
        }
    }
    
    dest += '\"';
}

void JSONValue::WriteIndent(String& dest, int indent)
{
    uint oldLength = dest.Length();
    dest.Resize(oldLength + indent);
    for (int i = 0; i < indent; ++i)
        dest[oldLength + i] = ' ';
}

bool JSONValue::ReadJSONString(String& dest, const char*& pos, const char*& end, bool inQuote)
{
    char c;
    
    if (!inQuote)
    {
        if (!NextChar(c, pos, end, true) || c != '\"')
            return false;
    }
    
    dest.Clear();
    for (;;)
    {
        if (!NextChar(c, pos, end, false))
            return false;
        if (c == '\"')
            break;
        else if (c != '\\')
            dest += c;
        else
        {
            if (!NextChar(c, pos, end, false))
                return false;
            switch (c)
            {
            case '\\':
                dest += '\\';
                break;
                
            case '\"':
                dest += '\"';
                break;
                
            case 'b':
                dest += '\b';
                break;
                
            case 'f':
                dest += '\f';
                break;
                
            case 'n':
                dest += '\n';
                break;
            
            case 'r':
                dest += '\r';
                break;
                
            case 't':
                dest += '\t';
                break;
                
            case 'u':
                {
                    /// \todo Doesn't handle unicode surrogate pairs
                    unsigned code;
                    /// @todo check return value
                    code = strtol(pos, 0, 16);
                    pos += 4;
                    dest.AppendUTF8(code);
                }
                break;
            }
        }
    }
    
    return true;
}

bool JSONValue::MatchString(const char* str, const char*& pos, const char*& end)
{
    while (*str)
    {
        if (pos >= end || *pos != *str)
            return false;
        else
        {
            ++pos;
            ++str;
        }
    }
    
    return true;
}

bool JSONValue::MatchChar(char c, const char*& pos, const char*& end)
{
    char next;

    while (NextChar(next, pos, end, false))
    {
        if (next == c)
            return true;
    }
    return false;
}

}
