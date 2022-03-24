// (c) 2013-2020 David Gerstl, all rights reserved
// For the use of my C++ students to use as a base to implement
// dynamic arrays, exceptions and operator overloading, and templates


// like #pragma once but more portable
#ifndef H_FARMINGDALESTATUS
#define H_FARMINGDALESTATUS
#pragma warning(disable : 26812) // disable unscoped enum "suggestion"

namespace farmingdale {
	// Note: namespaces are additive, so you can add new elements to this namespace in other files
	// while keeping this as the only definition of statusCode is acceptable.
	enum statusCode { SUCCESS, FAILURE }; // you will remove these if you create exceptions
}
   // to use these constants, specify farmingdale::statusCode::farmingdale::statusCode::SUCCESS or farmingdale::statusCode::farmingdale::statusCode::FAILURE in your code

#endif // ndef H_FARMINGDALESTATUS