#ifndef RUN_HPP
#define RUN_HPP
#include <iostream>
#include <boost/python.hpp>
#include <Python.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/python/module.hpp>
#include <boost/python/module.hpp>
#include <boost/python/object/add_to_namespace.hpp>
#include <QApplication>
using namespace std;
namespace Mushroom
{

	void run_script( const boost::filesystem::path & p )
	{
		boost::filesystem::path path( p );
		path += "/config.xml";
		boost::property_tree::ptree pt;
		boost::property_tree::xml_parser::read_xml( path.c_str( ), pt );
		bool run_script = pt.get<bool>( "script.run", true );
		if ( run_script )
		{
			auto script_type = pt.get<string>( "script.type" );
			assert( script_type == "python2.7" );
			try
			{
				boost::python::object main_module = boost::python::import( "__main__" );
				boost::python::object main_namespace = main_module.attr( "__dict__" );
				boost::python::exec_file( ( p.string( ) + "/" + pt.get<string>( "script.location" ) ).c_str( ), main_namespace );
			}
			catch ( const boost::python::error_already_set & e )
			{
				PyErr_Print( );
				throw e;
			}
		}
	}


	int run( int argc, char * argv[] )
	{
		QApplication app( argc, argv );
		Py_Initialize( );
		boost::ignore_unused_variable_warning( argc );
		boost::filesystem::path p = boost::filesystem::path( argv[0] ).parent_path( );
		p += "/init";
		for_each( boost::filesystem::directory_iterator( p ),
							boost::filesystem::directory_iterator( ), Mushroom::run_script );
		Py_Finalize( );
		cin.get( );
		return app.exec( );
	}

}
#endif // RUN_HPP
