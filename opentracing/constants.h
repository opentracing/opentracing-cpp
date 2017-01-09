#ifndef INCLUDED_OPENTRACING_CONSTANTS_H
#define INCLUDED_OPENTRACING_CONSTANTS_H

// ===========
// constants.h
// ===========
// This header provides a set of macros for the "standard" OpenTracing tag and log fields.
// See https://github.com/opentracing/specification/blob/master/data_conventions.yaml for
// a description of each value, and how it should be used.

#define OPENTRACING_TAG_ERROR             "error"
#define OPENTRACING_TAG_COMPONENT         "component"
#define OPENTRACING_TAG_SAMPLING_PRIORITY "sampling.priority "

#define OPENTRACING_TAG_HTTP_URL          "http.url"
#define OPENTRACING_TAG_HTTP_METHOD       "http.method"
#define OPENTRACING_TAG_HTTP_STATUS       "http.status"

#define OPENTRACING_TAG_SPAN_KIND         "span.kind"
#define OPENTRACING_TAG_PEER_HOSTNAME     "peer.hostname"
#define OPENTRACING_TAG_PEER_IPV4         "peer.ipv4"
#define OPENTRACING_TAG_PEER_IPV6         "peer.ipv6"
#define OPENTRACING_TAG_PEER_PORT         "peer.port"
#define OPENTRACING_TAG_PEER_SERVICE      "peer.service"

#define OPENTRACING_LOG_EVENT             "event"

#endif
