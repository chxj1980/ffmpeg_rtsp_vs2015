/* soapStub.h
   Generated by gSOAP 2.8.66 for service3.h

gSOAP XML Web services tools
Copyright (C) 2000-2018, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#define SOAP_NAMESPACE_OF_ns1	"http://service.bykj.com/"

#ifndef soapStub_H
#define soapStub_H
#include "stdsoap2.h"
#if GSOAP_VERSION != 20866
# error "GSOAP VERSION 20866 MISMATCH IN GENERATED CODE VERSUS LIBRARY CODE: PLEASE REINSTALL PACKAGE"
#endif


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes, Structs and Unions                                                *
 *                                                                            *
\******************************************************************************/

class ns1__getUdpUrl;	/* service3.h:137 */
class ns1__getUdpUrlResponse;	/* service3.h:140 */
class ns1__startGui;	/* service3.h:143 */
class ns1__startGuiResponse;	/* service3.h:146 */
struct __ns1__startGui;	/* service3.h:425 */
struct __ns1__getUdpUrl;	/* service3.h:492 */

/* service3.h:137 */
#ifndef SOAP_TYPE_ns1__getUdpUrl
#define SOAP_TYPE_ns1__getUdpUrl (7)
/* complex XSD type 'ns1:getUdpUrl': */
class SOAP_CMAC ns1__getUdpUrl {
      public:
        /// Optional element 'arg0' of XSD type 'xsd:string'
        char *arg0;
        /// Context that manages this object
        struct soap *soap;
      public:
        /// Return unique type id SOAP_TYPE_ns1__getUdpUrl
        virtual long soap_type(void) const { return SOAP_TYPE_ns1__getUdpUrl; }
        /// (Re)set members to default values
        virtual void soap_default(struct soap*);
        /// Serialize object to prepare for SOAP 1.1/1.2 encoded output (or with SOAP_XML_GRAPH) by analyzing its (cyclic) structures
        virtual void soap_serialize(struct soap*) const;
        /// Output object in XML, compliant with SOAP 1.1 encoding style, return error code or SOAP_OK
        virtual int soap_put(struct soap*, const char *tag, const char *type) const;
        /// Output object in XML, with tag and optional id attribute and xsi:type, return error code or SOAP_OK
        virtual int soap_out(struct soap*, const char *tag, int id, const char *type) const;
        /// Get object from XML, compliant with SOAP 1.1 encoding style, return pointer to object or NULL on error
        virtual void *soap_get(struct soap*, const char *tag, const char *type);
        /// Get object from XML, with matching tag and type (NULL matches any tag and type), return pointer to object or NULL on error
        virtual void *soap_in(struct soap*, const char *tag, const char *type);
        /// Return a new object of type ns1__getUdpUrl, default initialized and not managed by a soap context
        virtual ns1__getUdpUrl *soap_alloc(void) const { return SOAP_NEW_UNMANAGED(ns1__getUdpUrl); }
      public:
        /// Constructor with initializations
        ns1__getUdpUrl() : arg0(), soap() { }
        virtual ~ns1__getUdpUrl() { }
        /// Friend allocator used by soap_new_ns1__getUdpUrl(struct soap*, int)
        friend SOAP_FMAC1 ns1__getUdpUrl * SOAP_FMAC2 soap_instantiate_ns1__getUdpUrl(struct soap*, int, const char*, const char*, size_t*);
};
#endif

