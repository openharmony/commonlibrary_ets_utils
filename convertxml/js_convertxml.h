#ifndef FOUNDATION_ACE_CCRUNTIME_CONVERT_XML_CLASS_H
#define FOUNDATION_ACE_CCRUNTIME_CONVERT_XML_CLASS_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include <string>
#include <vector>
#include <map>

enum SpaceType {
    T_INT32,
    T_STRING,
    T_INIT = -1
};

struct Options {
        std::string declaration = "_declaration";
        std::string instruction = "_instruction";
        std::string attributes = "_attributes";
        std::string text = "_text";
        std::string cdata = "_cdata";
        std::string doctype = "_doctype";
        std::string comment = "_comment";
        std::string parent = "_parent";
        std::string type = "_type";
        std::string name = "_name";
        std::string elements = "_elements";
        bool compact = false;
        bool trim = false;
        bool nativetype = false;
        bool nativetypeattributes = false;
        bool addparent = false;
        bool alwaysArray = false;
        bool alwaysChildren = false;
        bool instructionHasAttributes = false;
        bool ignoreDeclaration = false;
        bool ignoreInstruction = false;
        bool ignoreAttributes = false;
        bool ignoreComment = false;
        bool ignoreCdata = false;
        bool ignoreDoctype = false;
        bool ignoreText = false;
        bool spaces = false;
};

class ConvertXml
{
public:
        explicit ConvertXml(napi_env env);
        virtual ~ConvertXml() {}
        void SetAttributes(xmlNodePtr curNode, napi_value& elementsObject);
        void SetXmlElementType(xmlNodePtr curNode, napi_value& elementsObject);
        void SetNodeInfo(xmlNodePtr curNode, napi_value& elementsObject);
        void SetEndInfo(xmlNodePtr curNode, napi_value& elementsObject, bool& bFlag, bool& TextFlag, int32_t index);
        void GetXMLInfo(xmlNodePtr curNode, napi_value& object, int flag = 0);
        napi_value convert(std::string strXml);
        std::string GetNodeType(xmlElementType enumType);
        napi_status DealNapiStrValue(napi_value napi_StrValue, std::string& result);
        void SetKeyValue(napi_value& object, std::string strKey, std::string strValue);
        void DealOptions(napi_value napi_obj);
        std::string Trim(std::string strXmltrim);
        void GetPrevNodeList(xmlNodePtr curNode);
        void DealSpaces(napi_value napi_obj);
        void DealIgnore(napi_value napi_obj);
        void SetPrevInfo(napi_value& recvElement, int flag, int32_t& index1);
private:
        napi_env env_;
        SpaceType m_SpaceType;
        int32_t m_iSpace;
        std::string m_strSpace;
        Options m_Options;
        std::vector<napi_value> m_prevObj;
};
#endif