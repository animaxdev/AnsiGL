/*
 * color.cpp
 *
 * ANSI and colors and palettes, oh my!
 */


#include <sstream>

#include "AnsiGL/color.h"


namespace AnsiGL
{
    ColorComponent::ColorComponent():
	m_Index(-1)
    {
	assignMasterPalette();
    }

    ColorComponent::ColorComponent( ANSIColorPalette::Ptr palette ):
	m_Palette(palette),
	m_Index(-1)
    {
	// This is one of the only 2 constructors that do NOT initialize the ANSIColorPalette::Master!
    }

    ColorComponent::ColorComponent( ANSIColorPalette::Ptr palette, const ANSIColorDef &color ):
	m_Palette(palette),
	m_Index(-1)
    {
	// This is one of the only 2 constructors that do NOT initialize the ANSIColorPalette::Master!

	if ( m_Palette )
	    m_Index = m_Palette->FindIndex( color );
    }

    ColorComponent::ColorComponent( ENUM_ANSISystemColors color ):
	m_Index(-1)
    {
	assignMasterPalette();
	Set( color );
    }

    ColorComponent::ColorComponent( unsigned char r, unsigned char g, unsigned char b ):
	m_Index(-1)
    {
	assignMasterPalette();
	Set( r, g, b );
    }

    ColorComponent::ColorComponent( unsigned char grayscale ):
	m_Index(-1)
    {
	assignMasterPalette();
	Set( grayscale );
    }

    bool ColorComponent::operator==( const ColorComponent &right ) const
    {
	if ( this == &right )
	    return true;

	if ( m_Index == -1 && right.m_Index == -1 )
	    return true;

	if ( m_Palette && right.m_Palette )
	{
	    if ( m_Index >= 0 && m_Index < (int)m_Palette->size() && right.m_Index >= 0 && right.m_Index < (int)right.m_Palette->size() )
	    {
		// m_Palette is a pointer, and so is (*m_Palette)[index]...hence the double deref
		if ( (*(*m_Palette)[m_Index]) == (*(*right.m_Palette)[right.m_Index]) )
		    return true;
	    }
	}

	return false;
    }

    bool ColorComponent::operator!=( const ColorComponent &right ) const
    {
	return !((*this) == right);
    }

    ANSIColorPalette::Ptr ColorComponent::Palette() const
    {
	return m_Palette;
    }

    void ColorComponent::Palette( ANSIColorPalette::Ptr palette )
    {
	m_Palette = palette;

	// Check our index bounds against the new palette...if we're still in range, allow it...if not, become transparent
	if ( !m_Palette || (m_Palette && m_Index >= (int)m_Palette->size()) )
	    m_Index = -1;
    }

    int ColorComponent::Index() const
    {
	return m_Index;
    }

    void ColorComponent::Set( ENUM_ANSISystemColors color )
    {
	if ( !m_Palette )
	    return;

	m_Index = m_Palette->FindIndex( color );
    }

    void ColorComponent::Set( unsigned char r, unsigned char g, unsigned char b )
    {
	if ( !m_Palette )
	    return;

	m_Index = m_Palette->FindIndex( r, g, b );
    }

    void ColorComponent::Set( unsigned char grayscale )
    {
	if ( !m_Palette )
	    return;

	m_Index = m_Palette->FindIndex( grayscale );
    }

    ANSIColorDef::Ptr ColorComponent::Color() const
    {
	//if ( m_Palette && m_Index != -1 && m_Index < m_Palette->size() )
	if ( m_Palette && m_Index != -1 )
	    return (*m_Palette)[m_Index];

	return ANSIColorDef::Ptr();
    }

    bool ColorComponent::IsColorless() const
    {
	return (!m_Palette || m_Index == -1);
    }

    void ColorComponent::Clear()
    {
	m_Palette.reset();
	m_Index = -1;
    }

    std::string ColorComponent::Render( ENUM_ColorDepth desiredDepth, bool background ) const
    {
	if ( IsColorless() || m_Index >= (int)m_Palette->size() )
	    return std::string("");

	return (*m_Palette)[m_Index]->Render( desiredDepth, background );
    }

    void ColorComponent::assignMasterPalette()
    {
	if ( !ANSIColorPalette::Master )
	    ANSIColorPalette::InitMasterPalette();

	m_Palette = ANSIColorPalette::Master;
    }


    ColorDef::ColorDef():
	Inverted(false)
    {
    }

