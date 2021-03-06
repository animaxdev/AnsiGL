#ifndef __ANSIGL_SCREEN__
#define __ANSIGL_SCREEN__

/*
 * screen.h
 *
 * The most basic context that can be instanciated.
 */


#include "context.h"


namespace AnsiGL
{
	class Screen : public Context
	{
	public:
		ANSIGL_POINTERS( Screen )

	protected:

	public:
		~Screen();

		virtual std::string str() const;
		virtual std::string Render() const;
		virtual void RenderToSurface( Surface_Ptr dest, Point2D pos = Point2D(), bool transparentSpaces = false );
	};
}


#endif // __ANSIGL_SCREEN__


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --indent-switches --pad-oper


