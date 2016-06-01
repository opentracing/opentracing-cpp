#include <opentracing/tracer.h>
#include <opentracing/span.h>

namespace opentracing
{

Span::~Span()
{
}

Span * Span::setOperationName(const std::string & operationName)
{
    (void)operationName;
    
    return this;
}

Span * Span::setTag(const std::string & key, const std::string & value)
{
    (void)key;
    (void)value;
            
    return this;
}

Span * Span::log(const LogData & data)
{
    (void)data;

    return this;
}

Span * Span::log(const std::vector<LogData> & logData)
{
	for (std::vector<LogData>::const_iterator i = logData.begin(); i != logData.end(); ++i)
	{
		log(*i);
	}
        return this;
}

Span * Span::setBaggageItem(const std::string & restrictedKey, const std::string & value)
{
    (void)restrictedKey;
    (void)value;

    return this;
}

bool Span::getBaggageItem(const std::string & restrictedKey, std::string * targetValue) const
{
    (void)restrictedKey;
    (void)targetValue;

    return false;
}

void Span::finish()
{
	finish(0);
}

void Span::finish(const uint64_t & finishTime)
{
    (void)finishTime;
}

LogData::LogData(const std::string & event_, const Payload & payload_)
    : timestamp(0)
    , event(event_)
    , payload(payload_)
{
}

LogData::LogData(const uint64_t & timestamp_, const std::string & event_, const Payload & payload_)
    : timestamp(timestamp_)
    , event(event_)
    , payload(payload_)
{
}

bool canonicalizeBaggageKey(std::string & key)
{
    if (key.empty())
    {
        return false;
    }
    
    for (std::string::iterator i = key.begin(); i != key.end(); ++i)
    {
        *i = tolower(*i);
    }

    for (std::string::iterator i = key.begin(); i != key.end(); ++i)
    {
        char c(*i);

        if ((c>='a' && c<='z') || (c>='0' && c<='9') || (c=='-' && i != key.begin()))
        {
            // This is ok.
        }
        else
        {
        	return false;
        }
    }
    
    return true;
}

}