    ColorDef::ColorDef( const ColorComponent &fg, const ColorComponent &bg ):
	FG(fg),
	BG(bg),
	Inverted(false)
    {
    }

    ColorDef::ColorDef( const ColorComponent &fg, const ColorComponent &bg, bool inverted ):
	FG(fg),
	BG(bg),
	Inverted(inverted)
    {
    }

    ColorDef::ColorDef( ENUM_ANSISystemColors fg, ENUM_ANSISystemColors bg ):
	FG(fg),
	BG(bg),
	Inverted(false)
    {
    }

    bool ColorDef::operator==( const ColorDef &right ) const
    {
	if ( Inverted )
	{
	    if ( right.Inverted )
	    {
		// Us inverted, them inverted
		if ( BG == right.BG && FG == right.FG )
		    return true;
	    }
	    else
	    {
		// Us inverted, them not
		if ( BG == right.FG && FG == right.BG )
		    return true;
	    }
	}
	else
	{
	    if ( right.Inverted )
	    {
		// Them inverted, us not
		if ( FG == right.BG && BG == right.FG )
		    return true;
	    }
	    else
	    {
		// Nobody inverted
		if ( FG == right.FG && BG == right.BG )
		    return true;
	    }
	}

	return false;
    }

    bool ColorDef::operator!=( const ColorDef &right ) const
    {
	return !((*this) == right);
    }

    bool ColorDef::IsColorless() const
    {
	return (FG.IsColorless() && BG.IsColorless());
    }

    void ColorDef::SetPalette( ANSIColorPalette::Ptr palette )
    {
	if ( !palette )
	    return;

	FG.Palette( palette );
	BG.Palette( palette );
    }

    void ColorDef::Clear()
    {
	FG.Clear();
	BG.Clear();
	Inverted = false;
    }

    std::string ColorDef::Render( ENUM_ColorDepth desiredDepth ) const
    {
	std::stringstream RenderedStr("");

	if ( IsColorless() )
	    return RenderedStr.str();

	if ( Inverted )
	{
	    RenderedStr << BG.Render( desiredDepth );

	    // Add a separator if needed
	    if ( !FG.IsColorless() && !BG.IsColorless() )
		RenderedStr << ANSI_CODE_SEPARATOR;

	    RenderedStr << FG.Render( desiredDepth, true );	// The true 2nd parameter renders this as a background color instead of as a foreground color
	}
	else
	{
	    RenderedStr << FG.Render( desiredDepth );

	    // Add a separator if needed
	    if ( !FG.IsColorless() && !BG.IsColorless() )
		RenderedStr << ANSI_CODE_SEPARATOR;


	    RenderedStr << BG.Render( desiredDepth, true );	// The true 2nd parameter renders this as a background color instead of as a foreground color
	}

	return RenderedStr.str();
    }


    ColorPalette::ColorPalette()
    {
    }

    ColorDef::Ptr ColorPalette::operator[]( int index )
    {
	if ( empty() || index < 0 || index >= (int)size() )
	    return ColorDef::Ptr();

	// If the index we want is greater than 1/2 of the total palette size...
	if ( index > ((int)size() >> 1) )
	{
	    // Start from the back
	    ColorPalette::reverse_iterator CurColor;
	    int CurIndex = (int)size() - 1;

	    for ( CurColor = rbegin(); CurColor != rend(); ++CurColor, --CurIndex )
	    {
		if ( CurIndex == index )
		    return (*CurColor);
	    }
	}
	else
	{
	    // Start from the front
	    ColorPalette::iterator CurColor;
	    int CurIndex = 0;

	    for ( CurColor = begin(); CurColor != end(); ++CurColor, ++CurIndex )
	    {
		if ( CurIndex == index )
		    return (*CurColor);
	    }
	}

	return ColorDef::Ptr();
    }

    const ColorDef::Ptr ColorPalette::operator[]( int index ) const
    {
	if ( empty() || index < 0 || index >= (int)size() )
	    return ColorDef::Ptr();

	// If the index we want is greater than 1/2 of the total palette size...
	if ( index > ((int)size() >> 1) )
	{
	    // Start from the back
	    ColorPalette::const_reverse_iterator CurColor;
	    int CurIndex = (int)size() - 1;

	    for ( CurColor = rbegin(); CurColor != rend(); ++CurColor, --CurIndex )
	    {
		if ( CurIndex == index )
		    return (*CurColor);
	    }
	}
	else
	{
	    // Start from the front
	    ColorPalette::const_iterator CurColor;
	    int CurIndex = 0;

	    for ( CurColor = begin(); CurColor != end(); ++CurColor, ++CurIndex )
	    {
		if ( CurIndex == index )
		    return (*CurColor);
	    }
	}

	return ColorDef::Ptr();
    }