/* service3.h:140 */
#ifndef SOAP_TYPE_ns1__getUdpUrlResponse
#define SOAP_TYPE_ns1__getUdpUrlResponse (8)
/* complex XSD type 'ns1:getUdpUrlResponse': */
class SOAP_CMAC ns1__getUdpUrlResponse {
      public:
        /// Optional element 'return' of XSD type 'xsd:string'
        char *return_;
        /// Context that manages this object
        struct soap *soap;
      public:
        /// Return unique type id SOAP_TYPE_ns1__getUdpUrlResponse
        virtual long soap_type(void) const { return SOAP_TYPE_ns1__getUdpUrlResponse; }
        /// (Re)set members to default values
        virtual void soap_default(struct soap*);
        /// Serialize object to prepare for SOAP 1.1/1.2 encoded output (or with SOAP_XML_GRAPH) by analyzing its (cyclic) structures
        virtual void soap_serialize(struct soap*) const;
        /// Output object in XML, compliant with SOAP 1.1 encoding style, return error code or SOAP_OK
        virtual int soap_put(struct soap*, const char *tag, const char *type) const;
        /// Output object in XML, with tag and optional id attribute and xsi:type, return error code or SOAP_OK
        virtual int soap_out(struct soap*, const char *tag, int id, const char *type) const;
        /// Get object from XML, compliant with SOAP 1.1 encoding style, return pointer to object or NULL on error
        virtual void *soap_get(struct soap*, const char *tag, const char *type);
        /// Get object from XML, with matching tag and type (NULL matches any tag and type), return pointer to object or NULL on error
        virtual void *soap_in(struct soap*, const char *tag, const char *type);
        /// Return a new object of type ns1__getUdpUrlResponse, default initialized and not managed by a soap context
        virtual ns1__getUdpUrlResponse *soap_alloc(void) const { return SOAP_NEW_UNMANAGED(ns1__getUdpUrlResponse); }
      public:
        /// Constructor with initializations
        ns1__getUdpUrlResponse() : return_(), soap() { }
        virtual ~ns1__getUdpUrlResponse() { }
        /// Friend allocator used by soap_new_ns1__getUdpUrlResponse(struct soap*, int)
        friend SOAP_FMAC1 ns1__getUdpUrlResponse * SOAP_FMAC2 soap_instantiate_ns1__getUdpUrlResponse(struct soap*, int, const char*, const char*, size_t*);
};
#endif

/* service3.h:143 */
#ifndef SOAP_TYPE_ns1__startGui
#define SOAP_TYPE_ns1__startGui (9)
/* complex XSD type 'ns1:startGui': */
class SOAP_CMAC ns1__startGui {
      public:
        /// Optional element 'arg0' of XSD type 'xsd:string'
        char *arg0;
        /// Context that manages this object
        struct soap *soap;
      public:
        /// Return unique type id SOAP_TYPE_ns1__startGui
        virtual long soap_type(void) const { return SOAP_TYPE_ns1__startGui; }
        /// (Re)set members to default values
        virtual void soap_default(struct soap*);
        /// Serialize object to prepare for SOAP 1.1/1.2 encoded output (or with SOAP_XML_GRAPH) by analyzing its (cyclic) structures
        virtual void soap_serialize(struct soap*) const;
        /// Output object in XML, compliant with SOAP 1.1 encoding style, return error code or SOAP_OK
        virtual int soap_put(struct soap*, const char *tag, const char *type) const;
        /// Output object in XML, with tag and optional id attribute and xsi:type, return error code or SOAP_OK
        virtual int soap_out(struct soap*, const char *tag, int id, const char *type) const;
        /// Get object from XML, compliant with SOAP 1.1 encoding style, return pointer to object or NULL on error
        virtual void *soap_get(struct soap*, const char *tag, const char *type);
        /// Get object from XML, with matching tag and type (NULL matches any tag and type), return pointer to object or NULL on error
        virtual void *soap_in(struct soap*, const char *tag, const char *type);
        /// Return a new object of type ns1__startGui, default initialized and not managed by a soap context
        virtual ns1__startGui *soap_alloc(void) const { return SOAP_NEW_UNMANAGED(ns1__startGui); }
      public:
        /// Constructor with initializations
        ns1__startGui() : arg0(), soap() { }
        virtual ~ns1__startGui() { }
        /// Friend allocator used by soap_new_ns1__startGui(struct soap*, int)
        friend SOAP_FMAC1 ns1__startGui * SOAP_FMAC2 soap_instantiate_ns1__startGui(struct soap*, int, const char*, const char*, size_t*);
};
#endif

