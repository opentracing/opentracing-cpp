#ifndef INCLUDED_OPENTRACING_TRACERGUARDS_H
#define INCLUDED_OPENTRACING_TRACERGUARDS_H

// ========
// guards.h
// ========
// class GenericTracerGuard - An RAII wrapper to manage Tracer resources

namespace opentracing {

// ========================
// class GenericTracerGuard
// ========================
// GenericTracerGuards are templated types used to automatically return
// resources to the installed Tracer when they go out of scope.
// The GenericTracerGuard own and manage another object through an installed
// pointer. Upon destruction, if the guard is still managing the object it
// will be automatically returned to the installed TRACER through the
// 'cleanup()' interface.
//
// When GenericTracerGuards are copied or assigned, the ownership of the
// managed objects will shift from the source to the destinations.
template <typename TRACER, typename MANAGED>
class GenericTracerGuard {
  public:
    explicit GenericTracerGuard(MANAGED* obj);
    // Construct a guard managing the supplied object.

    GenericTracerGuard& operator=(MANAGED* other);
    // Take ownership of any resources that may be managed by 'other'.

    ~GenericTracerGuard();
    // If an object is managed, return it to the Tracer, then
    // destroy this guard.

    MANAGED* get() const;
    // Return the pointer managed by this guard. May be NULL.

    MANAGED& operator*() const;
    // Return a reference to underlying object. Undefined behavior if
    // this guard is not currently managing an object.

    MANAGED* operator->() const;
    // Return the pointer managed by this guard. May be NULL.

    MANAGED* release();
    // Return the pointer managed by this guard, then reset the guard.
    // The object returned will have to have its lifetime managed explicitly.

    void reset(MANAGED* const obj = NULL);
    // Reset this guard, potentially cleaning up after an object if one is
    // currently being managed.

  private:
    GenericTracerGuard(const GenericTracerGuard&);
    GenericTracerGuard& operator=(GenericTracerGuard& other);
    // Disallow

    MANAGED* m_managed;
};

// ------------------------
// class GenericTracerGuard
// ------------------------

template <typename TRACER, typename MANAGED>
inline GenericTracerGuard<TRACER, MANAGED>::GenericTracerGuard(
    MANAGED* managed)
: m_managed(managed)
{
}

template <typename TRACER, typename MANAGED>
inline GenericTracerGuard<TRACER, MANAGED>&
GenericTracerGuard<TRACER, MANAGED>::operator=(MANAGED* other)
{
    reset(other);
    return *this;
}

template <typename TRACER, typename MANAGED>
inline GenericTracerGuard<TRACER, MANAGED>::~GenericTracerGuard()
{
    reset();
}

template <typename TRACER, typename MANAGED>
inline MANAGED*
GenericTracerGuard<TRACER, MANAGED>::get() const
{
    return m_managed;
}

template <typename TRACER, typename MANAGED>
inline MANAGED& GenericTracerGuard<TRACER, MANAGED>::operator*() const
{
    return *m_managed;
}

template <typename TRACER, typename MANAGED>
inline MANAGED* GenericTracerGuard<TRACER, MANAGED>::operator->() const
{
    return m_managed;
}

template <typename TRACER, typename MANAGED>
inline MANAGED*
GenericTracerGuard<TRACER, MANAGED>::release()
{
    MANAGED* const tmp = m_managed;
    m_managed          = NULL;
    return tmp;
}

template <typename TRACER, typename MANAGED>
inline void
GenericTracerGuard<TRACER, MANAGED>::reset(MANAGED* const other)
{
    if (m_managed)
    {
        TRACER::instance()->cleanup(m_managed);
    }
    m_managed = other;
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_TRACERGUARDS_H
