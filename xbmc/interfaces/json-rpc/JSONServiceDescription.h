#pragma once
/*
 *      Copyright (C) 2005-2012 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <string>
#include <vector>
#include <limits>

#include "JSONUtils.h"

namespace JSONRPC
{
  /*! 
   \ingroup jsonrpc
   \brief Class for a parameter of a
   json rpc method.
   
   Represents a parameter of a defined
   json rpc method and is used to verify
   and extract the value of the parameter
   in a method call.
   */
  class JSONSchemaTypeDefinition : protected CJSONUtils
  {
  public:
    JSONSchemaTypeDefinition();
    
    bool Parse(const CVariant &value, bool isParameter = false);
    JSONRPC_STATUS Check(const CVariant &value, CVariant &outputValue, CVariant &errorData) const;
    void Print(bool isParameter, bool isGlobal, bool printDefault, bool printDescriptions, CVariant &output) const;
    
    std::string missingReference;

    /*!
     \brief Name of the parameter (for 
     by-name calls)
     */
    std::string name;

    /*!
     \brief Id of the type (for
     referenced types)
     Renamed from "id" because of possible
     issues with Objective-C.
     */
    std::string ID;

    /*!
     \brief Array of reference types
     which are extended by this type.
     */
    std::vector<JSONSchemaTypeDefinition> extends;

    /*!
     \brief Description of the parameter
     */
    std::string description;

    /*!
     \brief JSON schema type of the parameter's value
     */
    JSONSchemaType type;

    /*!
     \brief JSON schema type definitions in case
     of a union type
     */
    std::vector<JSONSchemaTypeDefinition> unionTypes;

    /*!
     \brief Whether or not the parameter is
     optional
     */
    bool optional;

    /*!
     \brief Default value of the parameter
     (only needed when it is optional)
     */
    CVariant defaultValue;

    /*!
     \brief Minimum value for Integer
     or Number types
     */
    double minimum;

    /*!
     \brief Maximum value for Integer or Number types
     */
    double maximum;

    /*!
     \brief Whether to exclude the defined Minimum
     value from the valid range or not
     */
    bool exclusiveMinimum;

    /*!
     \brief  Whether to exclude the defined Maximum
     value from the valid range or not
     */
    bool exclusiveMaximum;

    /*!
     \brief Integer by which the value (of type
     Integer) must be divisible without rest
     */
    unsigned int divisibleBy;

    /*!
     \brief Minimum length for String types
     */
    int minLength;

    /*!
     \brief Maximum length for String types
     */
    int maxLength;

    /*!
     \brief (Optional) List of allowed values
     for the type
     */
    std::vector<CVariant> enums;

    /*!
     \brief List of possible values in an array
     */
    std::vector<JSONSchemaTypeDefinition> items;

    /*!
     \brief Minimum amount of items in the array
     */
    unsigned int minItems;

    /*!
     \brief Maximum amount of items in the array
     */
    unsigned int maxItems;

    /*!
     \brief Whether every value in the array
     must be unique or not
     */
    bool uniqueItems;

    /*!
     \brief List of json schema definitions for
     additional items in an array with tuple
     typing (defined schemas in "items")
     */
    std::vector<JSONSchemaTypeDefinition> additionalItems;

    /*!
     \brief Maps a properties name to its
     json schema type definition
     */
    class CJsonSchemaPropertiesMap
    {
    public:
      CJsonSchemaPropertiesMap();

      void add(const JSONSchemaTypeDefinition &property);

      typedef std::map<std::string, JSONSchemaTypeDefinition>::const_iterator JSONSchemaPropertiesIterator;
      JSONSchemaPropertiesIterator begin() const;
      JSONSchemaPropertiesIterator find(const std::string& key) const;
      JSONSchemaPropertiesIterator end() const;
      unsigned int size() const;
    private:
      std::map<std::string, JSONSchemaTypeDefinition> m_propertiesmap;
    };

    /*!
     \brief List of properties of the parameter (only needed when the
     parameter is an object)
     */
    CJsonSchemaPropertiesMap properties;

    /*!
     \brief Whether the type can have additional properties
     or not
     */
    bool hasAdditionalProperties;

    /*!
     \brief Type definition for additional properties
     */
    JSONSchemaTypeDefinition* additionalProperties;
  };

  /*! 
   \ingroup jsonrpc
   \brief Structure for a published json
   rpc method.
   
   Represents a published json rpc method
   and is used to verify an incoming json
   rpc request against a defined method.
   */
  class JsonRpcMethod : protected CJSONUtils
  {
  public:
    JsonRpcMethod();
  
    bool Parse(const CVariant &value);
    JSONRPC_STATUS Check(const CVariant &requestParameters, ITransportLayer *transport, IClient *client, bool notification, MethodCall &methodCall, CVariant &outputParameters) const;
    
    std::string missingReference;    
    
    /*!
     \brief Name of the represented method
     */
    std::string name;
    /*!
     \brief Pointer tot he implementation
     of the represented method
     */
    MethodCall method;
    /*!
     \brief Definition of the type of
     request/response
     */
    TransportLayerCapability transportneed;
    /*!
     \brief Definition of the permissions needed
     to execute the method
     */
    OperationPermission permission;
    /*!
     \brief Description of the method
     */
    std::string description;
    /*!
     \brief List of accepted parameters
     */
    std::vector<JSONSchemaTypeDefinition> parameters;
    /*!
     \brief Definition of the return value
     */
    JSONSchemaTypeDefinition returns;
  
  private:
    bool parseParameter(const CVariant &value, JSONSchemaTypeDefinition &parameter);
    bool parseReturn(const CVariant &value);
    static JSONRPC_STATUS checkParameter(const CVariant &requestParameters, const JSONSchemaTypeDefinition &type, unsigned int position, CVariant &outputParameters, unsigned int &handled, CVariant &errorData);
  };

  /*! 
   \ingroup jsonrpc
   \brief Structure mapping a json rpc method
   definition to an actual method implementation.
   */
  typedef struct
  {
    /*!
     \brief Name of the json rpc method.
     */
    std::string name;
    /*!
     \brief Pointer to the actual
     implementation of the json rpc
     method.
     */
    MethodCall method;
  } JsonRpcMethodMap;

  /*!
   \ingroup jsonrpc
   \brief Helper class for json schema service descriptor based
   service descriptions for the json rpc API

   Provides static functions to parse a complete json schema
   service descriptor of a published service containing json rpc 
   methods, print the json schema service descriptor representation 
   into a string (mainly for output purposes) and evaluate and verify 
   parameters provided in a call to one of the publish json rpc methods 
   against a parameter definition parsed from a json schema service
   descriptor.
   */
  class CJSONServiceDescription : public CJSONUtils
  {
    friend class JSONSchemaTypeDefinition;
    friend class JsonRpcMethod;
  public:
    /*!
     \brief Parses the given json schema description and evaluates
     and stores the defined type
     \param jsonType json schema description to parse
     \return True if the json schema description has been parsed sucessfully otherwise false
     */
    static bool AddType(const std::string &jsonType);

    /*!
     \brief Parses the given json schema description and evaluates
     and stores the defined method
     \param jsonMethod json schema description to parse
     \param method pointer to the implementation
     \return True if the json schema description has been parsed sucessfully otherwise false
     */
    static bool AddMethod(const std::string &jsonMethod, MethodCall method);

    /*!
     \brief Parses the given json schema description and evaluates
     and stores the defined builtin method
     \param jsonMethod json schema description to parse
     \return True if the json schema description has been parsed sucessfully otherwise false
     */
    static bool AddBuiltinMethod(const std::string &jsonMethod);

    /*!
     \brief Parses the given json schema description and evaluates
     and stores the defined notification
     \param jsonNotification json schema description to parse
     \return True if the json schema description has been parsed sucessfully otherwise false
     */
    static bool AddNotification(const std::string &jsonNotification);

    static bool AddEnum(const std::string &name, const std::vector<CVariant> &values, CVariant::VariantType type = CVariant::VariantTypeNull, const CVariant &defaultValue = CVariant::ConstNullVariant);
    static bool AddEnum(const std::string &name, const std::vector<std::string> &values);
    static bool AddEnum(const std::string &name, const std::vector<int> &values);

    /*!
     \brief Gets the version of the json
     schema description
     \return Version of the json schema description
     */
    static int GetVersion();

    /*!
     \brief Prints the json schema description into the given result object
     \param result Object into which the json schema description is printed
     \param transport Transport layer capabilities
     \param client Client requesting a print
     \param printDescriptions Whether to print descriptions or not
     \param printMetadata Whether to print XBMC specific data or not
     \param filterByTransport Whether to filter by transport or not
     */
    static JSONRPC_STATUS Print(CVariant &result, ITransportLayer *transport, IClient *client, bool printDescriptions = true, bool printMetadata = false, bool filterByTransport = true, const std::string &filterByName = "", const std::string &filterByType = "", bool printReferences = true);

    /*!
     \brief Checks the given parameters from the request against the
     json schema description for the given method
     \param method Called method
     \param requestParameters Parameters from the request
     \param client Client who sent the request
     \param notification Whether the request was sent as a notification or not
     \param methodCall Object which will contain the actual C/C++ method to be called
     \param outputParameters Cleaned up parameter list
     \return OK if the validation of the request succeeded otherwise an appropriate error code

     Checks if the given method is a valid json rpc method, if the client has the permission
     to call this method, if the method can be called as a notification or not, assigns the
     actual C/C++ implementation of the method to the "methodCall" parameter and checks the
     given parameters from the request against the json schema description for the given method.
     */
    static JSONRPC_STATUS CheckCall(const char* method, const CVariant &requestParameters, ITransportLayer *transport, IClient *client, bool notification, MethodCall &methodCall, CVariant &outputParameters);
    
    static JSONSchemaTypeDefinition* GetType(const std::string &identification);

  private:
    static bool prepareDescription(std::string &description, CVariant &descriptionObject, std::string &name);
    static bool addMethod(const std::string &jsonMethod, MethodCall method);
    static void parseHeader(const CVariant &descriptionObject);
    static bool parseJSONSchemaType(const CVariant &value, std::vector<JSONSchemaTypeDefinition>& typeDefinitions, JSONSchemaType &schemaType, std::string &missingReference);
    static void addReferenceTypeDefinition(const JSONSchemaTypeDefinition &typeDefinition);

    static void getReferencedTypes(const JSONSchemaTypeDefinition &type, std::vector<std::string> &referencedTypes);

    class CJsonRpcMethodMap
    {
    public:
      CJsonRpcMethodMap();

      void add(const JsonRpcMethod &method);

      typedef std::map<std::string, JsonRpcMethod>::const_iterator JsonRpcMethodIterator;
      JsonRpcMethodIterator begin() const;
      JsonRpcMethodIterator find(const std::string& key) const;
      JsonRpcMethodIterator end() const;
    private:
      std::map<std::string, JsonRpcMethod> m_actionmap;
    };

    static CJsonRpcMethodMap m_actionMap;
    static std::map<std::string, JSONSchemaTypeDefinition> m_types;
    static std::map<std::string, CVariant> m_notifications;
    static JsonRpcMethodMap m_methodMaps[];

    typedef enum SchemaDefinition
    {
      SchemaDefinitionType,
      SchemaDefinitionMethod
    } SchemaDefinition;

    typedef struct IncompleteSchemaDefinition
    {
      std::string Schema;
      SchemaDefinition Type;
      MethodCall Method;
    } IncompleteSchemaDefinition;

    typedef std::map<std::string, std::vector<IncompleteSchemaDefinition> > IncompleteSchemaDefinitionMap;
    static IncompleteSchemaDefinitionMap m_incompleteDefinitions;
  };
}