/* service3.h:146 */
#ifndef SOAP_TYPE_ns1__startGuiResponse
#define SOAP_TYPE_ns1__startGuiResponse (10)
/* complex XSD type 'ns1:startGuiResponse': */
class SOAP_CMAC ns1__startGuiResponse {
      public:
        /// Optional element 'return' of XSD type 'xsd:string'
        char *return_;
        /// Context that manages this object
        struct soap *soap;
      public:
        /// Return unique type id SOAP_TYPE_ns1__startGuiResponse
        virtual long soap_type(void) const { return SOAP_TYPE_ns1__startGuiResponse; }
        /// (Re)set members to default values
        virtual void soap_default(struct soap*);
        /// Serialize object to prepare for SOAP 1.1/1.2 encoded output (or with SOAP_XML_GRAPH) by analyzing its (cyclic) structures
        virtual void soap_serialize(struct soap*) const;
        /// Output object in XML, compliant with SOAP 1.1 encoding style, return error code or SOAP_OK
        virtual int soap_put(struct soap*, const char *tag, const char *type) const;
        /// Output object in XML, with tag and optional id attribute and xsi:type, return error code or SOAP_OK
        virtual int soap_out(struct soap*, const char *tag, int id, const char *type) const;
        /// Get object from XML, compliant with SOAP 1.1 encoding style, return pointer to object or NULL on error
        virtual void *soap_get(struct soap*, const char *tag, const char *type);
        /// Get object from XML, with matching tag and type (NULL matches any tag and type), return pointer to object or NULL on error
        virtual void *soap_in(struct soap*, const char *tag, const char *type);
        /// Return a new object of type ns1__startGuiResponse, default initialized and not managed by a soap context
        virtual ns1__startGuiResponse *soap_alloc(void) const { return SOAP_NEW_UNMANAGED(ns1__startGuiResponse); }
      public:
        /// Constructor with initializations
        ns1__startGuiResponse() : return_(), soap() { }
        virtual ~ns1__startGuiResponse() { }
        /// Friend allocator used by soap_new_ns1__startGuiResponse(struct soap*, int)
        friend SOAP_FMAC1 ns1__startGuiResponse * SOAP_FMAC2 soap_instantiate_ns1__startGuiResponse(struct soap*, int, const char*, const char*, size_t*);
};
#endif

/* service3.h:425 */
#ifndef SOAP_TYPE___ns1__startGui
#define SOAP_TYPE___ns1__startGui (15)
/* Wrapper: */
struct SOAP_CMAC __ns1__startGui {
      public:
        /** Optional element 'ns1:startGui' of XSD type 'ns1:startGui' */
        ns1__startGui *ns1__startGui_;
      public:
        /** Return unique type id SOAP_TYPE___ns1__startGui */
        long soap_type() const { return SOAP_TYPE___ns1__startGui; }
        /** Constructor with member initializations */
        __ns1__startGui() : ns1__startGui_() { }
        /** Friend allocator */
        friend SOAP_FMAC1 __ns1__startGui * SOAP_FMAC2 soap_instantiate___ns1__startGui(struct soap*, int, const char*, const char*, size_t*);
};
#endif

/* service3.h:492 */
#ifndef SOAP_TYPE___ns1__getUdpUrl
#define SOAP_TYPE___ns1__getUdpUrl (19)
/* Wrapper: */
struct SOAP_CMAC __ns1__getUdpUrl {
      public:
        /** Optional element 'ns1:getUdpUrl' of XSD type 'ns1:getUdpUrl' */
        ns1__getUdpUrl *ns1__getUdpUrl_;
      public:
        /** Return unique type id SOAP_TYPE___ns1__getUdpUrl */
        long soap_type() const { return SOAP_TYPE___ns1__getUdpUrl; }
        /** Constructor with member initializations */
        __ns1__getUdpUrl() : ns1__getUdpUrl_() { }
        /** Friend allocator */
        friend SOAP_FMAC1 __ns1__getUdpUrl * SOAP_FMAC2 soap_instantiate___ns1__getUdpUrl(struct soap*, int, const char*, const char*, size_t*);
};
#endif

/* service3.h:563 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (20)
/* SOAP_ENV__Header: */
struct SOAP_CMAC SOAP_ENV__Header {
      public:
        /** Return unique type id SOAP_TYPE_SOAP_ENV__Header */
        long soap_type() const { return SOAP_TYPE_SOAP_ENV__Header; }
        /** Constructor with member initializations */
        SOAP_ENV__Header() { }
        /** Friend allocator */
        friend SOAP_FMAC1 SOAP_ENV__Header * SOAP_FMAC2 soap_instantiate_SOAP_ENV__Header(struct soap*, int, const char*, const char*, size_t*);
};
#endif
#endif

