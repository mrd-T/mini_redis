#!/bin/bash
cd .. && xmake f -m release && xmake build lsm_pybind
cd -
cd tonilsm
rm -rf build dist tonilsm.egg-info

python3 setup.py sdist bdist_wheel

pip install -e .