    ColorPalette::iterator ColorPalette::begin()
    {
	return m_Colors.begin();
    }

    ColorPalette::iterator ColorPalette::end()
    {
	return m_Colors.end();
    }

    ColorPalette::const_iterator ColorPalette::begin() const
    {
	return m_Colors.begin();
    }

    ColorPalette::const_iterator ColorPalette::end() const
    {
	return m_Colors.end();
    }

    ColorPalette::reverse_iterator ColorPalette::rbegin()
    {
	return m_Colors.rbegin();
    }

    ColorPalette::reverse_iterator ColorPalette::rend()
    {
	return m_Colors.rend();
    }

    ColorPalette::const_reverse_iterator ColorPalette::rbegin() const
    {
	return m_Colors.rbegin();
    }

    ColorPalette::const_reverse_iterator ColorPalette::rend() const
    {
	return m_Colors.rend();
    }

    bool ColorPalette::empty() const
    {
	return m_Colors.empty();
    }

    size_t ColorPalette::size() const
    {
	return m_Colors.size();
    }

    int ColorPalette::Add( ColorDef::Ptr color )
    {
	// If it's colorless, do nothing
	if ( color && color->IsColorless() )
	    return -1;

	// Then try and find the color first...
	int NewIndex = FindIndex( *color );

	// If we found something, use that color rather than adding a duplicate
	if ( NewIndex != -1 )
	    return NewIndex;

	// Otherwise, lets actually add the color
	NewIndex = size();		// The new index will be whatever the current size is, since we count from base 0 with our index
	m_Colors.push_back( color );

	return NewIndex;
    }

    void ColorPalette::Remove( const ColorDef &color )
    {
	if ( empty() )
	    return;

	ColorPalette::iterator CurColor = begin();

	while ( CurColor != end() )
	{
	    // CurColor is an iterator to ColorDef::Ptr...hence the double-dref
	    if ( (*(*CurColor)) == color )
		CurColor = m_Colors.erase( CurColor );
	    else
		++CurColor;
	}
    }

    void ColorPalette::Remove( int index )
    {
	if ( index <= -1 || index >= (int)size() )
	    return;

	if ( empty() )
	    return;

	// If the index we want is greater than 1/2 of the total palette size...
	if ( index > ((int)size() >> 1) )
	{
	    // Start from the back
	    ColorPalette::reverse_iterator CurColor;
	    int CurIndex = size() - 1;

	    for ( CurColor = rbegin(); CurColor != rend(); ++CurColor, --CurIndex )
	    {
		if ( CurIndex == index )
		{
		    m_Colors.erase( --CurColor.base() );
		    break;
		}
	    }
	}
	else
	{
	    // Start from the front
	    ColorPalette::iterator CurColor;
	    int CurIndex = 0;

	    for ( CurColor = begin(); CurColor != end(); ++CurColor, ++CurIndex )
	    {
		if ( CurIndex == index )
		{
		    m_Colors.erase( CurColor );
		    break;
		}
	    }
	}
    }

    int ColorPalette::FindIndex( const ColorDef &color ) const
    {
	if ( empty() )
	    return -1;

	ColorPalette::const_iterator CurColor;
	int ColorIndex = 0;

	for ( CurColor = begin(); CurColor != end(); ++CurColor, ++ColorIndex )
	{
	    // CurColor is an iterator to ColorDef::Ptr...hence the double-dref
	    if ( (*(*CurColor)) == color )
		return ColorIndex;
	}

	return -1;
    }

    void ColorPalette::InvertColors()
    {
	ColorPalette::iterator CurColor;

	for ( CurColor = begin(); CurColor != end(); ++CurColor )
	    (*CurColor)->Inverted = !(*CurColor)->Inverted;
    }

/*
    std::string ColorPalette::Render()
    {
	std::stringstream RenderedStr("");

	ColorPalette::iterator CurColor;

	RenderedStr << "\033[" << ANSI_Default;		// ANSI Code Begin and reset all ANSI attributes

	for ( CurColor = begin(); CurColor != end(); ++CurColor )
	    RenderedStr << ';' << CurColor->Render();

	RetStr << 'm';		// ANSI Code End

	return RenderedStr.str();
    }
*/
}



