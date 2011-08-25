/*
 * =====================================================================================
 *
 *       Filename:  quicklz-module.c
 *
 *    Description: quicklz's python binding 
 *
 *        Version:  0.1
 *        Created:  08/24/2011 07:23:06 AM
 *       Revision:  r1 
 *       Compiler:  gcc
 *
 *         Author: Fu Haiping (email:haipingf@gmail.com)
 *        Company:  ICT,Beijing,China
 *
 * =====================================================================================
 */

/* 
 * quicklz's python binding
 * Copyright (C) 2011 Fu Haiping(email:haipingf@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * */


#define QUICKLZ_PYTHON_MODULE_VERSION "0.1"
#include <Python.h>
#include "quicklz.h"

/*
 * Ensure we have the updated quicklz version
 * */
#if !defined(PY_VERSION_HEX) || (PY_VERSION_HEX < 0X010502F0)
# error "Need Python version 1.5.2 or higher!"
#endif
#if (QLZ_VERSION_MAJOR < 1 && QLZ_VERSION_MINOR < 5)
# error "Need quicklz version 1.5 or higher!"
#endif


static PyObject *QuicklzError;
/* 
 * qlz_size_compressed
 * */
static char qlz_size_compressed_doc[] = 
"qlz_size_compressed(string) -- return string's expected compressed size.\n"
;

static PyObject *
_qlz_size_compressed(PyObject *self, PyObject *args)
{
	PyObject *result;
	const char *input;
	unsigned int size;
	if (!PyArg_ParseTuple(args, "s", &input)) {
		return NULL;
	}
	size = qlz_size_compressed(input);
	result = Py_BuildValue("i", size);
	return result;
}

/* 
 * qlz_size_decompressed
 * */

static char qlz_size_decompressed_doc[] = 
"qlz_size_decompressed(string) -- return string's expected decompressed size.\n"
;

static PyObject *
_qlz_size_decompressed(PyObject *self, PyObject *args)
{
	PyObject *result;
	const char *input;
	unsigned int size;
	if (!PyArg_ParseTuple(args, "s", &input)) {
		return NULL;
	}
	size = qlz_size_decompressed(input);
	result = Py_BuildValue("i", size);
	return result;
}

/*
 * compress
 * */

static char compress_doc[] =
"compress(string) -- Compress string using the default compression level,\n"
"(you should configure compression level in quicklz's source code),\n"
"return a string containing the compressed data.\n"
;
static PyObject *
compress(PyObject *self, PyObject *args)
{
	PyObject *result;
	const char *input;
	unsigned int isize;
	char *output;
	unsigned int osize;
	qlz_state_compress *state_compress = 
		(qlz_state_compress *)malloc(sizeof(qlz_state_compress));
	if (!PyArg_ParseTuple(args, "s#", &input, &isize))
		return NULL;
	output = (char *)malloc((sizeof(char)) 
			* (isize + 4096));
	if (output == NULL) {
		return PyErr_NoMemory();
	}
	osize = qlz_compress(input, output, isize, state_compress);
	if (osize < 0 || osize > (isize + 4096)) {
		free(output);
		PyErr_Format(QuicklzError, "Error while compressing data!");
		return NULL;
	}
	result = Py_BuildValue("s#", output, osize);
	free(output);
	return result;
}
 /*
  * decompress
  * */
static char decompress_doc[] =
"decompress(string) -- Decompress the data in string, returning a new string containing the decompressed data.\n"
;
static PyObject *
decompress(PyObject *self, PyObject *args)
{
	PyObject *result;
	const char *input;
	unsigned int isize;
	char *output;
	unsigned int osize;
	qlz_state_decompress *state_decompress = 
		(qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
	if (!PyArg_ParseTuple(args, "s#", &input, &isize))
		return NULL;
	output = (char *)malloc(sizeof(char)
			*qlz_size_decompressed(input));
	if (output == NULL) {
		return PyErr_NoMemory();
	}
	osize = qlz_decompress(input, output, state_decompress);
	if (osize < 0 || osize < isize) {
		free(output);
		PyErr_Format(QuicklzError, "Error while decompressing data!\n");
		return NULL;
	}
	result = Py_BuildValue("s#", output, osize);
	return result;

}

static PyMethodDef quicklz_methods[] =
{
	{"compress",             (PyCFunction)compress, METH_VARARGS, compress_doc},
	{"decompress",           (PyCFunction)decompress, METH_VARARGS, decompress_doc},
	{"qlz_size_compressed",  (PyCFunction)_qlz_size_compressed, METH_VARARGS, qlz_size_compressed_doc},
	{"qlz_size_decompressed",(PyCFunction)_qlz_size_decompressed, METH_VARARGS, qlz_size_decompressed_doc},
	{NULL, NULL, 0, NULL}
};

static char quicklz_doc[] = 
"The functions in this module allow compression and decompression"
"using the quicklz library.\n\n"
"qlz_size_compressed(string) -- return string's expected compressed size.\n"
"qlz_size_decompressed(string) -- return string's expected decompressed size.\n"
"compress(string) -- Compress a string, and returning a new string containing the compressed data.\n"
"decompress(string) -- Decompress a string , and returning a new string containing the decompressed data.\n"
;

PyMODINIT_FUNC
initquicklz(void)
{
	PyObject *m, *dict, *v;
	m = Py_InitModule3("quicklz", quicklz_methods, quicklz_doc);
	if (m == NULL)
		return;

    dict = PyModule_GetDict(m);

	QuicklzError = PyErr_NewException("quicklz.error", NULL, NULL);
	PyDict_SetItemString(dict, "error", QuicklzError);

    v = PyString_FromString("Fu Haiping <email:haipingf@gmail.com>");
    PyDict_SetItemString(dict, "__author__", v);
    Py_DECREF(v);

    v = PyString_FromString(QUICKLZ_PYTHON_MODULE_VERSION);
    PyDict_SetItemString(dict, "__version__", v);
    Py_DECREF(v);


	v = PyInt_FromLong(QLZ_COMPRESSION_LEVEL);
	PyDict_SetItemString(dict, "QLZ_COMPRESSION_LEVEL", v);
	Py_DECREF(v);

	v = PyInt_FromLong(QLZ_VERSION_MAJOR);
	PyDict_SetItemString(dict, "QLZ_VERSION_MAJOR", v);
	Py_DECREF(v);

	v = PyInt_FromLong(QLZ_VERSION_MINOR);
	PyDict_SetItemString(dict, "QLZ_VERSION_MINOR", v);
	Py_DECREF(v);


	v = PyInt_FromLong(QLZ_VERSION_REVISION);
	PyDict_SetItemString(dict, "QLZ_VERSION_REVISION", v);
	Py_DECREF(v);

	v = PyInt_FromLong(sizeof(qlz_state_compress));
	PyDict_SetItemString(dict, "QLZ_STATE_COMPRESS_SIZE", v);
	Py_DECREF(v);

	v = PyInt_FromLong(sizeof(qlz_state_decompress));
	PyDict_SetItemString(dict, "QLZ_STATE_DECOMPRESS_SIZE", v);
	Py_DECREF(v);
#if defined(QLZ_MEMORY_SAFE)
	v = PyInt_FromLong(QLZ_MEMORY_SAFE);
	PyDict_SetItemString(dict, "QLZ_MEMORY_SAFE", v);
	Py_DECREF(v);
#else
	v = PyInt_FromLong(0L);
	PyDict_SetItemString(dict, "QLZ_MEMORY_SAFE", v);
	Py_DECREF(v);
#endif

}
