/*
 * clone.cpp
 *
 * A content object that simply copies some other piece of content; effectively allowing multiple placements of the same content, without creating a full copy.
 */


#include "AnsiGL/clone.h"


namespace AnsiGL
{
    Clone::Clone()
    {
    }

    Clone::Clone( Content::Ptr target ):
	m_Target( target )
    {
    }

    Clone::~Clone()
    {
    }

    Content::Ptr Clone::Target() const
    {
	return m_Target.lock();
    }

    void Clone::Target( Content::Ptr target )
    {
	m_Target = target;

	if ( target )
	    this->m_Size = target->Size();
	else
	    this->m_Size = Area2D(0, 0);
    }

    std::string Clone::str()
    {
	Content::Ptr MyTarget = Target();

	if ( MyTarget )
	    return MyTarget->str();

	return std::string();
    }

    std::string Clone::Render() const
    {
	Content::Ptr MyTarget = Target();

	if ( MyTarget )
	    return MyTarget->Render();

	return std::string();
    }

    void Clone::RenderToSurface( Surface::Ptr dest, const Point2D &pos ) const
    {
	Content::Ptr MyTarget = Target();

	if ( MyTarget )
	    MyTarget->RenderToSurface( dest, pos );
    }
}



