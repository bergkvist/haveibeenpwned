#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "binary-lookup.c"

PyDoc_STRVAR(
    sha1_leak_count_doc,
    "Check if a SHA1 hash is included in a public password leak"
);
static PyObject* sha1_leak_count(PyObject* self, PyObject* args) {
    char uppercase_sha1_hash[40];
    const char* sha1_hash;
    char* sha1_filename;
    char* count_filename;
    PyObject* py_sha1_filename;
    PyObject* py_count_filename;

    int status = PyArg_ParseTuple(args, "sO&O&:number_of_leaks",
        &sha1_hash,
        PyUnicode_FSConverter, &py_sha1_filename,
        PyUnicode_FSConverter, &py_count_filename);
    if (status == 0) return NULL;
    if (strlen(sha1_hash) != 40) {
        PyErr_SetString(PyExc_TypeError, "SHA1 hash must be exactly 40 characters long");
        return NULL;
    }
    sha1_filename = PyBytes_AsString(py_sha1_filename);
    if (access(sha1_filename, F_OK) != 0) {
        PyErr_SetString(PyExc_FileNotFoundError, sha1_filename);
        return NULL;
    }
    count_filename = PyBytes_AsString(py_count_filename);
    if (access(count_filename, F_OK) != 0) {
        PyErr_SetString(PyExc_FileNotFoundError, count_filename);
        return NULL;
    }

    memcpy(uppercase_sha1_hash, sha1_hash, 40);
    str_to_upper(uppercase_sha1_hash);
    size_t result = number_of_matches(uppercase_sha1_hash, sha1_filename, count_filename);
    return Py_BuildValue("K", result);
}



PyDoc_STRVAR(
    password_leak_count_doc,
    "Check if a password is included in a public password leak"
);
static PyObject* password_leak_count(PyObject* self, PyObject* args) {
    char sha1_hash[40];
    const char* password;
    char* sha1_filename;
    char* count_filename;
    PyObject* py_sha1_filename;
    PyObject* py_count_filename;
    int status = PyArg_ParseTuple(args, "sO&O&:number_of_leaks",
        &password,
        PyUnicode_FSConverter, &py_sha1_filename,
        PyUnicode_FSConverter, &py_count_filename);
    if (status == 0) return NULL;

    sha1_filename = PyBytes_AsString(py_sha1_filename);
    if (access(sha1_filename, F_OK) != 0) {
        PyErr_SetString(PyExc_FileNotFoundError, sha1_filename);
        return NULL;
    }
    count_filename = PyBytes_AsString(py_count_filename);
    if (access(count_filename, F_OK) != 0) {
        PyErr_SetString(PyExc_FileNotFoundError, count_filename);
        return NULL;
    }
    hash_password(&sha1_hash[0], password);
    size_t result = number_of_matches(sha1_hash, sha1_filename, count_filename);
    return Py_BuildValue("K", result);
}

static PyMethodDef Methods[] = {
    {"sha1_leak_count", sha1_leak_count, METH_VARARGS, sha1_leak_count_doc},
    {"password_leak_count", password_leak_count, METH_VARARGS, password_leak_count_doc}
};

static struct PyModuleDef pwnedlookup = {
    PyModuleDef_HEAD_INIT,
    "pwnedlookup",   /* name of module */
    "check if password has been part of leak",
    -1,
    Methods
};

PyMODINIT_FUNC
PyInit_pwnedlookup(void) {
    return PyModule_Create(&pwnedlookup);
}


