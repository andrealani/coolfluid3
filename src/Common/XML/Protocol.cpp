// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "rapidxml/rapidxml.hpp"

#include "Common/BasicExceptions.hpp"
#include "Common/Log.hpp"
#include "Common/XML/XmlDoc.hpp"

#include "Common/XML/Protocol.hpp"

#include "Math/MatrixTypes.hpp"

/////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {
namespace XML {

/////////////////////////////////////////////////////////////////////////////////

  const char * Protocol::Tags::attr_array_delimiter() { return "delimiter"; }

  const char * Protocol::Tags::attr_array_size() { return "size"; }

  const char * Protocol::Tags::attr_array_type() { return "type"; }

  const char * Protocol::Tags::attr_clientid() { return "clientid"; }

  const char * Protocol::Tags::attr_descr() { return "descr"; }

  const char * Protocol::Tags::attr_frameid() { return "frameid"; }

const char * Protocol::Tags::attr_key() { return "key"; }

const char * Protocol::Tags::attr_pretty_name() { return "pretty_name"; }

const char * Protocol::Tags::key_restricted_values() { return "restrictedValues"; }

  const char * Protocol::Tags::attr_uri_schemes() { return "schemes"; }

  const char * Protocol::Tags::key_options() { return "options"; }

  const char * Protocol::Tags::key_properties() { return "properties"; }

  const char * Protocol::Tags::key_signals() { return "signals"; }

  const char * Protocol::Tags::node_array() { return "array"; }

  const char * Protocol::Tags::node_doc() { return "cfxml"; }

  const char * Protocol::Tags::node_frame() { return "frame"; }

  const char * Protocol::Tags::node_map() { return "map"; }

  const char * Protocol::Tags::node_value() { return "value"; }

  const char * Protocol::Tags::node_type_reply() { return "reply"; }

  const char * Protocol::Tags::node_type_signal() { return "signal"; }

  template<>
  Common_API const char * Protocol::Tags::type<bool>() { return "bool"; }

  template<>
  Common_API const char * Protocol::Tags::type<int>() { return "integer"; };

  template<>
  Common_API const char * Protocol::Tags::type<CF::Uint>() { return "unsigned"; }

  template<>
  Common_API const char * Protocol::Tags::type<CF::Real>() { return "real"; }

  template<>
  Common_API const char * Protocol::Tags::type<std::string>() { return "string"; }

  template<>
  Common_API const char * Protocol::Tags::type<URI>() { return "uri"; }
  
  template<>
  Common_API const char * Protocol::Tags::type<RealVector>() { return "realvector"; }

/////////////////////////////////////////////////////////////////////////////////

  XmlNode Protocol::goto_doc_node ( const XmlNode& node )
  {
    XmlNode fnode(node);

    if( !fnode.is_valid() )
      throw XmlError(FromHere(), "Node is not valid");

    if ( fnode.content->type() == rapidxml::node_document )
    {
      fnode.content = fnode.content->first_node();
      if ( !fnode.is_valid() )
        throw XmlError( FromHere(), "XML document is empty" );
    }

    if ( fnode.content->type() == rapidxml::node_document )
    {
      fnode.content = fnode.content->next_sibling();
      if ( !fnode.is_valid() )
        throw XmlError( FromHere(), "No xml nodes after declaration" );
    }

    // find the first doc node
    if ( strcmp(fnode.content->name() , Tags::node_doc()) ) /* are not equal */
      fnode.content = fnode.content->next_sibling( Tags::node_doc() );

    if ( !fnode.is_valid() )
      throw Common::XmlError( FromHere(), "No xml doc found" );

    return fnode;
  }

/////////////////////////////////////////////////////////////////////////////////

  XmlNode  Protocol::first_frame_node ( const XmlNode& node )
  {
    cf_assert( node.is_valid() );

    return XmlNode( node.content->first_node( Protocol::Tags::node_frame() ));
  }

/////////////////////////////////////////////////////////////////////////////////

  XmlNode Protocol::add_signal_frame ( XmlNode& node, const std::string & target,
                                       const URI & sender, const URI & receiver,
                                       bool user_trans )
  {
    cf_assert(sender.scheme() == URI::Scheme::CPATH);
    cf_assert(receiver.scheme() == URI::Scheme::CPATH);

    std::string uuid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

    XmlNode signalnode = node.add_node( Tags::node_frame() );

    signalnode.set_attribute( "type", Tags::node_type_signal() );
    signalnode.set_attribute( "target", target );
    signalnode.set_attribute( "sender", sender.string() );
    signalnode.set_attribute( "receiver", receiver.string() );
    signalnode.set_attribute( "transaction", user_trans ? "user" : "auto" );
    signalnode.set_attribute( "frameid", uuid );

    return signalnode;
  }

/////////////////////////////////////////////////////////////////////////////////

  XmlNode Protocol::add_reply_frame ( XmlNode& node )
  {
    cf_assert( node.is_valid() );
    cf_assert( is_not_null(node.content->parent()) );

    rapidxml::xml_node<>* xml_node = node.content;

    XmlNode replynode = XmlNode(node.content->parent()).add_node( Tags::node_frame());

    replynode.set_attribute( "type", Tags::node_type_reply() );

    // reply with same target
    rapidxml::xml_attribute<>* target_att = xml_node->first_attribute("target");
    std::string target = is_not_null(target_att) ? target_att->value() : "";
    replynode.set_attribute("target", target);

    // the sender becomes the receiver
    rapidxml::xml_attribute<>* sender_att = xml_node->first_attribute("sender");
    std::string receiver = is_not_null(sender_att) ? sender_att->value() : "";
    replynode.set_attribute("receiver", receiver);

    // same transaction type
    rapidxml::xml_attribute<>* trans_att = xml_node->first_attribute("transaction");
    std::string trans = is_not_null(trans_att) ? trans_att->value() : "auto";
    replynode.set_attribute("transaction", trans);

    // copy uuids, if any
    rapidxml::xml_attribute<>*  client_uuid_att = xml_node->first_attribute( Tags::attr_clientid() );
    rapidxml::xml_attribute<>*  frame_uuid_att = xml_node->first_attribute( Tags::attr_frameid() );

    if( is_not_null(client_uuid_att) )
      replynode.set_attribute(Tags::attr_clientid(), client_uuid_att->value());

    if( is_not_null(frame_uuid_att) )
      replynode.set_attribute(Tags::attr_frameid(), frame_uuid_att->value() );

    return replynode;
//    return XmlNode();
  }

/////////////////////////////////////////////////////////////////////////////////

  boost::shared_ptr<XmlDoc> Protocol::create_doc ()
  {
    boost::shared_ptr<XmlDoc> xmldoc ( new XmlDoc("1.0", "UTF-8") );
    XmlDoc& doc = *xmldoc.get();

    // add root node
    XmlNode root = doc.add_node( Tags::node_doc() );

    root.set_attribute("version", "1.0");
    root.set_attribute("type", "message");

    return xmldoc;
  }

/////////////////////////////////////////////////////////////////////////////////

} // XML
} // Common
} // CF
