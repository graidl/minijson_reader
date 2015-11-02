// Just a simple test program for minijson_reader
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "mh_util.h"
#include "mh_param.h"
#include "mh_random.h"
#include "mh_log.h"

#include "minijson_reader.hpp"


/** Name of json input file. */
string_param jsonfile("ifile","name of json input file","i.json");


// This is the exemplary hierarchical object we want to read in:
struct obj_type
{
    long field1 = 0;
    std::string field2; // you can use a const char*, but
                        // in that case beware of lifetime!
    struct
    {
        double f1 = 0;
        bool f2 = false;
    } nested;
    std::vector<int> array;
} obj;

using namespace minijson;

void testjson() {
	// Read in from an istream_context associated with a file
	ifstream f(jsonfile().c_str());
	minijson::istream_context ctx(f);
	// Read in an obj
	parse_object(ctx, [&](const char* k, value v) {
			dispatch (k)
			<<"field1">> [&]{ obj.field1 = v.as_long(); }
			<<"field2">> [&]{ obj.field2 = v.as_string(); }
			<<"nested">> [&]
			{
				parse_object(ctx, [&](const char* k, value v)
				{
					dispatch (k)
					<<"f1">> [&]{ obj.nested.f1 = v.as_double(); }
					<<"f2">> [&]{ obj.nested.f2 = v.as_bool(); }
					<<any>> [&]{ minijson::ignore(ctx); };
				});
			}
			<<"array">> [&]
			{
				parse_array(ctx, [&](value v)
				{
					obj.array.push_back(v.as_long());
				});
			}
			<<any>> [&]{ minijson::ignore(ctx); };
		});
	// Read in a simple single integer 
	int intobj;
	parse_object(ctx, [&](const char* k, value v) {
			dispatch (k)
			<<"intobj">> [&]{ intobj = int(v.as_long()); };
	});

	// Write out some of the fields:
	out() << "intobj: " << intobj << endl;
	out() << "field1: " << obj.field1 << endl;
	out() << "field2: " << obj.field2 << endl;
	out() << "f1: " << obj.nested.f1 << endl;
	out() << "f2: " << obj.nested.f2 << endl;
	out() << "array: ";
	for (auto a : obj.array) {
			out() << a << " ";
	}
	out() << endl;

	// Also test a comment within a const_buffer_context
	const char *b = "{\"a\":88#3\n}";
	minijson::const_buffer_context ctxb(b, strlen(b));
	int a;
	parse_object(ctxb, [&](const char* k, value v) {
			dispatch (k)
			<<"a">> [&]{ a = v.as_long(); };
			});
	out() << "a: " << a << endl;
}


int main(int argc, char *argv[])
{
	try 
	{
		// Probably set some parameters to new default values
		//pmut.setDefault(2);
		
		// parse arguments and initialize random number generator
		param::parseArgs(argc,argv);
		random_seed();

		/* initialize out() stream for standard output and logstr object for logging
		   according to set parameters. */
		initOutAndLogstr();

		out() << "#--------------------------------------------------" 
			<< endl;
		out() << "# ";
		for (int i=0;i<argc;i++)
			out() << argv[i] << ' ';
		out() << endl;
		out() << "#--------------------------------------------------" 
			<< endl;
		out () << "# " << mhversion() << endl;
		param::printAll(out());
		out() << endl;

		testjson();

		out() << "All went well!" << endl;
	}
	// catch all exceptions and write error message
	catch (std::string &s)
	{ cerr << s << endl; return 1; }
	catch (exception &e)
	{ cerr << "Standard exception occured: " << e.what() << endl; return 1; }
	catch (...)
	{ cerr << "Unknown exception occured" << endl; return 1; }
	return 0;
}
