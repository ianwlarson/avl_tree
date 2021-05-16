#!/bin/sh

lcov --capture -d static -d shared --output-file coverage.info
genhtml coverage.info --output-directory out

