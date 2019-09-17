#ifndef OPENTRACING_EXT_TAGS_H
#define OPENTRACING_EXT_TAGS_H

#include <opentracing/string_view.h>
#include <opentracing/symbols.h>
#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace ext {
// The following tags are described in greater detail at the following url:
// https://github.com/opentracing/specification/blob/master/semantic_conventions.md
//
// Here we define standard names for tags that can be added to spans by the
// instrumentation code. The actual tracing systems are not required to
// retain these as tags in the stored spans if they have other means of
// representing the same data. For example, the SPAN_KIND='server' can be
// inferred from a Zipkin span by the presence of ss/sr annotations.

// ---------------------------------------------------------------------------
// span_kind hints at relationship between spans, e.g. client/server
// ---------------------------------------------------------------------------
OPENTRACING_API extern const opentracing::string_view span_kind;

// Marks a span representing the client-side of an RPC or other remote call
OPENTRACING_API extern const opentracing::string_view span_kind_rpc_client;

// Marks a span representing the server-side of an RPC or other remote call
OPENTRACING_API extern const opentracing::string_view span_kind_rpc_server;

// ---------------------------------------------------------------------------
// error indicates whether a Span ended in an error state.
// ---------------------------------------------------------------------------
OPENTRACING_API extern const opentracing::string_view error;

// ---------------------------------------------------------------------------
// component (string) ia s low-cardinality identifier of the module, library,
// or package that is generating a span.
// ---------------------------------------------------------------------------
OPENTRACING_API extern const opentracing::string_view component;

// ---------------------------------------------------------------------------
// sampling_priority (uint16) determines the priority of sampling this Span.
// ---------------------------------------------------------------------------
OPENTRACING_API extern const opentracing::string_view sampling_priority;

// ---------------------------------------------------------------------------
// peer_* tags can be emitted by either client-side of server-side to describe
// the other side/service in a peer-to-peer communications, like an RPC call.
// ---------------------------------------------------------------------------
// peer_service (string) records the service name of the peer
OPENTRACING_API extern const opentracing::string_view peer_service;

// peer_hostname (string) records the host name of the peer
OPENTRACING_API extern const opentracing::string_view peer_hostname;

// peer_address (string) suitable for use in a networking client library.
// This may be a "ip:port", a bare "hostname", a FQDN, or even a
// JDBC substring like "mysql://prod-db:3306"
OPENTRACING_API extern const opentracing::string_view peer_address;

// peer_host_ipv4 (uint32) records IP v4 host address of the peer
OPENTRACING_API extern const opentracing::string_view peer_host_ipv4;

// peer_host_ipv6 (string) records IP v6 host address of the peer
OPENTRACING_API extern const opentracing::string_view peer_host_ipv6;

// peer_port (uint16) records port number of the peer
OPENTRACING_API extern const opentracing::string_view peer_port;

// ---------------------------------------------------------------------------
// HTTP tags
// ---------------------------------------------------------------------------

// http_url (string) should be the URL of the request being handled in this
// segment of the trace, in standard URI format. The protocol is optional.
OPENTRACING_API extern const opentracing::string_view http_url;

// http_method (string) is the HTTP method of the request.
// Both upper/lower case values are allowed.
OPENTRACING_API extern const opentracing::string_view http_method;

// http_status_code (int) is the numeric HTTP status code (200, 404, etc)
// of the HTTP response.
OPENTRACING_API extern const opentracing::string_view http_status_code;

// ---------------------------------------------------------------------------
// DATABASE tags
// ---------------------------------------------------------------------------

// database_instance (string) The database instance name. E.g., In java, if
// the jdbc.url="jdbc:mysql://127.0.0.1:3306/customers", the instance
// name is "customers"
OPENTRACING_API extern const opentracing::string_view database_instance;

// database_statement (string) A database statement for the given database
// type. E.g., for db.type="SQL", "SELECT * FROM user_table";
// for db.type="redis", "SET mykey 'WuValue'".
OPENTRACING_API extern const opentracing::string_view database_statement;

// database_type (string) For any SQL database, "sql". For others,
// the lower-case database category, e.g. "cassandra", "hbase", or "redis".
OPENTRACING_API extern const opentracing::string_view database_type;

// database_user (string) Username for accessing database. E.g.,
// "readonly_user" or "reporting_user"
OPENTRACING_API extern const opentracing::string_view database_user;

// ---------------------------------------------------------------------------
// message_bus tags
// ---------------------------------------------------------------------------

// message_bus_destination (string) An address at which messages can be
// exchanged. E.g. A Kafka record has an associated "topic name" that can
// be extracted by the instrumented producer or consumer and stored
// using this tag.
OPENTRACING_API extern const opentracing::string_view message_bus_destination;
}  // namespace ext
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_EXT_TAGS_H