/* service3.h:563 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (21)
/* Type SOAP_ENV__Code is a recursive data type, (in)directly referencing itself through its (base or derived class) members */
/* SOAP_ENV__Code: */
struct SOAP_CMAC SOAP_ENV__Code {
      public:
        /** Optional element 'SOAP-ENV:Value' of XSD type 'xsd:QName' */
        char *SOAP_ENV__Value;
        /** Optional element 'SOAP-ENV:Subcode' of XSD type 'SOAP-ENV:Code' */
        struct SOAP_ENV__Code *SOAP_ENV__Subcode;
      public:
        /** Return unique type id SOAP_TYPE_SOAP_ENV__Code */
        long soap_type() const { return SOAP_TYPE_SOAP_ENV__Code; }
        /** Constructor with member initializations */
        SOAP_ENV__Code() : SOAP_ENV__Value(), SOAP_ENV__Subcode() { }
        /** Friend allocator */
        friend SOAP_FMAC1 SOAP_ENV__Code * SOAP_FMAC2 soap_instantiate_SOAP_ENV__Code(struct soap*, int, const char*, const char*, size_t*);
};
#endif
#endif

/* service3.h:563 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (23)
/* SOAP_ENV__Detail: */
struct SOAP_CMAC SOAP_ENV__Detail {
      public:
        char *__any;
        /** Any type of element 'fault' assigned to fault with its SOAP_TYPE_T assigned to __type */
        /** Do not create a cyclic data structure throught this member unless SOAP encoding or SOAP_XML_GRAPH are used for id-ref serialization */
        int __type;
        void *fault;
      public:
        /** Return unique type id SOAP_TYPE_SOAP_ENV__Detail */
        long soap_type() const { return SOAP_TYPE_SOAP_ENV__Detail; }
        /** Constructor with member initializations */
        SOAP_ENV__Detail() : __any(), __type(), fault() { }
        /** Friend allocator */
        friend SOAP_FMAC1 SOAP_ENV__Detail * SOAP_FMAC2 soap_instantiate_SOAP_ENV__Detail(struct soap*, int, const char*, const char*, size_t*);
};
#endif
#endif

/* service3.h:563 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (26)
/* SOAP_ENV__Reason: */
struct SOAP_CMAC SOAP_ENV__Reason {
      public:
        /** Optional element 'SOAP-ENV:Text' of XSD type 'xsd:string' */
        char *SOAP_ENV__Text;
      public:
        /** Return unique type id SOAP_TYPE_SOAP_ENV__Reason */
        long soap_type() const { return SOAP_TYPE_SOAP_ENV__Reason; }
        /** Constructor with member initializations */
        SOAP_ENV__Reason() : SOAP_ENV__Text() { }
        /** Friend allocator */
        friend SOAP_FMAC1 SOAP_ENV__Reason * SOAP_FMAC2 soap_instantiate_SOAP_ENV__Reason(struct soap*, int, const char*, const char*, size_t*);
};
#endif
#endif

/* service3.h:563 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (27)
/* SOAP_ENV__Fault: */
struct SOAP_CMAC SOAP_ENV__Fault {
      public:
        /** Optional element 'faultcode' of XSD type 'xsd:QName' */
        char *faultcode;
        /** Optional element 'faultstring' of XSD type 'xsd:string' */
        char *faultstring;
        /** Optional element 'faultactor' of XSD type 'xsd:string' */
        char *faultactor;
        /** Optional element 'detail' of XSD type 'SOAP-ENV:Detail' */
        struct SOAP_ENV__Detail *detail;
        /** Optional element 'SOAP-ENV:Code' of XSD type 'SOAP-ENV:Code' */
        struct SOAP_ENV__Code *SOAP_ENV__Code;
        /** Optional element 'SOAP-ENV:Reason' of XSD type 'SOAP-ENV:Reason' */
        struct SOAP_ENV__Reason *SOAP_ENV__Reason;
        /** Optional element 'SOAP-ENV:Node' of XSD type 'xsd:string' */
        char *SOAP_ENV__Node;
        /** Optional element 'SOAP-ENV:Role' of XSD type 'xsd:string' */
        char *SOAP_ENV__Role;
        /** Optional element 'SOAP-ENV:Detail' of XSD type 'SOAP-ENV:Detail' */
        struct SOAP_ENV__Detail *SOAP_ENV__Detail;
      public:
        /** Return unique type id SOAP_TYPE_SOAP_ENV__Fault */
        long soap_type() const { return SOAP_TYPE_SOAP_ENV__Fault; }
        /** Constructor with member initializations */
        SOAP_ENV__Fault() : faultcode(), faultstring(), faultactor(), detail(), SOAP_ENV__Code(), SOAP_ENV__Reason(), SOAP_ENV__Node(), SOAP_ENV__Role(), SOAP_ENV__Detail() { }
        /** Friend allocator */
        friend SOAP_FMAC1 SOAP_ENV__Fault * SOAP_FMAC2 soap_instantiate_SOAP_ENV__Fault(struct soap*, int, const char*, const char*, size_t*);
};
#endif
#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/


