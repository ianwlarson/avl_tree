#!/bin/sh

lcov --capture -d . --output-file coverage.info
genhtml coverage.info --output-directory out

