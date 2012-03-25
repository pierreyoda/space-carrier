#ifndef PYTHONEMBEDDER_HPP
#define PYTHONEMBEDDER_HPP

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/filesystem.hpp>

// A macro to simplify overloaded member function (=method) binding
#define method(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS))&CLASS::METHOD)
// same but with arguments - const version
#define method_const(CLASS, METHOD, RTYPE, ARGS...) \
    def(BOOST_PP_STRINGIZE(METHOD), \
        (RTYPE(CLASS::*)(ARGS)const)&CLASS::METHOD)


/** \brief A class that handles the Python interpretor and provides functions to deal with.
*
*/
class PythonEmbedder
{
    public:
        PythonEmbedder();
        ~PythonEmbedder();

        /** \brief Init the Python interpreter.
        *This function MUST be called before other Python-related ones, but after module export.
        *
        * \return True if successful, false otherwise.
        *
        */
        bool init();

        /** \brief Import a module.
        *
        *Unlike boost::python::import, absolute path is handled.
        *
        * \param path boost::path to module.
        * \param Python object to assign as the loaded module, which will be none if failed (returning a reference seems to crash...)
        *
        */
        void importModule(const boost::filesystem3::path &path, boost::python::object &module);

        /** \brief Parse a python exception.
        *
        * \author Python Wiki, : http://wiki.python.org/moin/boost.python/EmbeddingPython
        *
        * \return Python exception.
        *
        */
        static std::string parsePythonException();

        /** \brief Determine if a caught error_already_set exception is a Python exception.
        *
        * \return Is error a python exception?
        *
        */
        static bool isPythonException();

        /** \brief Get the Python main module.
        *
        * \return Reference to python main module.
        *
        */
        boost::python::object &mainModule() { return main_module; }

        /** \brief Get the Python main namespace.
        *
        * \return Reference to python main namespace.
        *
        */
        boost::python::object &getGlobal() { return global; }

        /** \brief Export Oiw engine classes into Python.
        *Must be called in BOOST_PYTHON_MODULE block.\n\n
        *
        */
        static void exportOiwEngine();
        /** \brief Export Space Carrier engine classes into Python.
        *Must be called in BOOST_PYTHON_MODULE block.\n\n
        *
        */
        static void exportSpaceCarrier();
        /** \brief Export Thor classes into Python.
        *Must be called in BOOST_PYTHON_MODULE block.\n\n
        */
        static void exportThor();
        /** \brief Export SFML classes into Python.
        *Must be called in BOOST_PYTHON_MODULE block.\n\n
        *Note : SFML exporting could have been done by pysfml2-cython, but
        *I found no way to make it work properly with my embedded Python.
        */
        static void exportSf();

        /** \brief Export SFML Window classes into Python.
        *Otherwise provoked a "too large file" error.
        */
        static void exportSfWindow();

    private:

        boost::python::object main_module, global;
};

#endif /* PYTHONEMBEDDER_HPP */