/* (built-in):0 */
#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (5)
typedef char *_XML;
#endif

/* (built-in):0 */
#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (6)
typedef char *_QName;
#endif

/******************************************************************************\
 *                                                                            *
 * Serializable Types                                                         *
 *                                                                            *
\******************************************************************************/


/* char has binding name 'byte' for type 'xsd:byte' */
#ifndef SOAP_TYPE_byte
#define SOAP_TYPE_byte (3)
#endif

/* int has binding name 'int' for type 'xsd:int' */
#ifndef SOAP_TYPE_int
#define SOAP_TYPE_int (1)
#endif

/* ns1__startGuiResponse has binding name 'ns1__startGuiResponse' for type 'ns1:startGuiResponse' */
#ifndef SOAP_TYPE_ns1__startGuiResponse
#define SOAP_TYPE_ns1__startGuiResponse (10)
#endif

/* ns1__startGui has binding name 'ns1__startGui' for type 'ns1:startGui' */
#ifndef SOAP_TYPE_ns1__startGui
#define SOAP_TYPE_ns1__startGui (9)
#endif

/* ns1__getUdpUrlResponse has binding name 'ns1__getUdpUrlResponse' for type 'ns1:getUdpUrlResponse' */
#ifndef SOAP_TYPE_ns1__getUdpUrlResponse
#define SOAP_TYPE_ns1__getUdpUrlResponse (8)
#endif

/* ns1__getUdpUrl has binding name 'ns1__getUdpUrl' for type 'ns1:getUdpUrl' */
#ifndef SOAP_TYPE_ns1__getUdpUrl
#define SOAP_TYPE_ns1__getUdpUrl (7)
#endif

/* struct SOAP_ENV__Fault has binding name 'SOAP_ENV__Fault' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (27)
#endif

/* struct SOAP_ENV__Reason has binding name 'SOAP_ENV__Reason' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (26)
#endif

/* struct SOAP_ENV__Detail has binding name 'SOAP_ENV__Detail' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (23)
#endif

/* struct SOAP_ENV__Code has binding name 'SOAP_ENV__Code' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (21)
#endif

/* struct SOAP_ENV__Header has binding name 'SOAP_ENV__Header' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (20)
#endif

/* struct SOAP_ENV__Reason * has binding name 'PointerToSOAP_ENV__Reason' for type '' */
#ifndef SOAP_TYPE_PointerToSOAP_ENV__Reason
#define SOAP_TYPE_PointerToSOAP_ENV__Reason (29)
#endif

/* struct SOAP_ENV__Detail * has binding name 'PointerToSOAP_ENV__Detail' for type '' */
#ifndef SOAP_TYPE_PointerToSOAP_ENV__Detail
#define SOAP_TYPE_PointerToSOAP_ENV__Detail (28)
#endif

/* struct SOAP_ENV__Code * has binding name 'PointerToSOAP_ENV__Code' for type '' */
#ifndef SOAP_TYPE_PointerToSOAP_ENV__Code
#define SOAP_TYPE_PointerToSOAP_ENV__Code (22)
#endif

/* ns1__getUdpUrl * has binding name 'PointerTons1__getUdpUrl' for type 'ns1:getUdpUrl' */
#ifndef SOAP_TYPE_PointerTons1__getUdpUrl
#define SOAP_TYPE_PointerTons1__getUdpUrl (16)
#endif

/* ns1__startGui * has binding name 'PointerTons1__startGui' for type 'ns1:startGui' */
#ifndef SOAP_TYPE_PointerTons1__startGui
#define SOAP_TYPE_PointerTons1__startGui (12)
#endif

/* _QName has binding name '_QName' for type 'xsd:QName' */
#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (6)
#endif

/* _XML has binding name '_XML' for type '' */
#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (5)
#endif

/* char * has binding name 'string' for type 'xsd:string' */
#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string (4)
#endif

/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Client-Side Call Stub Functions                                            *
 *                                                                            *
\******************************************************************************/

    SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__startGui(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns1__startGui *ns1__startGui_, ns1__startGuiResponse &ns1__startGuiResponse_);
    SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__getUdpUrl(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns1__getUdpUrl *ns1__getUdpUrl_, ns1__getUdpUrlResponse &ns1__getUdpUrlResponse_);

#endif

/* End of soapStub.h */