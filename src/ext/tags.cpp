#include <opentracing/ext/tags.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace ext {
const opentracing::string_view span_kind = "span.kind";
const opentracing::string_view span_kind_rpc_client = "client";
const opentracing::string_view span_kind_rpc_server = "server";

const opentracing::string_view error = "error";

const opentracing::string_view component = "component";

const opentracing::string_view sampling_priority = "sampling.priority";

const opentracing::string_view peer_service = "peer.service";

extern const opentracing::string_view peer_hostname = "peer.hostname";
extern const opentracing::string_view peer_address = "peer.address";
extern const opentracing::string_view peer_host_ipv4 = "peer.ipv4";
extern const opentracing::string_view peer_host_ipv6 = "peer.ipv6";
extern const opentracing::string_view peer_port = "peer.port";

extern const opentracing::string_view http_url = "http.url";
extern const opentracing::string_view http_method = "http.method";
extern const opentracing::string_view http_status_code = "http.status_code";

extern const opentracing::string_view database_instance = "db.instance";
extern const opentracing::string_view database_statement = "db.statement";
extern const opentracing::string_view database_type = "db.type";
extern const opentracing::string_view database_user = "db.user";

extern const opentracing::string_view message_bus_destination =
    "message_bus.destination";
}  // namespace ext
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
