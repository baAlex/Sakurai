#!/bin/bash

cloc --exclude-dir=libjapan,glad --not-match-f='(state-test\d.c)' ./source/
