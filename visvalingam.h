/**
 * @file visvalingam.h
 * @brief Main interface for the Visvalingam-Whyatt polygon simplification algorithm
 *
 * This module provides the Python-callable interface for polygon simplification
 * using the Visvalingam-Whyatt algorithm. It handles multiple resolution levels
 * efficiently by reusing computed areas.
 */

#ifndef VISVALINGAM_H
#define VISVALINGAM_H

#include <Python.h>
#include <numpy/arrayobject.h>

/**
 * @brief Python-callable function to perform multi-resolution polygon simplification
 *
 * Takes a polygon and list of target resolutions, returns a list of simplified
 * polygons at each requested resolution.
 *
 * @param self Python module self reference (unused)
 * @param args Tuple containing points array and resolutions array
 * @return PyObject* List of numpy arrays containing simplified polygons
 */
PyObject* visvalingam_whyatt_multi_c(PyObject* self, PyObject* args);

#endif /* VISVALINGAM_H */